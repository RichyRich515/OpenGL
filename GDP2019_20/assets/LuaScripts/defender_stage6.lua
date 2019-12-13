v, x, y, z = getObjectPosition(id)

move1 = createCommand_MoveToTimed(id, 10.0, 85, 110, -170)
rotate1 = createCommand_RotateToTimed(id, 3.0, 10, 180, 0)
addParallelCommand(move1, rotate1)

move2 = createCommand_MoveToTimed(id, 5.0, 25, 100, -200)
rotate2 = createCommand_RotateToTimed(id, 5.0, 0, 260, 0)

addSerialCommand(move1, move2)
addParallelCommand(move2, rotate2)

move3 = createCommand_MoveToTimed(id, 25.0, -190, 45, -213)
addSerialCommand(move2, move3)

setObjectCommand(id, move1)