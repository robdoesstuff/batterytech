-- Play module

PLAY_STATE_START = 0
PLAY_STATE_PLAY = 1
PLAY_STATE_FINISH = 2

PLAY_MOVE_SPEED = 15.0
PLAY_TURN_SPEED = 2.0

PLAY_WORLD_WIDTH = 50
PLAY_WORLD_LENGTH = 50
PLAY_BOX_SIZE = 2

PLAY_MINIMAP_SIZE = 200

TAU = 2 * math.pi
TO_DEGREES = 180 / math.pi
TO_RADS = math.pi / 180

local ENABLE_POINT_LIGHT = false
local ENABLE_PARTICLES = false

local FPS_CHECK_TIME = 3.0

local FWD_KEYS = {[38]=38, [0]=0, [118]=118}
local RIGHT_KEYS = {[39]=39, [3]=3, [121]=121}
local BACK_KEYS = {[40]=40, [1]=1, [119]=119}
local LEFT_KEYS = {[37]=37, [2]=2, [120]=120}
local JUMP_KEYS = {[32]=32, [13]=13, [96]=96, [99]=99}
local EXIT_KEYS = {[97]=97, [27]=27}


Box = {
	x = 0,
	y = 0
}

local CHAR_JUMP_HEIGHT = 4

Character = table.copy(GameObject)

Character.animations = {
	run = {startTime = 49/24, stopTime = 72/24, loop = true, rate = 1.6},
	turnR = {startTime = 94/24, stopTime = 104/24, loop = true, rate = 1.0},
	turnL = {startTime = 105/24, stopTime = 115/24, loop = true, rate = 1.0},
	idle1 = {startTime = 1/24, stopTime = 24/24, loop = true, rate = 1.0},
	idle2 = {startTime = 25/24, stopTime = 48/24, loop = true, rate = 1.0},
	jump = {startTime = 73/24, stopTime = 93/24, loop = false, rate = 0.7},
}

function Character.new()
    self = allocMeta(Character.createInstance(), Character)
    self:cInit()
    self:anim_allocAnimations(1)
    self:anim_initDynamic(0, "models/Bleep.bai", nil)
    self.animation = Character.animations.idle1
    self.animTime = self.animation.startTime
    self.animPlaybackRate = 1.0
    self.x = 0
    self.y = 0
    self.z = 0
    self.rot = 0
    return self
end

function Character:run()
	if self.animation ~= Character.animations.jump then
    	self.animation = Character.animations.run
    end
end

function Character:turn(dir)
	if self.animation ~= Character.animations.jump then
		if dir < 0 then
	    	self.animation = Character.animations.turnR
	    else
	    	self.animation = Character.animations.turnL
	    end
    end
end

function Character:idle()
	if self.animation ~= Character.animations.jump then
    	self.animation = Character.animations.idle1
    end
end

function Character:jump()
    self.animation = Character.animations.jump
end

function Character:update(tickDelta)
	-- now advance and interpolate the animation
	self.animTime = self.animTime + (tickDelta * self.animPlaybackRate * self.animation.rate)
	if self.animTime < self.animation.startTime then
		self.animTime = self.animation.startTime
		self.animationComplete = false
	end
	if self.animTime >= self.animation.stopTime then
		if self.animation.loop then
			self.animTime = self.animation.startTime
			self.animationComplete = false
		else
			self.animTime = self.animation.stopTime
			self.animationComplete = true
		end
	end
	if self.animation == Character.animations.jump then
		local jumpStart = self.animation.startTime + .2
		local jumpEnd = self.animation.stopTime - .2
		local t = (self.animTime - jumpStart) / (jumpEnd - jumpStart)
		if t > 0 and t < 1 then
			local z = CHAR_JUMP_HEIGHT - math.pow(t-.5, 2)*CHAR_JUMP_HEIGHT*4
			self.z = z
		else
			self.z = 0
		end
	end
	if self.animationComplete then
		self.animation = Character.animations.idle1
		self.animTime = self.animation.startTime
	end
    self:anim_interpolate(0, self.animTime)
end

function Character:render()
    local z = 1.68 + self.z
    local scale = 6
    local idx = game:renderAssimp(self, 0, "models/Bleep.bai", nil, nil, true, self.x,self.y,z, scale,scale,scale, 0,0,self.rot)
    -- It's so hard to get self-shadowing to look just right, we'll just disable it because it looks ok without
    game:setRenderItemParam(idx, "noshadowrecv", true)
    game:setRenderItemParam(idx, "lightperpixel", true)
end

--- Star Spinner
local STARSPINNER_TIME = 1.5
local STARSPINNER_TARGET_Z = 5
StarSpinner = {}
function StarSpinner.new(x,y,rot)
	local self = table.copy(StarSpinner)
	self.x = x
	self.y = y
	self.t = 0
	self.rot = rot
	self.finished = false
	return self
end

function StarSpinner:update(delta)
	self.t = self.t + delta
	self.rot = self.rot + delta * 540
	if self.t > STARSPINNER_TIME then
		self.finished = true
		self.t = STARSPINNER_TIME
	end
end

function StarSpinner:render()
	local zOffset = lerp1(STARSPINNER_TARGET_Z, 0, self.t / STARSPINNER_TIME)
	local alpha = (1 - lerp1(1, 0, self.t / STARSPINNER_TIME)) * 0.7
	local idx = game:renderAssimpM(nil, 0, "models/star.bai", nil, nil, true, 1,0,0,0,0,1,0,0,0,0,1,0,self.x,self.y,PLAY_BOX_SIZE/2 + zOffset,1, PLAY_BOX_SIZE,PLAY_BOX_SIZE,PLAY_BOX_SIZE, self.rot)
	game:setRenderItemParam(idx, "alpha", alpha)
	game:setRenderItemParam(idx, "nodirlight", true)
	game:setRenderItemParam(idx, "noshadowgen", true)
end

Play = {}

function Play.new()
	local vpWidth, vpHeight = getViewportSize()
	local self = table.copy(Play)
	self.localObjects = {}
	self.boxrot = 0
	self.buttons = {}
	-- quit button
	local button = makeButtonCentered(85, 50, 150, 100, "Quit")
	button.onClickUp = function()
		self:quit()
	end
	table.insert(self.buttons, button)
	-- arrow controls
	local arrowButtonW, arrowButtonH = scaleForUI(120,120)
	local leftPad = scaleX(5)
	local bottomPad = scaleY(5)
	-- up
	local sX = leftPad + arrowButtonW + arrowButtonW/2
	local sY = vpHeight - bottomPad - arrowButtonH*2 - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	self.upButton = button
	table.insert(self.buttons, button)
	-- right
	sX = leftPad + arrowButtonW*2 + arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	button.rotation = 90
	self.rightButton = button
	table.insert(self.buttons, button)
	-- top right
	sX = leftPad + arrowButtonW*2 + arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH*2 - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	self.topRightButton = button
	table.insert(self.buttons, button)
	-- down
	local sX = leftPad + arrowButtonW + arrowButtonW/2
	local sY = vpHeight - bottomPad - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	button.rotation = 180
	self.downButton = button
	table.insert(self.buttons, button)
	-- bottom right
	sX = leftPad + arrowButtonW*2 + arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	self.bottomRightButton = button
	table.insert(self.buttons, button)
	-- left
	sX = leftPad + arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	button.rotation = 270
	self.leftButton = button
	table.insert(self.buttons, button)
	-- bottom left
	sX = leftPad + arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	self.bottomLeftButton = button
	table.insert(self.buttons, button)
	-- top left
	sX = leftPad + arrowButtonW/2
	local sY = vpHeight - bottomPad - arrowButtonH*2 - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	self.topLeftButton = button
	table.insert(self.buttons, button)
	-- jump
	local jumpSize = scaleForUI(200, 0)
	local sX = vpWidth - jumpSize*1.2
	local sY = vpHeight - jumpSize*1.2
	button = TouchControl.new(sX, sY, jumpSize, jumpSize)
	button.normalAsset = "textures/jump.png"
	button.pressedAsset = "textures/jump_pressed.png"
	self.jumpButton = button
	table.insert(self.buttons, button)
	
	self.wasInput = false
	self.success = false
	self.level = 1
	self.nextRandomNoiseTime = math.random(1,3)
	self.fpsTestTimeLeft = FPS_CHECK_TIME
	self.fpsTotal = 0
	self.lowFPS = false
	return self
end

function Play:quit()
	self:cleanUp()
	game:setMode(MODE_MAINMENU)
end

function Play:cleanUp()
	game:clearLocalLights()
	game:clearParticleEmitters()
end

function Play:show()
	self.x = 0
	self.y = 0
	self.dir = math.pi
	self.rotAnim = 2.5
	self.state = PLAY_STATE_START
	self:setupLevel()
	self.controlFwd = 0
	self.controlTurn = 0
	self.controlJump = false
	self.panelAnim = 0
	playSound("sounds/whoosh.ogg")
	if ENABLE_POINT_LIGHT then
		local lightIdx = game:addLocalLight(0,0,5);
		game:setLocalLightParam(lightIdx, "ambient", 0,0,0,0);
		game:setLocalLightParam(lightIdx, "diffuse", 1,0,0,1);
		game:setLocalLightParam(lightIdx, "specular", 0,0,0,0);
		game:setLocalLightParam(lightIdx, "attenuation", 0.2, 0.05, 0.01);
	end
	if ENABLE_PARTICLES then
		local emitterId = game:addParticleEmitter()
		game:setParticleEmitterTextureAsset(emitterId,"textures/particle.png")
		game:setParticleEmitterPosition(emitterId, 0,0,1.0)
		game:setParticleEmitterDirection(emitterId, 0,0,1)
		game:setParticleEmissionRate(emitterId, 40)
		game:setParticleEmitterTimeRange(emitterId,1.5,3.0)
		game:setParticleEmitterConeRange(emitterId,0.1)
		game:setParticleInitialScale(emitterId,2.0)
		game:setParticleAlphaSpeedRange(emitterId, -0.5, -0.5)
		game:startParticleEmitter(emitterId)
	end
	game.fpsCounter:reset()
end

function Play:setupLevel()
	self.boxesTotal = 5 + self.level * 5
	self.timeLeft = 60 - self.level * 2
	if self.timeLeft < 30 then
		self.timeLeft = 30
	end
	self.boxesCollected = 0
	self.boxes = {}
	for i = 1, self.boxesTotal do
		local box = table.copy(Box)
		box.x = math.random() * PLAY_WORLD_WIDTH*2 - PLAY_WORLD_WIDTH
		box.y = math.random() * PLAY_WORLD_LENGTH*2 - PLAY_WORLD_LENGTH
		table.insert(self.boxes, box)
	end
	self.character = Character.new()
end

function Play:update(tickDelta)
	-- check for low FPS, shut off shadow if it doesn't come up
	if self.fpsTestTimeLeft > 0 then
		self.fpsTestTimeLeft = self.fpsTestTimeLeft - tickDelta
		self.fpsTotal = self.fpsTotal + game.fpsCounter.fps * tickDelta
		if self.fpsTestTimeLeft <= 0 then
			local avgFps = self.fpsTotal / FPS_CHECK_TIME
			logmsg("avgFps = " .. avgFps)
			if avgFps < 15 then
				self.lowFPS = true
				logmsg("Low FPS detected (average " .. avgFps .. ") - disabling shadows to improve")
			end
		end
	end
	for i,v in ipairs(self.buttons) do
		v:update(tickDelta)
	end
	for i = 1,#self.localObjects do
		if i > 0 then
			local obj = self.localObjects[i]
			if obj then
				obj:update(tickDelta)
				if obj.finished then
					table.remove(self.localObjects, i)
					i = i - 1
				end
			end
		end
	end
	self.boxrot = self.boxrot + tickDelta * 45
	if self.state == PLAY_STATE_PLAY then
		self:updatePlayState(tickDelta)
	elseif self.state == PLAY_STATE_START then
		if self.panelAnim < 1 then
			self.panelAnim = self.panelAnim + tickDelta * 3
			if self.panelAnim > 1 then self.panelAnim = 1 end
		end
		-- on key or mouse start
		if not self.wasInput and (getPointerState(0) or getKeyState(0)) then
			if self.panelAnim < 1 then
				self.panelAnim = 1
			else
				self:startGame()
			end
		end
	elseif self.state == PLAY_STATE_FINISH then
		if self.panelAnim < 1 then
			self.panelAnim = self.panelAnim + tickDelta * 3
			if self.panelAnim > 1 then self.panelAnim = 1 end
		end
		if not self.wasInput and (getPointerState(0) or getKeyState(0)) then
			if self.panelAnim < 1 then
				self.panelAnim = 1
			else
				if self.success then
					self.state = PLAY_STATE_START
					self.level = self.level + 1
					self:setupLevel()
				else
					self.state = PLAY_STATE_START
					self:setupLevel()
				end
			end
		end
	end
	self.wasInput = (getPointerState(0) or getKeyState(0))
end

function Play:updatePlayState(tickDelta)
	self.controlFwd = 0
	self.controlTurn = 0
	self.controlJump = false
	-- process key input
	for i = 0, 3 do
		local isDown, keyCode = getKeyState(i)
		if isDown then
			-- logmsg("keyCode " .. keyCode)
			if FWD_KEYS[keyCode] then
				--fwd
				self.controlFwd = 1
			elseif LEFT_KEYS[keyCode] then
				--left
				self.controlTurn = 1
			elseif RIGHT_KEYS[keyCode] then
				--right
				self.controlTurn = -1
			elseif BACK_KEYS[keyCode] then
				--back
				self.controlFwd = -1
			elseif JUMP_KEYS[keyCode] then
				--jump
				self.controlJump = true
			elseif EXIT_KEYS[keyCode] then
				--quit
				self:quit()
			end
		end
	end
	-- process touch input
	if self.topRightButton.isPressed then
		self.upButton.isPressed = true
		self.rightButton.isPressed = true
	end
	if self.bottomRightButton.isPressed then
		self.downButton.isPressed = true
		self.rightButton.isPressed = true
	end
	if self.bottomLeftButton.isPressed then
		self.downButton.isPressed = true
		self.leftButton.isPressed = true
	end
	if self.topLeftButton.isPressed then
		self.upButton.isPressed = true
		self.leftButton.isPressed = true
	end
	if self.upButton.isPressed then
		self.controlFwd = 1
	end
	if self.rightButton.isPressed then
		self.controlTurn = -1
	end
	if self.downButton.isPressed then
		self.controlFwd = -1
	end
	if self.leftButton.isPressed then
		self.controlTurn = 1
	end
	if self.jumpButton.isPressed then
		self.controlJump = true
	end
	self.dir = self.dir + tickDelta * self.controlTurn * PLAY_TURN_SPEED
	self.dir = self.dir % TAU
	local moveDist = tickDelta * self.controlFwd * PLAY_MOVE_SPEED
	if self.controlJump then
		self.character:jump()
	else
		if moveDist ~= 0 then
			self.character:run()
		elseif self.controlTurn ~= 0 then
			self.character:turn(self.controlTurn)
		else
			self.character:idle()
		end
	end
	self.y = self.y + math.cos(-self.dir) * moveDist
	self.x = self.x + math.sin(-self.dir) * moveDist
	-- keep the player in bounds
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
	-- collision test
	for i = 1, #self.boxes do
		local box = self.boxes[i]
		-- check to see if its null if we removed the box
		if not box then break end
		local boxLeft = box.x - PLAY_BOX_SIZE
		local boxRight = boxLeft + PLAY_BOX_SIZE*2
		local boxTop = box.y + PLAY_BOX_SIZE
		local boxBottom = boxTop - PLAY_BOX_SIZE*2
		if self.x < boxLeft or self.x > boxRight or self.y > boxTop or self.y < boxBottom then
			-- not intersecting
		else
			self.localObjects[#self.localObjects+1] = StarSpinner.new(box.x, box.y, self.boxrot)
			table.remove(self.boxes, i)
			self:collectBox()
		end
	end
	if #self.boxes == 0 then
		self:levelComplete()
	end
	self.timeLeft = self.timeLeft - tickDelta
	if self.timeLeft < 0 then
		self.timeLeft = 0
		if #self.boxes > 0 then
			self:levelFailed()
		end
	end
	self.nextRandomNoiseTime = self.nextRandomNoiseTime - tickDelta
	if self.nextRandomNoiseTime < 0 then
		self.nextRandomNoiseTime = math.random(12,18)
		playSound("sounds/space_noises.ogg", 0, 0.25, math.random(0.9,1.1))
	end
	if self.character then 
		self.character.x = self.x
		self.character.y = self.y
		self.character.rot = -math.deg(self.dir)+180
		self.character:update(tickDelta) 
	end
end

function Play:startGame()
	self.state = PLAY_STATE_PLAY
end

function Play:collectBox()
	self.boxesCollected = self.boxesCollected + 1
	playSound("sounds/laser_echo.ogg")
end

function Play:levelComplete()
	stopSound("sounds/space_noises.ogg")
	self.state = PLAY_STATE_FINISH
	self.success = true
	self.panelAnim = 0
	playSound("sounds/whoosh.ogg")
end

function Play:levelFailed()
	stopSound("sounds/space_noises.ogg")
	self.state = PLAY_STATE_FINISH
	self.success = false
	self.panelAnim = 0
	playSound("sounds/whoosh.ogg")
end

function Play:render()
	-- CAMERA PARAM
	
	local rotSin = math.sin(self.dir)
	local rotCos = math.cos(self.dir)
	local followX, followY = 0, -5
	local camOffsetX, camOffsetY = (followX*rotCos - followY*rotSin), (followX*rotSin + followY*rotCos)
	local camX, camY, camZ = self.x + camOffsetX, self.y + camOffsetY, 3
	if self.character then
		camZ = camZ + self.character.z
	end
	setCameraParams(camX, camY, camZ, 80, TO_DEGREES * self.dir)
	setCameraNearFarFOV(1, 500, 60)
	if self.character then
		self.rotAnim = self.rotAnim + 0.001
		self.rotAnim = self.rotAnim % TAU
		local ox,oy,oz = math.sin(self.rotAnim), math.cos(self.rotAnim),1
		ox,oy,oz = vec3_normalize(ox,oy,oz)
		game:setGlobalLightDir(ox,oy,oz)
		local dist = 5
		game:setShadowLightOrigin(self.character.x + ox*dist, self.character.y + oy*dist, 2.5 + oz*dist + self.character.z)
	end
	game:setShadowLightFrustumNearFar(2, 20)
 	game:setShadowColorAndEpsilon(0.6, 0.6, 0.6, 0.000001)
 	-- cull back face, not self-shadowing so lines up better with ground
 	game:setShadowCullMode(2)
	game:setGlobalLightAmbient(.3, .3, .3, 1)
	game:setGlobalLightDiffuse(1.0, 1.0, 1.0, 1)
	game:setGlobalLightSpecular(2.0, 2.0, 2.0, 1)
 	local orthoAmt = 1.5
    game:setShadowOrtho(orthoAmt, -orthoAmt, -orthoAmt, orthoAmt)
    -- game:setShadowPerspective(45)
   	local vpWidth, vpHeight = getViewportSize()
   	-- custom shadowmap size, 1/3 of viewport height
   	if self.lowFPS then
   		game:setShadowType(0)
   	else
		game:setShadowType(3, vpHeight/3, vpHeight/3)
   	end
	game:setGlobalLightEnabled(true)
    game:setFogEnabled(false)
    game:setFogParams(1, 45, .2, .2, .2, 1.0)
    -- draw BG
    -- local idx = game:render2D("shadowmap",vpWidth - vpWidth/4,vpHeight - vpHeight/4,vpWidth/2,vpHeight/2,0)
    -- game:setRenderItemParam(idx, "uvs", 1, 1, 0, 0)
    --local idx = game:render2DBG("textures/space.jpg", vpWidth/2, vpHeight/2, vpWidth, vpHeight, 0)
    local scale = .2
    local idx = game:renderAssimp(nil, 0, "models/skybox.obj", nil, nil, true, camX, camY, camZ, scale,scale,scale, 0,0, TO_DEGREES * self.rotAnim - 105)
	game:setRenderItemParam(idx, "nodirlight", true)
    game:setRenderItemParam(idx, "noshadowrecv", true)
   	game:setRenderItemParam(idx, "noshadowgen", true)
   	game:setRenderItemParam(idx, "nofog", true)
	-- draw stars
	for i = 1, #self.boxes do
		local box = self.boxes[i]
		local idx = game:renderAssimpM(nil, 0, "models/star.bai", nil, nil, true, 1,0,0,0,0,1,0,0,0,0,1,0,box.x,box.y,PLAY_BOX_SIZE/2,1, PLAY_BOX_SIZE,PLAY_BOX_SIZE,PLAY_BOX_SIZE, self.boxrot)
		game:setRenderItemParam(idx, "alpha", 0.7)
		game:setRenderItemParam(idx, "nodirlight", true)
   		game:setRenderItemParam(idx, "noshadowgen", true)
   		local scale = 5
    	local idx = game:renderAssimp(nil, 0, "models/quad.obj", nil, "textures/particle.png", true, box.x,box.y, 1.2, scale,scale,scale, 0,0, self.boxrot)
    	game:setRenderItemParam(idx, "colorFilter", 0, 1, 1, 0.7)
		game:setRenderItemParam(idx, "nodirlight", true)
		game:setRenderItemParam(idx, "noshadowrecv", true)
		game:setRenderItemParam(idx, "noshadowgen", true)
	end
	-- draw local objects
	for i = 1,#self.localObjects do
		local obj = self.localObjects[i]
		obj:render()
	end
			
	if self.character then self.character:render() end
	local scale = 7.0
	local idx = game:renderAssimp(nil, 0, "models/crater.obj", nil, nil, true, 0,0, 1.0, scale,scale,0.4, 0,0, 0)
    game:setRenderItemParam(idx, "noshadowgen", true)
	game:setRenderItemParam(idx, "nodirlight", true)
	local timeString = string.format("Time: %2d", self.timeLeft)
	local renderIdx = game:renderText2D("ui", timeString, scaleX(640), scaleY(50))
	game:setRenderItemParam(renderIdx, "align", "center")
	renderIdx = game:renderText2D("ui", self.boxesCollected .. "/" .. self.boxesTotal, scaleX(1250), scaleY(50))
	game:setRenderItemParam(renderIdx, "align", "right")
	if self.state == PLAY_STATE_PLAY then
		-- minimap
		local mmx,mmy = scaleXY(1280-105, 200)
		local w,h = scaleXY(PLAY_MINIMAP_SIZE, PLAY_MINIMAP_SIZE)
		renderIdx = game:render2D("textures/minimap_atlas.png", mmx,mmy, w,h)
		game:setRenderItemParam(renderIdx, "uvs", 0.0, 0.5, 0.5, 1.0)
		
		w,h = scaleXY(25, 25)
		local worldToMinimapWidthScale = PLAY_MINIMAP_SIZE / (PLAY_WORLD_WIDTH*2)
		local worldToMinimapHeightScale = PLAY_MINIMAP_SIZE / (PLAY_WORLD_LENGTH*2)
		for i = 1, #self.boxes do
			local box = self.boxes[i]
			local x,y = scaleXY(-(box.x * worldToMinimapWidthScale), box.y * worldToMinimapHeightScale)
			renderIdx = game:render2D("textures/minimap_atlas.png", mmx + x, mmy + y, w,h)
			game:setRenderItemParam(renderIdx, "uvs", 0.0, 0.0, 0.5, 0.5)
		end
		local x,y = scaleXY(-(self.x * worldToMinimapWidthScale), self.y * worldToMinimapHeightScale)
		renderIdx = game:render2D("textures/minimap_atlas.png", mmx + x, mmy + y, w,h, -self.dir-math.pi)
		game:setRenderItemParam(renderIdx, "uvs", 0.5, 0.0, 1.0, 0.5)
	elseif self.state == PLAY_STATE_START then
		-- 5 px offset on bg to make up for shadow
		local x,y = scaleXY(640, 400)
		local w,h = scaleForUI(self.panelAnim * 500, self.panelAnim * 550)
		game:render2D("textures/play_panel_bg.png", x,y,w,h, 0)
		if self.panelAnim == 1 then
			x,y = scaleXY(640, 240)
			local renderIdx = game:renderText2D("ui", "Level " .. self.level, x,y)
			game:setRenderItemParam(renderIdx, "align", "center")
			x,y = scaleXY(640, 330)
			local renderIdx = game:renderText2D("ui", "Boxes: " .. self.boxesTotal, x,y)
			game:setRenderItemParam(renderIdx, "align", "center")
			x,y = scaleXY(640, 420)
			local renderIdx = game:renderText2D("ui", "Time: " .. self.timeLeft, x,y)
			game:setRenderItemParam(renderIdx, "align", "center")
			x,y = scaleXY(640, 580)
			local renderIdx = game:renderText2D("ui", "Press To Start!", x,y)
			game:setRenderItemParam(renderIdx, "align", "center")
		end
	elseif self.state == PLAY_STATE_FINISH then
		-- 5 px offset on bg to make up for shadow
		local x,y = scaleXY(650, 400)
		local w,h = scaleForUI(self.panelAnim * 500, self.panelAnim * 500)
		game:render2D("textures/play_panel_bg.png", x,y,w,h, 0)
		if self.panelAnim == 1 then
			x,y = scaleXY(640, 220)
			if self.success then
				local renderIdx = game:renderText2D("ui", "Level Complete!", x,y)
				game:setRenderItemParam(renderIdx, "align", "center")
			else
				local renderIdx = game:renderText2D("ui", "Level Failed", x,y)
				game:setRenderItemParam(renderIdx, "align", "center")
			end
			x,y = scaleXY(640, 580)
			if self.success then
				local renderIdx = game:renderText2D("ui", "Press To Continue", x,y)
				game:setRenderItemParam(renderIdx, "align", "center")
			else
				local renderIdx = game:renderText2D("ui", "Press To Retry", x,y)
				game:setRenderItemParam(renderIdx, "align", "center")
			end
		end
	end
	
	-- this will debug the shadowmap
	-- local vpWidth, vpHeight = getViewportSize()
 	-- game:render2D("shadowmap",vpWidth - vpWidth/4,vpHeight - vpHeight/4,vpWidth/2,vpHeight/2,0)
	
	-- buttons
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	
end

