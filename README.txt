Richard Woods

Physics Project 4
OpenGL 3D engine physics simulation, spheres, planes, and cloths

Compile and run in Visual Studio 2019 Community
for best performance use Release x64

Scene is defined in /assets/scenes/scene1.json

- GDP2019_20
	the game code, handles creation/destruction of objects and drawing of them on screen

	key files:
		- main.cpp
		- cPhysicsGameObject
		- cPhysicsManager

- MyPhysicsLib
	the physics library, all the integration, collision detection and response code is here.

- MyPhysicsWrapper
	the wrapper code for MyPhysicsLib

- MyPhysicsInterfaces
	the interface code my game code sees to connect to the physics engine


Marks:
	!! Ugly code
		Should all be satisfactory.

	1. Cloth
		configurable via JSON (scene1.json, the last gameobject in the list in this example)
		cloth reacts to objects passing through it and always returns to a "resting" position

	2. Broad phase
		A sphere -> X check, see the end of the IntegrateSoftBody function to see how I calculate the bounding sphere

	3. Wind 
		Applied as in a sin wave force over time

	5. Bonus
		N/A


Notes: 
	- The currently controlled ball is colored white
	- The cloth *can* be unstable on slower computers, I'm not sure why.
		If that is the case you can try to turn down the number of nodes across/down until it becomes more stable.
	- The wind only affects the cloth and is on a sin wave pattern from on to off to on etc.


Controls:
	- W A S D 
		Apply forces to currently selected Ball

	- Space
		Change currently selected ball

	- Q E
		Rotate camera left and right

	- R F
		Zoom in and out