#pragma once

/*
         _____ _
        | ___ \ |             (_)
        | |_/ / |__  _   _ ___ _  ___ ___
        |  __/| '_ \| | | / __| |/ __/ __|
        | |   | | | | |_| \__ \ | (__\__ \
        \_|   |_| |_|\__, |___/_|\___|___/
                      __/ |
                     |___/

    - Who?
        - Created by Richard Woods
    
    - What? 
        - MyPhysicsLib
            - A simple physics simulation library
            - Depends on glm
            - Created for the GDP program at Fanshawe College
            - Collision detection code used from Christer Ericson's "Real-Time Collision Detection" textbook
            
    - Why?
        - To simulate real time collision detection and response between simple shapes
            - This library was not created with super efficiency in mind
            - It is for learning and understanding how physics simulation libraries *could* be implemented
        - Intended to be used with my GDP project
            - Custom wrapper code created for this specific case

    - How?
        - Ideally you create a wrapper class to wrap over the physics world and handle creating/removing rigidbodies for the simulation
            - cWorld will never delete a cRigidBody
                - Must be handled either in the wrapper or the game code, depending on your implementation
            - Shapes *can* be shared between multiple cRigidBody instances
                - This is useful for multiple of the same shape to reduce memory usage
                - In my implementation of a wrapper I gave each "physics object" it's own shape and rigidbody

    - Where?
        - Here

    - When?
        - 2020-02-08
 */

// All the files required to make full use of this library:
#include "eShapeType.h"  // enum class for all supported shape types
#include "iShape.h"      // iShape is the base interface class for all shapes
#include "cRigidBody.h"  // cRigidBody is this library's rigid body representation
#include "shapes.h"      // Concrete classes for all supported shape types
#include "cWorld.h"      // cWorld is the primary container and operator of a simulation