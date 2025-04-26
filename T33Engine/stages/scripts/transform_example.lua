-- Example Lua Script for Transform component

-- local posx, posy, posz = getPositionOf("EntityId")), also scale/rotation

function update(dt)

    local x, y, z = getPosition()

    x = x + dt

    setPosition(x, y, z)

end
