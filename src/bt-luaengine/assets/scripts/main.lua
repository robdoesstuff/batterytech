-- Game Script

addAssimp("models/Seymour.bai")
addTexture("models/Seymour.jpg")
 
execScript("scripts/util.lua")
 
 
Character = table.copy(GameObject)
function Character.new()
    self = allocMeta(Character.createInstance(), Character)
    self:cInit()
    self:anim_allocAnimations(1)
    self:anim_initDynamic(0, "models/Seymour.bai", nil)
    self.animPos = 0
    return self
end
 
function Character:update(tickDelta)
	logmsg("Character:update")
	self.animPos = self.animPos + tickDelta
	self:anim_interpolate(0, self.animPos)
	logmsg("Character:update - DONE")
end
 
function Character:render()
    local x = 0
    local y = -20
    local z = 1.05
    local scale = .75
    local idx = game:renderAssimpM(self, 0, "models/Seymour.bai", nil, nil, true, 1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1, scale,scale,scale, 180)
end
 
 
function Game:init()
end

function Game:update(tickDelta)
	if not self.character then
		logmsg("Character created in Game:update")
		self.character = Character.new()
		self.character:update(tickDelta)
		logmsg("Character created and updated")
	end
	self.character:update(tickDelta)
end
 
 
function Game:render(state)
	setCameraParams(0, 0, 5, 90, 180)
	setCameraNearFarFOV(1, 200, 60)
	game:setGlobalLightEnabled(false)
    game:setFogParams(1, 25, .3,.3,.3,1)
    if self.character then
        self.character:render()
    end
end
 
logmsg("Game:getInstance()")
game = allocMeta(Game.getInstance())