Richard Woods
Physics Final Project
FPS tech demo


WARNING:
	Check volume before starting, might be a bit loud... 
	Start low and turn up after testing the gunshot sound


1. Implemented at least 3 shapes not covered in class (ball/plane)
	Box = crates, train, container, rock thing
	Cylinder = barrels, pipe
	Cone = pylon, bell
	(Technically also a capsule, as the player controller utilizes one as it's collision shape)
	

2. Implemented 3 constraints
	Train = Slider Constraint, stays on the rails and stops at the ends
	Bell = Hinge Constraint, only swings along the one axis
	Pipe = Cone Twist Constraint, spins and swings around freely in all directions but cannot translate


3. btKinematicCharacterController (see controls)
	able to move in any direction using standard FPS control/feel.
	able to jump off the ground and off other rigidbodies


4. N/a (No character model/animations)
	:(


5. Weapon feature (see controls)
	interact with the scene by shooting a small high velocity sphere at the direction facing
	sphere is removed from the world after a couple seconds
	has high impact impulse against other objects due to having somewhat larger mass


6. Implemented collision feedback influencing the game
	Bullets knock over objects and push them around, can also walk into them but this hardly moves them.. 
		Character controller seems to be lacking in the collision detection/response for this regard.
	Powerup (ghost object) increases bullet size and rate of fire, and plays sound when picked up 
		(to the left of your starting position, semi-transparent purple sphere)
	Sound when certain objects are hit with bullet 
		(Only the bell, works a little strange, I think I have my collision listener setup done incorrectly...)


Controls
	WASD to move
	Space to jump
	Aim with mouse
	Left mouse button to shoot (Can hold down to fire automatically)