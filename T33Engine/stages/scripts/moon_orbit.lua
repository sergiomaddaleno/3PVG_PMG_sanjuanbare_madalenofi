local earthId = 3
local radius = 5.0
local speed = 1.0
local angle = math.random() * 2 * math.pi

function update(dt)
    local earthX, earthY, earthZ = getPositionOf(earthId)

    angle = (angle + dt * speed) % (2 * math.pi)

    local xp = earthX + math.cos(angle) * radius
    local zp = earthZ + math.sin(angle) * radius
    local yp = earthY

    setPosition(xp, yp, zp)
    setRotation(0, angle, 0)
end
