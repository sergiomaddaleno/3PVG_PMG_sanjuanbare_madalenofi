local orbitCenterX = 0
local orbitCenterY = 0
local orbitCenterZ = 0

local entity_posX, entity_posY, entity_posZ = getPosition()

local radiusX = math.sqrt((entity_posX - orbitCenterX)^2 + (entity_posZ - orbitCenterZ)^2)
local radiusZ = radiusX * (0.7 + math.random() * 0.3) 

local speed = 1.0 / math.sqrt((radiusX + radiusZ) / 2.0)
local angle = radiusZ

function update(dt)
    angle = (angle + dt * speed) % (2 * math.pi)

    local xp = orbitCenterX + math.cos(angle) * radiusX
    local zp = orbitCenterZ + math.sin(angle) * radiusZ

    setPosition(xp, 0.0, zp)
    setRotation(0, angle, 0)
end
