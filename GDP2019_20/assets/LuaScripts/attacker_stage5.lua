v, x, y, z = getObjectPosition(id)
cb1 = getObjectID("cannonball1")
cb2 = getObjectID("cannonball2")
cb3 = getObjectID("cannonball3")
cb4 = getObjectID("cannonball4")

wait0 = createCommand_Wait(id, 0.0)

-- Step 1: set cannonballs to position
cb1set1 = createCommand_SetPosition(cb1, 65.0, 200.45, -51.1)
cb2set1 = createCommand_SetPosition(cb2, 65.0, 200.45, -51.65)
cb3set1 = createCommand_SetPosition(cb3, 65.0, 200.45, -52.2)
cb4set1 = createCommand_SetPosition(cb4, 65.0, 200.45, -52.75)

-- Step 1: Add delay between shots
cb1wait1 = createCommand_Wait(cb1, 0.0)
cb2wait1 = createCommand_Wait(cb2, 1.0)
cb3wait1 = createCommand_Wait(cb3, 2.0)
cb4wait1 = createCommand_Wait(cb4, 3.0)

addParallelCommand(wait0, cb1set1)
addParallelCommand(wait0, cb2set1)
addParallelCommand(wait0, cb3set1)
addParallelCommand(wait0, cb4set1)

addParallelCommand(cb1set1, cb1wait1)
addParallelCommand(cb2set1, cb2wait1)
addParallelCommand(cb3set1, cb3wait1)
addParallelCommand(cb4set1, cb4wait1)

-- Step 2: set to visible AFTER they have waited
cb1visible2 = createCommand_SetVisible(cb1, true)
cb2visible2 = createCommand_SetVisible(cb2, true)
cb3visible2 = createCommand_SetVisible(cb3, true)
cb4visible2 = createCommand_SetVisible(cb4, true)

addSerialCommand(cb1wait1, cb1visible2)
addSerialCommand(cb2wait1, cb2visible2)
addSerialCommand(cb3wait1, cb3visible2)
addSerialCommand(cb4wait1, cb4visible2)

-- Step 3 Fire across in a straight line
cb1move3 = createCommand_MoveToTimed(cb1, 5.0, 129.0, 191, -54.1)
cb2move3 = createCommand_MoveToTimed(cb2, 5.0, 129.0, 191, -55.65)
cb3move3 = createCommand_MoveToTimed(cb3, 5.0, 129.0, 191.5, -56.2)
cb4move3 = createCommand_MoveToTimed(cb4, 5.0, 129.0, 192, -57.75)

addSerialCommand(cb1visible2, cb1move3)
addSerialCommand(cb2visible2, cb2move3)
addSerialCommand(cb3visible2, cb3move3)
addSerialCommand(cb4visible2, cb4move3)

-- Step 4: make them invisible, reset position and wait for reload to fire again
cb1visible4 = createCommand_SetVisible(cb1, false)
cb2visible4 = createCommand_SetVisible(cb2, false)
cb3visible4 = createCommand_SetVisible(cb3, false)
cb4visible4 = createCommand_SetVisible(cb4, false)

addSerialCommand(cb1move3, cb1visible4)
addSerialCommand(cb2move3, cb2visible4)
addSerialCommand(cb3move3, cb3visible4)
addSerialCommand(cb4move3, cb4visible4)

cb1set4 = createCommand_SetPosition(cb1, 65.0, 200.45, -51.1)
cb2set4 = createCommand_SetPosition(cb2, 65.0, 200.45, -51.65)
cb3set4 = createCommand_SetPosition(cb3, 65.0, 200.45, -52.2)
cb4set4 = createCommand_SetPosition(cb4, 65.0, 200.45, -52.75)

cb1wait4 = createCommand_Wait(cb1, 15 + 0.0)
cb2wait4 = createCommand_Wait(cb2, 15 + 0.0)
cb3wait4 = createCommand_Wait(cb3, 15 + 0.0)
cb4wait4 = createCommand_Wait(cb4, 15 + 0.0)

addParallelCommand(cb1visible4, cb1set4)
addParallelCommand(cb2visible4, cb2set4)
addParallelCommand(cb3visible4, cb3set4)
addParallelCommand(cb4visible4, cb4set4)

addParallelCommand(cb1visible4, cb1wait4)
addParallelCommand(cb2visible4, cb2wait4)
addParallelCommand(cb3visible4, cb3wait4)
addParallelCommand(cb4visible4, cb4wait4)

-- Step 5: fire again
cb1visible5 = createCommand_SetVisible(cb1, true)
cb2visible5 = createCommand_SetVisible(cb2, true)
cb3visible5 = createCommand_SetVisible(cb3, true)
cb4visible5 = createCommand_SetVisible(cb4, true)

addSerialCommand(cb1wait4, cb1visible5)
addSerialCommand(cb2wait4, cb2visible5)
addSerialCommand(cb3wait4, cb3visible5)
addSerialCommand(cb4wait4, cb4visible5)

cb1move5 = createCommand_MoveToTimed(cb1, 5.0, 129.0, 199, -54.1)
cb2move5 = createCommand_MoveToTimed(cb2, 5.0, 129.0, 199.33, -55.65)
cb3move5 = createCommand_MoveToTimed(cb3, 5.0, 129.0, 199.66, -56.2)
cb4move5 = createCommand_MoveToTimed(cb4, 5.0, 129.0, 200, -57.75)

addParallelCommand(cb1visible5, cb1move5)
addParallelCommand(cb2visible5, cb2move5)
addParallelCommand(cb3visible5, cb3move5)
addParallelCommand(cb4visible5, cb4move5)

-- Step 6: invisible
cb1visible6 = createCommand_SetVisible(cb1, false)
cb2visible6 = createCommand_SetVisible(cb2, false)
cb3visible6 = createCommand_SetVisible(cb3, false)
cb4visible6 = createCommand_SetVisible(cb4, false)

addSerialCommand(cb1move5, cb1visible6)
addSerialCommand(cb2move5, cb2visible6)
addSerialCommand(cb3move5, cb3visible6)
addSerialCommand(cb4move5, cb4visible6)


setObjectCommand(id, wait0)