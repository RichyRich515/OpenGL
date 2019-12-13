command = createCommand_MoveToTimed(id, 5.0, 0.0, 0.0, 10.0)
command2 = createCommand_MoveToTimed(id, 10.0, 5.0, 0.0, 0.0)
command3 = createCommand_MoveToTimed(id, 15.0, 0.0, 15.0, 0.0)

command4 = createCommand_RotateToTimed(id, 5.0, 0.0, 90.0, 0.0)

setParallelCommand(command, command2)
setParallelCommand(command, command3)
setParallelCommand(command, command4)

setObjectCommand(id, command)