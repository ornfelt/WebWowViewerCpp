//
// Fork-local: WoW style third person camera that orbits a player on the ground.
//

#ifndef WOWVIEWERLIB_THIRDPERSONCAMERA_H
#define WOWVIEWERLIB_THIRDPERSONCAMERA_H

#ifdef USE_CUSTOM_CHANGES

#include <mathfu/vector.h>
#include "mathfu/glsl_mappings.h"
#include "CameraInterface.h"
#include "../customPlayer/CustomPlayerState.h"

// Orbits the shared CustomPlayerState instead of flying freely:
//
//   left drag    orbits the camera around the player, the player keeps facing where it did
//   right drag   orbits the camera and turns the player to face the same way
//   W / S        walk forward / back along the player's facing
//   A / D        turn the player, the camera swings along to stay behind
//   Q / E        strafe
//   wheel        pulls the boom in and out
//
// Left and right drag arrive as different IControllable calls already (addHorizontalViewDir
// and addVerticalViewDir for left, addCameraViewOffset for right), so telling them apart
// needs no change in the input handling.
//
// There is no camera collision: the boom keeps its length even when it ends up inside a
// hill. Adding that would need a terrain raycast, which lives on the Map side.
class ThirdPersonCamera : public ICamera {
public:
    explicit ThirdPersonCamera(HCustomPlayerState playerState) : m_playerState(std::move(playerState)) {};

private:
    HCustomPlayerState m_playerState;

    //Where the camera sits relative to the player. Yaw is the direction from the player
    //towards the camera, so it is the player's facing + 180 while the camera is behind.
    float m_boomYawDeg = 180.0f;
    float m_boomPitchDeg = 20.0f;
    float m_boomLength = 12.0f;

    //Accumulated mouse movement, applied in tick() the same way FirstPersonCamera does it
    float delta_x = 0;
    float delta_y = 0;
    //Set by a right drag: the player turns to face wherever the camera ends up looking
    bool m_turnPlayerWithCamera = false;

    float MDDepthPlus = 0;
    float MDDepthMinus = 0;
    float MDTurnLeft = 0;
    float MDTurnRight = 0;
    float MDStrafeLeft = 0;
    float MDStrafeRight = 0;

    float m_moveSpeed = 1.0f / 30.0f;
    bool updatedAtLeastOnce = false;

    //Derived in tick()
    mathfu::vec4 camera = {0, 0, 0, 1};
    mathfu::vec3 lookAt = {0, 0, 0};
    mathfu::vec3 upVector = {0, 0, 1};
    mathfu::mat4 lookAtMat = {};
    mathfu::mat4 invTranspViewMat = {};
    mathfu::vec4 interiorDirectLightDir = {0, 0, 0, 0};

public:
    //Implemented IControllable
    void addHorizontalViewDir(float val) override;
    void addVerticalViewDir(float val) override;
    void addForwardDiff(float val) override;
    void startMovingForward() override;
    void stopMovingForward() override;
    void startMovingBackwards() override;
    void stopMovingBackwards() override;
    //A and D: turn the player rather than strafe, the way WoW binds them
    void startStrafingLeft() override;
    void stopStrafingLeft() override;
    void startStrafingRight() override;
    void stopStrafingRight() override;
    //Q and E: the actual strafe
    void startMovingUp() override;
    void stopMovingUp() override;
    void startMovingDown() override;
    void stopMovingDown() override;
    void stopAllMovement() override;

    void zoomInFromMouseScroll(float val) override;
    void zoomInFromTouch(float val) override;
    void addCameraViewOffset(float x, float y) override;

    //Sets where the player stands, not where the camera is: the camera is always derived
    //from the player. Used when a map scene opens at given coordinates.
    void setCameraPos(float x, float y, float z) override;
    void getCameraPosition(float *position) override;
    void setCameraLookAt(float x, float y, float z) override;

    //Implemented ICamera
    HCameraMatrices getCameraMatrices(float fov,
                                      float canvasAspect,
                                      float nearPlane,
                                      float farPlane) override;
    void tick(animTime_t timeDelta) override;
    float getMovementSpeed() override;
    void setMovementSpeed(float value) override;
};

#endif //USE_CUSTOM_CHANGES

#endif //WOWVIEWERLIB_THIRDPERSONCAMERA_H
