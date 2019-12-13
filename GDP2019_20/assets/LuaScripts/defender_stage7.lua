v, x, y, z = getObjectPosition(id)

cb = getObjectID("cannonball1")

move1 = createCommand_MoveToTimed(id, 20.0, -360, y, -243)


-- Spawn and shoot cannonball backwards
cbvisible2 = createCommand_SetVisible(cb, true)
cbset2 = createCommand_SetPosition(cb, -187, 46, -212)

cbmove2 = createCommand_MoveToTimed(cb, 5.0, -166, 56, -196)

addParallelCommand(move1, cbvisible2)
addParallelCommand(cbvisible2, cbset2)
addParallelCommand(cbvisible2, cbmove2)

-- remove cannonball
cbvisible3 = createCommand_SetVisible(cb, false)
addSerialCommand(cbmove2, cbvisible3)


setObjectCommand(id, move1)