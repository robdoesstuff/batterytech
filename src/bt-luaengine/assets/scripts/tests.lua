-- Tests module

ENABLE_POINT_LIGHT = false
ENABLE_PARTICLES = true

PLAY_MOVE_SPEED = 15.0
PLAY_TURN_SPEED = 2.0

PLAY_WORLD_WIDTH = 50
PLAY_WORLD_LENGTH = 50
PLAY_BOX_SIZE = 2

local FWD_KEYS = {[38]=38, [0]=0, [118]=118}
local RIGHT_KEYS = {[39]=39, [3]=3, [121]=121}
local BACK_KEYS = {[40]=40, [1]=1, [119]=119}
local LEFT_KEYS = {[37]=37, [2]=2, [120]=120}
local EXIT_KEYS = {[97]=97, [27]=27}

AnimatedModel = table.copy(GameObject)
function AnimatedModel.new()
    self = allocMeta(AnimatedModel.createInstance(), AnimatedModel)
    self:cInit()
    self:anim_allocAnimations(1)
    self:anim_initDynamic(0, "models/Seymour.bai", nil)
    self.animPos = 0
    return self
end

function AnimatedModel:update(tickDelta)
    self.animPos = self.animPos + tickDelta
    self:anim_interpolate(0, self.animPos)
end

function AnimatedModel:render(pointLightCount)
    local x = 0
    local y = -20
    local z = 1.05
    local scale = 0.75
    local idx = game:renderAssimpM(self, 0, "models/Seymour.bai", nil, nil, true, 1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1, scale,scale,scale, 180)
    game:setRenderItemParam(idx, "maxPointLights", pointLightCount)
end

PointLight = {}
function PointLight.new(idx,x,y,z)
    local self = table.copy(PointLight)
    self.idx = idx
    self.centerX = x
    self.centerY = y
    self.x = x
    self.y = y
    self.z = z
    self.rot = 0
    self.r, self.g, self.b = 0,0,0
    self.flicker = 1
    return self
end

function PointLight:update(tickDelta)
    self.rot = self.rot + tickDelta
    self.rot = self.rot % TAU
    self.x = self.centerX + math.sin(self.rot) * 5
    self.y = self.centerY + math.cos(self.rot) * 5
    -- self.flicker = math.random() * 0.3 + 0.7
    -- logmsg("plx = " .. self.x .. " ply = " .. self.y)
    game:setLocalLightParam(self.idx, "position", self.x, self.y, self.z);
    game:setLocalLightParam(self.idx, "diffuse", self.r*self.flicker,self.g*self.flicker,self.b*self.flicker,1);
end

function PointLight:render()
    local idx = game:renderBB("textures/particle.png", self.x,self.y,self.z, 3,3, math.random()*TAU)
    game:setRenderItemParam(idx, "colorFilter", self.r*self.flicker, self.g*self.flicker, self.b*self.flicker, 0.5)
end

Tests = {}

function Tests.new()
	local self = table.copy(Tests)
	self.wasInput = false
	return self
end

function Tests:quit()
	self:cleanUp()
	game:setMode(MODE_MAINMENU)
end

function Tests:cleanUp()
	game:clearLocalLights()
	game:clearParticleEmitters()
    self.pointLights = {}
    self.pointLightCount = 0
    if self.seymour then
    	self.seymour:cDeactivate()
    	self.seymour = nil
    end
end

function Tests:show()
    self.globalLightEnabled = true
    self.pointLightCount = 0
    self.pointLights = {}
    self.shadowType = 0
    self.fogEnabled = false
    self.shadowMapDebug = false
    self.bgEnabled = true
    self.animModelEnabled = true
	self.boxrot = 0
    self.bgPos = .5
    self.bgDir = 1
    self:setupUI()
	self.x = 0
	self.y = 0
	self.dir = math.pi
	self.state = PLAY_STATE_START
	self:setupLevel()
	self.controlFwd = 0
	self.controlTurn = 0
	self.panelAnim = 0
	-- playSound("sounds/whoosh.ogg")
	if ENABLE_PARTICLES then
		local emitterId = game:addParticleEmitter()
		game:setParticleEmitterTextureAsset(emitterId,"textures/particle.png")
		game:setParticleEmitterPosition(emitterId, 7,-20,1.0)
		game:setParticleEmitterDirection(emitterId, 0,0,1)
		game:setParticleEmissionRate(emitterId, 40)
		game:setParticleEmitterTimeRange(emitterId,1.5,3.0)
		game:setParticleEmitterConeRange(emitterId,0.1)
		game:setParticleInitialScale(emitterId,2.0)
		game:setParticleAlphaSpeedRange(emitterId, -0.5, -0.5)
		game:startParticleEmitter(emitterId)
		test2DParticles = false
		if test2DParticles then
			local emitterId = game:add2DParticleEmitter()
			game:setParticleEmitterTextureAsset(emitterId,"textures/particle.png")
			game:setParticleEmitterPosition(emitterId, 640, 400)
			game:setParticleEmitterPositionRange(emitterId, 50, 30)
			game:setParticleEmitterDirection(emitterId, 0, 1)
			game:setParticleEmissionRate(emitterId, 40)
			game:setParticleEmitterTimeRange(emitterId,1.5,3.0)
			game:setParticleEmitterConeRange(emitterId,0.5)
			game:setParticleInitialScale(emitterId,25.0)
			game:setParticleAlphaSpeedRange(emitterId, -0.5, -0.5)
			game:setParticleMaxSpeedRange(emitterId, 50, 50)
			game:setParticleGravity(emitterId, 0, 10)
			game:setParticleAutoStopMax(emitterId, 50)
			game:startParticleEmitter(emitterId)
		end
	end
    if not self.newResourcesAdded then
        addTestResources()
        game:loadNewResources()
        self.newResourcesAdded = true
    end
end

function Tests:setupUI()
	local vpWidth, vpHeight = getViewportSize()
	self.buttons = {}
	-- quit button
	local button = makeButtonCentered(85, 50, 150, 100, "Quit")
	button.onClickUp = function()
		self:quit()
	end
	table.insert(self.buttons, button)
    local settingButtonWidth = 350
	-- global light button
	local button = makeButtonCentered(settingButtonWidth/2, 200, settingButtonWidth, 100, "GLight: on")
	button.onClickUp = function(button)
        self.globalLightEnabled = not self.globalLightEnabled
        if self.globalLightEnabled then
            button.label = "GLight: on"
        else
            button.label = "GLight: off"
        end
	end
	table.insert(self.buttons, button)
	-- point lights button
	local button = makeButtonCentered(settingButtonWidth/2, 300, settingButtonWidth, 100, "PLights: 0")
	button.onClickUp = function(button)
        if self.pointLightCount == 3 then
            self.pointLightCount = 0
            game:clearLocalLights()
            self.pointLights = {}
        else
            self.pointLightCount = self.pointLightCount + 1
        end
        button.label = "PLights: " .. self.pointLightCount
	end
	table.insert(self.buttons, button)
	-- shadow button
	local button = makeButtonCentered(settingButtonWidth/2, 400, settingButtonWidth, 100, "Shadow: Off")
	button.onClickUp = function()
        self.shadowType = self.shadowType + 1
        self.shadowType = self.shadowType % 3
        game:setShadowType(self.shadowType)
        if self.shadowType == 0 then
            button.label = "Shadow: Off"
        elseif self.shadowType == 1 then
            button.label = "Shadow: Low"
        else
            button.label = "Shadow: High"
        end
	end
	table.insert(self.buttons, button)
	-- shadowmap debug button
	local button = makeButtonCentered(settingButtonWidth/2, 500, settingButtonWidth, 100, "ShDbg: Off")
	button.onClickUp = function()
        self.shadowMapDebug = not self.shadowMapDebug
        if self.shadowMapDebug then
            button.label = "ShMapDbg: on"
        else
            button.label = "ShMapDbg: off"
        end
	end
	table.insert(self.buttons, button)
	-- fog button
	local button = makeButtonCentered(settingButtonWidth/2, 600, settingButtonWidth, 100, "Fog: Off")
	button.onClickUp = function(button)
        self.fogEnabled = not self.fogEnabled
        if self.fogEnabled then
            button.label = "Fog: on"
        else
            button.label = "Fog: off"
        end
	end
	table.insert(self.buttons, button)
	-- anim model button
	local button = makeButtonCentered(settingButtonWidth/2, 700, settingButtonWidth, 100, "AModel: On")
	button.onClickUp = function(button)
        self.animModelEnabled = not self.animModelEnabled
        if self.animModelEnabled then
            button.label = "AModel: on"
        else
            button.label = "AModel: off"
        end
	end
	table.insert(self.buttons, button)
	-- arrow controls
	local arrowButtonW, arrowButtonH = scaleForUI(100,100)
	local rightPad = scaleX(5)
	local bottomPad = scaleY(5)
	-- up
	local sX = vpWidth - rightPad - arrowButtonW - arrowButtonW/2
	local sY = vpHeight - bottomPad - arrowButtonH*2 - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	self.upButton = button
	table.insert(self.buttons, button)
	-- right
	sX = vpWidth - rightPad - arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	button.rotation = 90
	self.rightButton = button
	table.insert(self.buttons, button)
	-- down
	local sX = vpWidth - rightPad - arrowButtonW - arrowButtonW/2
	local sY = vpHeight - bottomPad - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	button.rotation = 180
	self.downButton = button
	table.insert(self.buttons, button)
	-- left
	sX = vpWidth - rightPad - arrowButtonW*2 - arrowButtonW/2
	sY = vpHeight - bottomPad - arrowButtonH - arrowButtonH/2
	button = TouchControl.new(sX-arrowButtonW/2, sY-arrowButtonH/2, arrowButtonW, arrowButtonH)
	button.normalAsset = "textures/arrow.png"
	button.pressedAsset = "textures/arrow_pressed.png"
	button.rotation = 270
	self.leftButton = button
	table.insert(self.buttons, button)
end

function Tests:setupLevel()
	self.boxes = {}
    self:makeBox(9,-25, {1.0, 0, 1.0, 1.0})
    self:makeBox(3,-10, {1.0, 1.0, 1.0, 0.2}, true)
    self:makeBox(-7,-25, {0, 0, 1.0, 1.0})
    self:makeBox(-5,-15, {1.0, 1.0, 1.0, 0.2})
    self:makeBox(-7.5,-15, {1.0, 0, 0, 1.0})
    self:makeBox(-10,-15, {0, 1.0, 0, 1.0})
end

function Tests:makeBox(x,y,color,twosided)
    local box = table.copy(Box)
    box.x = x
    box.y = y
    box.color = color
    if twosided then
        box.twosided = true
    else
        box.twosided = false
    end
    table.insert(self.boxes, box)
end

function Tests:update(tickDelta)
    if self.animModelEnabled then
        if not self.seymour then
            self.seymour = AnimatedModel.new()
        else
            self.seymour:update(tickDelta)
        end
    end
    self:updatePointLights(tickDelta)
    for i,v in ipairs(self.buttons) do
        v:update(tickDelta)
    end
    self.boxrot = self.boxrot + tickDelta * 45
    if self.bgDir == 1 then
        self.bgPos = self.bgPos + tickDelta * .1
        if self.bgPos >= 1 then
            self.bgPos = 1
            self.bgDir = -1
        end
    else
        self.bgPos = self.bgPos - tickDelta * .1
        if self.bgPos <= 0 then
            self.bgPos = 0
            self.bgDir = 1
        end
    end
    self:updateTestsState(tickDelta)
    self.wasInput = (getPointerState(0) or getKeyState(0))
end

function Tests:updatePointLights(tickDelta)
    if self.pointLightCount > #self.pointLights then
        while self.pointLightCount > #self.pointLights do
            local lightIdx = game:addLocalLight(0,0,5);
            local r,g,b = 0,0,0
            if #self.pointLights == 0 then
                r = 1
            elseif #self.pointLights == 1 then
                g = 1
            elseif #self.pointLights == 2 then
                b = 1
            end
            game:setLocalLightParam(lightIdx, "ambient", 0,0,0,0);
            game:setLocalLightParam(lightIdx, "diffuse", r,g,b,1);
            game:setLocalLightParam(lightIdx, "specular", r,g,b,1);
            game:setLocalLightParam(lightIdx, "attenuation", 0.2, 0.05, 0.01);
            local pl = PointLight.new(lightIdx, 0,-20,5)
            pl.r = r
            pl.g = g
            pl.b = b
            table.insert(self.pointLights, pl)
        end
    end
    for i = 1,self.pointLightCount do
        local pl = self.pointLights[i]
        pl:update(tickDelta)
    end
end

function Tests:updateTestsState(tickDelta)
	self.controlFwd = 0
	self.controlTurn = 0
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
			elseif EXIT_KEYS[keyCode] then
				--quit
				self:quit()
			end
		end
	end
	-- process touch input
	if self.upButton.isPressed then
		self.controlFwd = 1
	end
	if self.downButton.isPressed then
		self.controlFwd = -1
	end
	if self.rightButton.isPressed then
		self.controlTurn = -1
	end
	if self.leftButton.isPressed then
		self.controlTurn = 1
	end
	self.dir = self.dir + tickDelta * self.controlTurn * PLAY_TURN_SPEED
	self.dir = self.dir % TAU
	local moveDist = tickDelta * self.controlFwd * PLAY_MOVE_SPEED
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
end

function Tests:render()
    local vpWidth, vpHeight = getViewportSize()
	-- CAMERA PARAM
	-- setCameraParams(0,0,100, 0,0)
	setCameraParams(self.x, self.y, 5, 90, TO_DEGREES * self.dir)
	setCameraNearFarFOV(1, 100, 60)
	game:setShadowLightOrigin(-14, -14, 30)
	game:setShadowColorAndEpsilon(0.8, 0.8, 0.8, 0.01)
	game:setShadowLightFrustumNearFar(10, 50)
    game:setShadowOrtho(15, -15, 15, -15)
	game:setShadowType(self.shadowType)
	game:setGlobalLightDir(-0.2, 0.2, .7)
	game:setGlobalLightAmbient(.8, .8, .8, 1)
	game:setGlobalLightDiffuse(.8, .8, .8, 1)
	game:setGlobalLightSpecular(.9, .9, .9, 1)
	game:setGlobalLightEnabled(self.globalLightEnabled)
    game:setFogEnabled(self.fogEnabled)
    if self.fogEnabled then
        game:setFogParams(1, 25, .3,.3,.3,1)
    end
    if self.bgEnabled then
        local idx = game:render2DBG("textures/box_surface.jpg", vpWidth/2, vpHeight/2, vpWidth, vpHeight, 0)
        game:setRenderItemParam(idx, "colorFilter", 0.5, 0.5, 0.5, 1.0)
        local uvOffset = (sigmoid(self.bgPos) - 0.5) * .2
        game:setRenderItemParam(idx, "uvs", 0.2+uvOffset, 0.2, 0.8+uvOffset, 0.8)
    end
	local idx = game:renderAssimpM(nil, 0, "models/box.bai", nil, "textures/ground.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,0,-20,0,1, 50.0,50.0,1.0, 0)
    game:setRenderItemParam(idx, "maxPointLights", self.pointLightCount)
    game:setRenderItemParam(idx, "noshadowgen", "true")
	-- draw boxes
	for i = 1, #self.boxes do
		local box = self.boxes[i]
		local idx = game:renderAssimpM(nil, 0, "models/box.bai", nil, "textures/box_star.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,box.x,box.y,PLAY_BOX_SIZE/2,1, PLAY_BOX_SIZE,PLAY_BOX_SIZE,PLAY_BOX_SIZE, self.boxrot)
        game:setRenderItemParam(idx, "colorFilter", box.color[1], box.color[2], box.color[3], box.color[4])
        if box.twosided then
            game:setRenderItemParam(idx, "twosided", "true")
        end
        if box.color[4] < 1.0 then
            game:setRenderItemParam(idx, "noshadowgen", "true")
            game:setRenderItemParam(idx, "noshadowrecv", "true")
            game:setRenderItemParam(idx, "nofog", "true")
        end
		game:setRenderItemParam(idx, "maxPointLights", self.pointLightCount)
	end
    if self.seymour and self.animModelEnabled then
        self.seymour:render(self.pointLightCount)
    end
    -- draw point lights
    for i = 1, #self.pointLights do
        local pl = self.pointLights[i]
        pl:render()
    end
	-- this will debug the shadowmap
    if self.shadowType ~= 0 and self.shadowMapDebug then
        game:render2D("shadowmap",vpWidth - vpWidth/4,vpHeight - vpHeight/4,vpWidth/2,vpHeight/2,0)
	end
	-- buttons
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	-- local idx = game:renderText2D("test", "Test... gpBMFont-Render!^", 400, 400)
	-- local idx = game:renderText2D("test2", "Test... gpBMFont-Render!^", 400, 400)
 	-- local idx = game:renderText2D("ui", "Test... gpBMFont-Render!^", 400, 400)
    -- local idx = game:renderText2D("test2", "This Is\nMultiline\nText", 400, 400)
    -- game:setRenderItemParam(idx, "multiline", true)
end
