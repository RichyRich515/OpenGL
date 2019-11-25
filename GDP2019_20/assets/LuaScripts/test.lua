-- test script

drawDebugLine(0.0, 0.0, 0.0, 
			math.sin(tt) * 10.0, 10.0, math.cos(tt) * 10.0,
			1.0, 1.0, 0.0, 
			0.0)


drawDebugLine(0.0, 0.0, 0.0, 
			math.sin(tt) * -10.0, 10.0, math.cos(tt) * -10.0,
			0.0, 1.0, 1.0, 
			0.0)


drawDebugTri(0.0, 0.0, 0.0, 
			math.sin(tt) * -10.0, -10.0, math.cos(tt) * -10.0,
			math.sin(tt) * 10.0, -10.0, math.cos(tt) * 10.0,
			1.0, 0.0, 1.0, 
			0.0)

-- print(tostring(id))

v, x, y, z = getObjectPosition(id)
-- print(tostring(z))
if (v) then
	v = setObjectPosition(id, x, y, z + (1 * dt))
end