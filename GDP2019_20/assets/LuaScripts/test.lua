v, x, y, z = getObjectPosition(id)
--print(tostring(z))
if (v) then
	drawDebugLine(x, y, z, x + 1.0, y, z, 1.0, 0.0, 0.0, 0.0)
	drawDebugLine(x, y, z, x, y + 1.0, z, 0.0, 1.0, 0.0, 0.0)
	drawDebugLine(x, y, z, x, y, z + 1.0, 0.0, 0.0, 1.0, 0.0)
end