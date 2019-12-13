v, x, y, z = getObjectPosition(id)

wait0 = createCommand_Wait(id, 1.0)


-- rotate around mountain in 3 steps
move1 = createCommand_MoveToTimed(id, 7.5, 140, y, 52)
rotate1 = createCommand_RotateToTimed(id, 7.5, 0.0, 90.0, 0.0)

addSerialCommand(wait0, move1)
addParallelCommand(move1, rotate1)

move2 = createCommand_MoveToTimed(id, 7.5, 200, y, -66)
rotate2 = createCommand_RotateToTimed(id, 7.5, 0.0, 180.0, 0.0)

addSerialCommand(move1, move2)
addParallelCommand(move2, rotate2)

move3 = createCommand_MoveToTimed(id, 7.5, 140, y-25, -169)
rotate3 = createCommand_RotateToTimed(id, 7.5, 0.0, 260.0, 0.0)

addSerialCommand(move2, move3)
addParallelCommand(move3, rotate3)

move4 = createCommand_MoveToTimed(id, 15.5, -130, 56, -190)

addSerialCommand(move3, move4)

setObjectCommand(id, wait0)