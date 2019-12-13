v, x, y, z = getObjectPosition(id)
-- need to ease out
move1 = createCommand_MoveToTimed(id, 5.0, -256.0, y, -235.0)
rotate1 = createCommand_RotateToTimed(id, 5.0, 0.0, 50.0, 0.0)
addParallelCommand(move1, rotate1)

setObjectCommand(id, move1)