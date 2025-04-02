local radius = 50
local speed = 1
local angle = 0

local orbitCenterX = 0
local orbitCenterY = 0
local orbitCenterZ = 0

local entity_posX, entity_posY, entity_posZ = getPosition(3)

local offsetX = entity_posX - orbitCenterX
local offsetZ = entity_posZ - orbitCenterZ


function update(dt)
    angle = angle + dt * speed

    local xp = orbitCenterX + math.cos(angle) * radius
    local zp = orbitCenterZ + math.sin(angle) * radius

    setPosition(xp, orbitCenterY, zp)
    setRotation(0, angle, 0) 
end
