v, x, y, z = getObjectPosition(id)

-- move forward
move1 = createCommand_MoveToTimed(id, 5.0, -166, 50, -196)

-- get hit by cannonball sink into sea
move2 = createCommand_MoveToTimed(id, 10.0, -200, -15, -205)
rotate2 = createCommand_RotateToTimed(id, 5.0, 45.0, 270.0, 0.0)

addSerialCommand(move1, move2)
addParallelCommand(move2, rotate2)

setObjectCommand(id, move1)