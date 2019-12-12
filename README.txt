Richard Woods
Graphics Project 2

use Mouse to look around
use WASD to move
use Space to ascend
use C to descend

Press tilde/grave (~/`) to activate Normal as Color rendering 

Press V to toggle the headlights on the tanks and the Laser Beam on the alien ship.

Use < or > to select object (see Title bar of program for info)

Press F1 to save to Scene file, appends UNIX timestamp to end of file name so you can make multiple saves

Hold CTRL then use same controls as Camera to move selected Object around
Hold SHIFT then use same controls as camera to mvoe selected Light around




- Varying transparencies on the Carriers above
- Sorted by distance to camera
- Multi texture
	Water(blended two waters and heightmap), 
	Cruiser (Blended shielding texture), 
	Terrain (texture and heightmap), 
	Carrier (discard map)

- Lights
	Spot light
	Point lights
	Directional light
	Ambient lighting
	
trivial combining: blending textures
Non-trivial combining: discard map, height map