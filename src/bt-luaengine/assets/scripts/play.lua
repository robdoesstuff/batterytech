-- Play module

Play = {}

function Play.new()
	local self = table.copy(Play)
	self.boxrot = 0
	self.x = 0
	self.y = 0
	self.dir = 0
	return self
end

function Play:show()
end

function Play:update(tickDelta)
	self.boxrot = self.boxrot + tickDelta * 45
end

function Play:render()
	-- CAMERA PARAM
	-- setCameraParams(0,0,100, 0,0)
	setCameraParams(self.x, self.y, 5, 90, self.dir)
	setCameraNearFarFOV(1, 500, 60)
	game:setShadowLightOrigin(0, 0, 100)
	game:setShadowColorAndEpsilon(0.5, 0.5, 0.5, 0.021)
	game:setShadowLightFrustumNearFar(80, 110)
	game:setGlobalLightDir(-0.2, 0.2, .7)
	game:setGlobalLightAmbient(.7, .7, .7, 1)
	game:setGlobalLightDiffuse(.7, .7, .7, 1)
	game:setGlobalLightSpecular(.5, .5, .5, 1)

	-- draw playing surface
	game:renderDynamicM(nil, 0, "models/box.obj", nil, "textures/box_surface.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1, 100.0,100.0,1.0, 0)
end


