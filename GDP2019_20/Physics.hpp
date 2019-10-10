// Physics.hpp
// Physics stuff
// Richard Woods
// 2019-09-19
#pragma once

#include <vector>
#include "cGameObject.hpp"
#include "DebugRenderer/cDebugRenderer.h"

constexpr float MAX_PHYSICS_DELTA_TIME = 0.17f;

void physicsUpdate(std::vector<cGameObject*>& vecGameObjects, glm::vec3 gravity, float dt, cDebugRenderer* debugRenderer, bool debug_mode);