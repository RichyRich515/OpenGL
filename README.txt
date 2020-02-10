Richard Woods

Physics Project 2
OpenGL 3D engine physics simulation, spheres and planes

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


Notes: 
	- The currently controlled ball is colored white

	- I was unable to finish an RK4 integrator for this project so I have used semi-implicit euler instead

Controls:
	- W A S D 
		Apply forces to currently selected Ball

	- Space
		Change currently selected ball

	- Q E
		Rotate camera left and right

	- R F
		Zoom in and out



My favourite thing to do is get control of the smallest ball and fling it around the scene at insane speed.