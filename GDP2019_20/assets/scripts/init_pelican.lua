v, x, y, z = getObjectPosition(id);


-- Follow pillar_of_autumn for 55 seconds
target = getObjectID("pillar_of_autumn");

follow1 = createCommand_FollowTimed(id, target, 55.0, 5.0, 
	2.0, 5.0, 
	0,0,0,
	false)

-- set position to 0, 35, 0
set2 = createCommand_SetPosition(id, 0, 35, 0)
addSerialCommand(follow1, set2)

-- wait 5 seconds
wait3 = createCommand_Wait(id, 5.0)
addSerialCommand(set2, wait3)

-- become invisible
visible4 = createCommand_SetVisible(id, false)
addSerialCommand(wait3, visible4)


setObjectCommand(id, follow1)