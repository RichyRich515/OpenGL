Richard Woods

gems project 2
threading in game engine

For this project I decided to multi-thread loading in textures. I setup a sample scene with 200 textured cubes, the textures get loaded on background threads while the main program loop continues. 
Each iteration of the program loop it checks if a new texture is ready to load to the GPU (Since this can only be done on the main thread, thanks OpenGL...)


From my testing, on x64 release mode (please dont use debug mode unless you want your CPU to cry). 
The non-threaded version from launch to "playing" was ~5-6 seconds.
On the multi threaded version it is ~1 second.


If the cloth is too intensive you can turn down the number of nodes on it in the "assets/scenes/scene.json"
just ctrl+f "cloth" and find nodesAcross/nodesDown



Key points of program: 

Line 246: Threaded_LoadTexturesFromFiles() 
	The function that is called by the threads to load the textures. Notice the use of a LOCK (mutex) to prevent double loading of the same texture, etc.

Line 569: starting the threads to load the textures.
	I do this right before the main loop begins executing so you can see the textures loading in the background during execution, even if it's only for a short bit.
	
Line 585: Right at the start of the game loop 
	I check if a new texture is ready to load to the GPU, if so I load only one at a time up. If I tried to load the entire array up it would freeze for a short while and then continue.. which defeats the purpose of background threading, again notice the use of LOCK (mutex) to prevent erroneous removals from the vector, etc.