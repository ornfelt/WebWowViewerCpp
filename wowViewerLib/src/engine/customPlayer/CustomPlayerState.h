//
// Fork-local: state shared between the third person camera and the player model Map draws.
//

#ifndef WOWVIEWERLIB_CUSTOMPLAYERSTATE_H
#define WOWVIEWERLIB_CUSTOMPLAYERSTATE_H

#ifdef USE_CUSTOM_CHANGES

#include <memory>
#include "mathfu/glsl_mappings.h"

// The player the third person camera orbits around, and that Map draws a model for.
//
// Ownership of the fields is split, because neither side can do the other's job:
// ThirdPersonCamera drives position.x/y and facingRad from the input it receives, but it
// cannot sample the terrain; Map can, so it writes position.z back every frame. The camera
// reads that height one frame later, which is not noticeable while walking.
struct CustomPlayerState {
    // World position of the player's feet.
    mathfu::vec3 position = {0.0f, 0.0f, 0.0f};

    // Where the player faces, in radians, 0 = +X. Same convention as the GameObject
    // placements, so it can be handed to M2Object::createPlacementMatrix as degrees.
    float facingRad = 0.0f;

    // False until Map has found an ADT under the player to take a height from. While it is
    // false position.z is whatever the camera was last given, so the model floats.
    bool heightIsFromTerrain = false;
};

typedef std::shared_ptr<CustomPlayerState> HCustomPlayerState;

// Startup value for Config::customThirdPersonCamera:
//   true  - WoW style third person: the camera orbits a player model that walks the terrain.
//   false - free flying camera, with the player model kept in front of it.
// The config field can be flipped at runtime, this is only what it starts at.
static const bool CUSTOM_PLAYER_THIRD_PERSON_DEFAULT = true;

#endif //USE_CUSTOM_CHANGES

#endif //WOWVIEWERLIB_CUSTOMPLAYERSTATE_H
