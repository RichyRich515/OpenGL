Richard Woods
OpenGL 3D
Gamejam final for GDP
Graphics, Animation and Gems

Compile and run in VS2019 with Release x64 (or Debug x64 but performance will be poor)


Things completed in no particular order:

Texture "macrovariantions"
	modulate a texture based on another texture in order to make it harder to see repeating patterns in the texture
	see the ground texture for this in action, or the container

Edge Detection
	A combination of color change detection and frag's world normal to eye vector

Full screen blur
	using a 7 length kernel, two pass blur is applied to entire output (horizontal then vertical)

Night vision
	Makes everything tinted green and brightens dark areas.. really nothing too special

Deferred Rendering
	using a 2.5 pass system
		1st pass: render all opaque objects to the color, normal, world position and depth buffers
		2nd pass: take the buffers and apply lighting
		2.5th pass: swap the depth buffers and render transparent objects

Particles
	transparent smoke (!see deferred rendering!)

Uber Shader
	everything shader related is all in one shader program (fragmentShader01.glsl and vertexShader01.glsl)

LOD/Level of Detail
	Swap the model to a lower resolution version on the fly to reduce load on GPU when high poly object is far away

Multiple Animations
	different animations play based on input and status of player
		Walking, strafing, backwalking, falling

Controls:

	WASD
		Move

	Space
		Jump

	Left Click
		Shoot

	C
		Toggle between first and second person

	V
		Toggle edge detection (use '<' or '>' to change the threshold)

	B
		Toggle blur mode

	N
		Toggle night vision mode

	G (hold)
		Speed up the game's DT by a factor of 10 (BUGGY!!)
	
