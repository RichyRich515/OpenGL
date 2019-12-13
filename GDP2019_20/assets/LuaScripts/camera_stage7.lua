v, x, y, z = getObjectPosition(id)

move1 = createCommand_MoveToTimed(id, 5.0, -145, 53, -200)

move2 = createCommand_MoveToTimed(id, 10.0, -145, 10.0, -200)
rotate2 = createCommand_RotateToTimed(id, 10.0, 20, 260, 0)

addSerialCommand(move1, move2)
addParallelCommand(move2, rotate2)

rotate3 = createCommand_RotateToTimed(id, 5.0, -10, 255, 0)

addSerialCommand(move2, rotate3)

setObjectCommand(id, move1)