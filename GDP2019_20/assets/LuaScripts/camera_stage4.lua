v, x, y, z = getObjectPosition(id)

wait1 = createCommand_Wait(id, 10.0, x, y, z)

move2 = createCommand_MoveToTimed(id, 30.0, 120, 190, -120)
rotate2 = createCommand_RotateToTimed(id, 30.0, 0.0, 350.0, 0.0)
addParallelCommand(move2, rotate2)

addSerialCommand(wait1, move2)

setObjectCommand(id, wait1)