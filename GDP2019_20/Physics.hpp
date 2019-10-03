// Physics.hpp
// Physics stuff
// Richard Woods
// 2019-09-19
#pragma once

#include <vector>
#include "cGameObject.hpp"

constexpr float MAX_PHYSICS_DELTA_TIME = 0.17;

void physicsUpdate(std::vector<cGameObject*>& vecGameObjects, float dt);