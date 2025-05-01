-- Example Lua Script for Transform component

-- local posx, posy, posz = getPositionOf("EntityId")), also scale/rotation

    local x, y, z = getRotation()
function update(dt)


    y = y + dt

    setRotation(0.0, y, 0.0)

end
