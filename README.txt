Richard Woods

AI project 2
Formations and Boids

Compile and run in Visual Studio 2019 Community
for best performance use Release x64

the important files are main.cpp, cCoordinateComponent.cpp, and cBoidGameObject.cpp

Controls:
	WASD Space/C
		move camera around and up/down

	Mouse move
		aim camera around

	G hold
		increase delta time by factor of 10

	Pressing number keys:

	1  Circle formation
	2  V formation
	3  Square formation
	4  Line formation
	5  Two Rows formation
	6  Flocking
		U/J: inc/dec separation
		I/K: inc/dec alighment
		O/L: inc/dec cohesion
	7  Stop flocking return to last formation
	8  Path following
	9  Reverses direction of path following (once next node is hit)
	0  Stop path following and return to last formation
	-/+  Flock while following path