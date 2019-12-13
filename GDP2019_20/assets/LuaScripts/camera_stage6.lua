v, x, y, z = getObjectPosition(id)

move1 = createCommand_MoveToTimed(id, 4.0, 250.0, y, -160.0)
rotate1 = createCommand_RotateToTimed(id, 4.0, 10.0, 300, 0.0)

addParallelCommand(move1, rotate1)

wait2 = createCommand_Wait(id, 18.5)

addSerialCommand(move1, wait2)

move3 = createCommand_MoveToTimed(id, 17.5, -71.0, 74, -190.0)
rotate3 = createCommand_RotateToTimed(id, 17.5, 10.0, 250, 0.0)

addParallelCommand(move3, rotate3)
addSerialCommand(wait2, move3)

setObjectCommand(id, move1)