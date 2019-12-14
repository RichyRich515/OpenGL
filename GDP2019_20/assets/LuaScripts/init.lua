v, x, y, z = getObjectPosition(id);

--move1 = createCommand_MoveToTimed(id, 10.0, 0.0, y, 1.0, true, true)
rotate1 = createCommand_RotateToTimed(id, 10.0, 0.0, 90.0, 0.0, true, true)

--addParallelCommand(move1, rotate1)

rotate2 = createCommand_RotateToTimed(id, 20.0, 0.0, -90.0, 0.0, true, true)

addSerialCommand(rotate1, rotate2)

setObjectCommand(id, rotate1)