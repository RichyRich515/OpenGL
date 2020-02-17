Richard Woods

Physics Project 2
OpenGL 3D engine physics simulation, spheres and planes

Compile and run in Visual Studio 2019 Community
for best performance use Release x64

To change the physics library you must edit line 55 of main.cpp
from: //#define MY_PHYSICS
to: #define MY_PHYSICS

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

- BulletPhysicsWrapper
	the wrapper code for Bullet's physics library

Notes: 
	- The currently controlled ball is colored white

Controls:
	- W A S D 
		Apply forces to currently selected Ball

	- Space
		Change currently selected ball

	- Q E
		Rotate camera left and right

	- R F
		Zoom in and out