v, x, y, z = getObjectPosition(id)

wait1 = createCommand_Wait(id, 5.0)

move2 = createCommand_MoveToTimed(id, 35.0, x, 206, z)

addSerialCommand(wait1, move2)

setObjectCommand(id, wait1)