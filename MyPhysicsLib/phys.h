#pragma once

/* 
    - MyPhysicsLib
        - A simple physics simulation library
        - Created by Richard Woods
        - Depends on glm
        - Created for the GDP program at Fanshawe College
        - Collision detection code used from Christer Ericson's "Real-Time Collision Detection" textbook

    - The general approach to creating this library
        - This library was not created with super efficiency in mind
        - It is for learning and understanding how physics simulation libraries *could* be implemented

    - Notes on usage
        - Intended to be used with my GDP project
            - Custom wrapper code created for this specific case
        - cWorld will never delete a cRigidBody
            - Must be handled either in the wrapper or the game code, depending on your implementation
        - Shapes *can* be shared between multiple cRigidBody instances
            - In my implementation of a wrapper I gave each physics object it's own shape
 */

// All the files required to make full use of this library:
#include "eShapeType.h"  // enum class for all supported shape types
#include "iShape.h"      // iShape is the base interface class for all shapes
#include "cRigidBody.h"  // cRigidBody is this library's rigid body representation
#include "shapes.h"      // Concrete classes for all supported shape types
#include "cWorld.h"      // cWorld is the primary container and operator of a simulation