-- Example Lua Script for Transform component

local sx, sy, sz = getScale()
setScale(1, 2, 1)

local yRotation = 0

function update(dt)

    local x, y, z = getPosition()
    --setPosition(x + dt, 0, 0)

    yRotation = yRotation + dt * 5
    setRotation(0, yRotation, 0)

end
