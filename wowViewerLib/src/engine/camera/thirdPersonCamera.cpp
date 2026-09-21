//
// Fork-local: WoW style third person camera that orbits a player on the ground.
//

#ifdef USE_CUSTOM_CHANGES

#include <algorithm>
#include <cmath>
#include "thirdPersonCamera.h"
#include "../algorithms/mathHelper.h"

//How fast A and D turn the player. tick() is handed milliseconds, not seconds
//(SceneWindow calls it as tick(deltaTime * 1000)), so this is per millisecond:
//0.14 deg/ms is 140 deg/s, about a half turn per second.
static const float TURN_SPEED_DEG_PER_MS = 0.14f;
//Height above the player's feet the camera aims at, so it frames the model rather than the ground
static const float FOCUS_HEIGHT = 2.0f;
//How far the boom can be pulled in and pushed out with the wheel
static const float MIN_BOOM_LENGTH = 1.5f;
static const float MAX_BOOM_LENGTH = 50.0f;
static const float BOOM_ZOOM_STEP = 1.5f;
//Kept short of straight up and straight down, where the look at matrix has no usable up vector
static const float MIN_BOOM_PITCH_DEG = -80.0f;
static const float MAX_BOOM_PITCH_DEG = 85.0f;
//Right drag arrives at 1/8 of the pixel delta while left drag arrives at 1/4, and with the
//vertical axis already negated. This brings a right drag onto the same scale as a left one.
static const float RIGHT_DRAG_TO_LEFT_DRAG = 2.0f;
//Same smoothing FirstPersonCamera applies to accumulated mouse movement
static const float MOUSE_SPRINGINESS = 300.0f;
//The free camera flies rather than walks, so the speed it is handed is far too fast for a
//player on the ground: SceneWindow hands cameras 0.3 units per millisecond, which is 300
//units a second. A WoW player runs at roughly 7 yards a second, so scale the shared speed
//down to about that. Scaling rather than replacing keeps the Movement Speed slider working.
static const float PLAYER_WALK_SPEED_SCALE = 0.023f;

void ThirdPersonCamera::addHorizontalViewDir(float val) {
    delta_x += val;
}

void ThirdPersonCamera::addVerticalViewDir(float val) {
    delta_y += val;
}

void ThirdPersonCamera::addCameraViewOffset(float x, float y) {
    //Right drag: same orbit as a left drag, and the player turns to follow the camera
    delta_x += x * RIGHT_DRAG_TO_LEFT_DRAG;
    delta_y += -y * RIGHT_DRAG_TO_LEFT_DRAG;
    m_turnPlayerWithCamera = true;
}

void ThirdPersonCamera::addForwardDiff(float val) {
    zoomInFromMouseScroll(val);
}

void ThirdPersonCamera::startMovingForward()   { MDDepthPlus = 1; }
void ThirdPersonCamera::stopMovingForward()    { MDDepthPlus = 0; }
void ThirdPersonCamera::startMovingBackwards() { MDDepthMinus = 1; }
void ThirdPersonCamera::stopMovingBackwards()  { MDDepthMinus = 0; }

void ThirdPersonCamera::startStrafingLeft()  { MDTurnLeft = 1; }
void ThirdPersonCamera::stopStrafingLeft()   { MDTurnLeft = 0; }
void ThirdPersonCamera::startStrafingRight() { MDTurnRight = 1; }
void ThirdPersonCamera::stopStrafingRight()  { MDTurnRight = 0; }

void ThirdPersonCamera::startMovingUp()   { MDStrafeLeft = 1; }
void ThirdPersonCamera::stopMovingUp()    { MDStrafeLeft = 0; }
void ThirdPersonCamera::startMovingDown() { MDStrafeRight = 1; }
void ThirdPersonCamera::stopMovingDown()  { MDStrafeRight = 0; }

void ThirdPersonCamera::stopAllMovement() {
    MDDepthPlus = 0;
    MDDepthMinus = 0;
    MDTurnLeft = 0;
    MDTurnRight = 0;
    MDStrafeLeft = 0;
    MDStrafeRight = 0;
}

void ThirdPersonCamera::zoomInFromMouseScroll(float val) {
    m_boomLength = std::min<float>(
        std::max<float>(m_boomLength + val * BOOM_ZOOM_STEP, MIN_BOOM_LENGTH),
        MAX_BOOM_LENGTH);
}

void ThirdPersonCamera::zoomInFromTouch(float val) {
    zoomInFromMouseScroll(val);
}

float ThirdPersonCamera::getMovementSpeed() {
    return m_moveSpeed;
}

void ThirdPersonCamera::setMovementSpeed(float value) {
    m_moveSpeed = value;
}

void ThirdPersonCamera::setCameraPos(float x, float y, float z) {
    if (m_playerState == nullptr) return;

    m_playerState->position = mathfu::vec3(x, y, z);
    //Let Map re-sample the terrain for the new spot
    m_playerState->heightIsFromTerrain = false;
}

void ThirdPersonCamera::getCameraPosition(float *position) {
    position[0] = camera.x;
    position[1] = camera.y;
    position[2] = camera.z;
}

void ThirdPersonCamera::setCameraLookAt(float x, float y, float z) {
    if (m_playerState == nullptr) return;

    //Point the player, and with it the camera, at the given spot
    auto toTarget = mathfu::vec3(x, y, z) - m_playerState->position;
    if (toTarget.xy().LengthSquared() > 0.0f) {
        m_playerState->facingRad = std::atan2(toTarget.y, toTarget.x);
        m_boomYawDeg = fromRadian(m_playerState->facingRad) + 180.0f;
    }
}

void ThirdPersonCamera::tick(animTime_t timeDelta) {
    updatedAtLeastOnce = true;
    if (m_playerState == nullptr) return;

    //Same smoothing as FirstPersonCamera, so a drag feels identical in both modes
    float d = (float) (1.0 - exp(log(0.5) * MOUSE_SPRINGINESS * timeDelta));

    m_boomYawDeg += -delta_x * d;
    //Dragging down swings the camera up and over the player, looking down at it
    m_boomPitchDeg += delta_y * d;

    delta_x = 0;
    delta_y = 0;

    m_boomPitchDeg = std::min<float>(std::max<float>(m_boomPitchDeg, MIN_BOOM_PITCH_DEG), MAX_BOOM_PITCH_DEG);

    //A and D turn the player. The boom turns with it so the camera stays behind.
    float turnDeg = (MDTurnRight - MDTurnLeft) * TURN_SPEED_DEG_PER_MS * (float) timeDelta;
    if (turnDeg != 0.0f) {
        //Turning right means turning clockwise seen from above, which is decreasing yaw
        m_playerState->facingRad -= toRadian(turnDeg);
        m_boomYawDeg -= turnDeg;
    }

    if (m_turnPlayerWithCamera) {
        //The camera looks back down the boom, so that is where the player should face
        m_playerState->facingRad = toRadian(m_boomYawDeg + 180.0f);
        m_turnPlayerWithCamera = false;
    }

    //Walk the player. Movement is in the player's frame, not the camera's, so looking around
    //with a left drag does not change where W goes.
    float walkSpeed = m_moveSpeed * PLAYER_WALK_SPEED_SCALE;
    float forwardDist = (MDDepthPlus - MDDepthMinus) * walkSpeed * (float) timeDelta;
    float strafeDist = (MDStrafeRight - MDStrafeLeft) * walkSpeed * (float) timeDelta;

    if (forwardDist != 0.0f || strafeDist != 0.0f) {
        float facing = m_playerState->facingRad;
        mathfu::vec2 forwardVec(cosf(facing), sinf(facing));
        //Right hand side of the facing direction
        mathfu::vec2 rightVec(forwardVec.y, -forwardVec.x);

        mathfu::vec2 step = forwardVec * forwardDist + rightVec * strafeDist;
        m_playerState->position.x += step.x;
        m_playerState->position.y += step.y;
        //Map re-samples the terrain under the new spot
        m_playerState->heightIsFromTerrain = false;
    }

    //Place the camera on the end of the boom, aimed at the player
    float yaw = toRadian(m_boomYawDeg);
    float pitch = toRadian(m_boomPitchDeg);
    mathfu::vec3 boomDir(
        cosf(pitch) * cosf(yaw),
        cosf(pitch) * sinf(yaw),
        sinf(pitch));

    lookAt = m_playerState->position + mathfu::vec3(0.0f, 0.0f, FOCUS_HEIGHT);
    camera = mathfu::vec4(lookAt + boomDir * m_boomLength, 1.0f);

    lookAtMat = mathfu::mat4::LookAt(
        lookAt,
        camera.xyz(),
        mathfu::vec3(0, 0, 1), 1);

    invTranspViewMat = lookAtMat.Inverse().Transpose();

    mathfu::vec4 interiorSunDir = mathfu::vec4(-0.30822f, -0.30822f, -0.89999998f, 0);
    interiorSunDir = invTranspViewMat * interiorSunDir;
    interiorDirectLightDir = mathfu::vec4(interiorSunDir.xyz().Normalized(), 0.0f);

    upVector = ((invTranspViewMat * mathfu::vec4(0.0f, 0.0f, 1.0f, 0.0f)).xyz()).Normalized();
}

HCameraMatrices ThirdPersonCamera::getCameraMatrices(float fov,
                                                     float canvasAspect,
                                                     float nearPlane,
                                                     float farPlane) {
    if (!updatedAtLeastOnce)
        tick(0.0f);

    HCameraMatrices cameraMatrices = std::make_shared<CameraMatrices>();
    cameraMatrices->perspectiveMat = mathfu::mat4::Perspective(fov, canvasAspect, nearPlane, farPlane, 1.0f);
    cameraMatrices->lookAtMat = lookAtMat;
    cameraMatrices->invTranspViewMat = invTranspViewMat;

    cameraMatrices->cameraPos = camera;
    cameraMatrices->lookAt = mathfu::vec4(lookAt, 1.0f);
    cameraMatrices->viewUp = mathfu::vec4(upVector, 0);
    cameraMatrices->interiorDirectLightDir = interiorDirectLightDir;

    return cameraMatrices;
}

#endif //USE_CUSTOM_CHANGES
