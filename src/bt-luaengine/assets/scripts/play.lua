-- Play module

PLAY_STATE_START = 0
PLAY_STATE_PLAY = 1
PLAY_STATE_FINISH = 2

PLAY_MOVE_SPEED = 15
PLAY_TURN_SPEED = math.pi

PLAY_WORLD_WIDTH = 50
PLAY_WORLD_LENGTH = 50
PLAY_BOX_SIZE = 2

TAU = 2 * math.pi
TO_DEGREES = 180 / math.pi
TO_RADS = math.pi / 180

Box = {
	x = 0,
	y = 0
}

Play = {}

function Play.new()
	local self = table.copy(Play)
	self.boxrot = 0
	self.buttons = {}
	local button = makeButtonCentered(85, 50, 150, 100, "Quit")
	button.onClickUp = function()
		game:setMode(MODE_MAINMENU)
	end
	table.insert(self.buttons, button)
	return self
end

function Play:show()
	self.x = 0
	self.y = 0
	self.dir = 0
	self.state = PLAY_STATE_PLAY
	self.controlFwd = 0
	self.controlTurn = 0
	self.timeLeft = 40
	self.boxesTotal = 10
	self.boxesCollected = 0
	self.boxes = {}
	for i = 1, self.boxesTotal do
		local box = table.copy(Box)
		box.x = math.random() * PLAY_WORLD_WIDTH*2 - PLAY_WORLD_WIDTH
		box.y = math.random() * PLAY_WORLD_LENGTH*2 - PLAY_WORLD_LENGTH
		table.insert(self.boxes, box)
	end
end

function Play:update(tickDelta)
	for i,v in ipairs(self.buttons) do
		v:update(tickDelta)
	end
	self.boxrot = self.boxrot + tickDelta * 45
	if self.state == PLAY_STATE_PLAY then
		self.controlFwd = 0
		self.controlTurn = 0
		-- process key input
		for i = 0, 3 do
			local isDown, keyCode = getKeyState(i)
			if isDown then
				-- logmsg("keyCode " .. keyCode)
				if keyCode == 38 then
					--fwd
					self.controlFwd = 1
				elseif keyCode == 37 then
					--left
					self.controlTurn = 1
				elseif keyCode == 39 then
					--right
					self.controlTurn = -1
				elseif keyCode == 40 then
					--back
					self.controlFwd = -1
				end
			end
		end
		self.dir = self.dir + tickDelta * self.controlTurn * PLAY_TURN_SPEED
		self.dir = self.dir % TAU
		local moveDist = tickDelta * self.controlFwd * PLAY_MOVE_SPEED
		self.y = self.y + math.cos(-self.dir) * moveDist
		self.x = self.x + math.sin(-self.dir) * moveDist
		if self.x > PLAY_WORLD_WIDTH then
			self.x = PLAY_WORLD_WIDTH
		end
		if self.x < -PLAY_WORLD_WIDTH then
			self.x = -PLAY_WORLD_WIDTH
		end
		if self.y > PLAY_WORLD_LENGTH then
			self.y = PLAY_WORLD_LENGTH
		end
		if self.y < -PLAY_WORLD_LENGTH then
			self.y = -PLAY_WORLD_LENGTH
		end
		self.timeLeft = self.timeLeft - tickDelta
	end
end

function Play:render()
	-- CAMERA PARAM
	-- setCameraParams(0,0,100, 0,0)
	setCameraParams(self.x, self.y, 2, 90, TO_DEGREES * self.dir)
	setCameraNearFarFOV(1, 500, 60)
	game:setShadowLightOrigin(0, 0, 100)
	game:setShadowColorAndEpsilon(0.5, 0.5, 0.5, 0.021)
	game:setShadowLightFrustumNearFar(80, 110)
	game:setGlobalLightDir(-0.2, 0.2, .7)
	game:setGlobalLightAmbient(.7, .7, .7, 1)
	game:setGlobalLightDiffuse(.7, .7, .7, 1)
	game:setGlobalLightSpecular(.5, .5, .5, 1)

	-- draw boxes
	for i = 1, #self.boxes do
		local box = self.boxes[i]
		game:renderDynamicM(nil, 0, "models/box.obj", nil, "textures/box_star.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,box.x,box.y,PLAY_BOX_SIZE/2,1, PLAY_BOX_SIZE,PLAY_BOX_SIZE,PLAY_BOX_SIZE, self.boxrot)
	end
	-- draw playing surface
	game:renderDynamicM(nil, 0, "models/box.obj", nil, "textures/box_surface.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1, 100.0,100.0,1.0, 0)
	local timeString = string.format("Time: %2d", self.timeLeft)
	local renderIdx = game:renderText2D("ui", timeString, scaleX(640), scaleY(50))
	game:setRenderItemParam(renderIdx, "align", "center")
	renderIdx = game:renderText2D("ui", self.boxesCollected .. "/" .. self.boxesTotal, scaleX(1250), scaleY(50))
	game:setRenderItemParam(renderIdx, "align", "right")
	for i,v in ipairs(self.buttons) do
		v:render()
	end
end


