v, x, y, z = getObjectPosition(id)

move1 = createCommand_MoveToTimed(id, 10.0, 36.0, y, -110.0)

move2 = createCommand_MoveToTimed(id, 20.0, 64.0, 200, -53.0)
rotate2 = createCommand_RotateToTimed(id, 20, 0, 0, 0)

addParallelCommand(move2, rotate2)

addSerialCommand(move1, move2)

setObjectCommand(id, move1);