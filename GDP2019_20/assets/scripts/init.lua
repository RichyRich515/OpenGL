v, x, y, z = getObjectPosition(id);

-- move and rotate at the same time, both ease in/out
move1 = createCommand_MoveToTimed(id, 10.0, -10.0, y, -5.0, true, true)
rotate1 = createCommand_RotateToTimed(id, 10.0, 0.0, -90.0, 0.0, true, true)

addParallelCommand(move1, rotate1)

-- move along mathematical curve and rotate at same time, rotate ends before movement both ease in/out
curve2 = createCommand_MoveCurveTimed(id, 20, 
	10, y, 10, 
	-5, y+5, 15, 
	true, true)
rotate2 = createCommand_RotateToTimed(id, 15.0, 10.0, 90.0, -15.0, true, true)

addParallelCommand(curve2, rotate2)
addSerialCommand(move1, curve2)

-- rotate back to normal without easing
rotate3 = createCommand_RotateToTimed(id, 2.5, 0,0,0, false, false)

addSerialCommand(curve2, rotate3)

-- move along curve without ease in/out
curve4 = createCommand_MoveCurveTimed(id, 20, 
	-10, 32, -10, 
	10, 47, -10, 
	false, false)

addSerialCommand(rotate3, curve4)

-- wait 10 seconds
wait = createCommand_Wait(id, 10.0)

addSerialCommand(curve4, wait)

-- Move in straight line rotate
move5 = createCommand_MoveToTimed(id, 10.0, -20, 30, 20)
rotate5 = createCommand_RotateToTimed(id, 2.5, 0, 90, 0, true, true)

addSerialCommand(wait, move5)
addParallelCommand(move5, rotate5)

-- add serial command to command that is parallel to previous command
rotate6 = createCommand_RotateToTimed(id, 5.0, 0, -90, 0, true, true)

addSerialCommand(rotate5, rotate6)


setObjectCommand(id, move1)