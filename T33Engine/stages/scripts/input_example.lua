
-- Script Example

local x, y, z = getPosition()
local rotX, rotY, rotZ = getRotation()

function update(dt)
    
    if isKeyPressed("SPACE") then
        setPosition(x, 5, z)
    end

    rotY = rotY + 45 * dt * 0.005
    setRotation(0.0, rotY, 0.0)
end
