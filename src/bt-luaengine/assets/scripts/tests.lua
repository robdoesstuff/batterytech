-- Tests module

ENABLE_POINT_LIGHT = false
ENABLE_PARTICLES = true

PLAY_MOVE_SPEED = 15.0
PLAY_TURN_SPEED = 2.0

PLAY_WORLD_WIDTH = 50
PLAY_WORLD_LENGTH = 50
PLAY_BOX_SIZE = 2

AnimatedModel = table.copy(GameObject)
function AnimatedModel.new()
    self = allocMeta(AnimatedModel.createInstance(), AnimatedModel)
    self:cInit()
    self:anim_allocAnimations(1)
    self:anim_initDynamic(0, "models/Seymour.dae", nil)
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
    local z = 1.0
    local scale = 0.75
    local idx = game:renderAssimpM(self, 0, "models/Seymour.dae", nil, nil, true, 1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1, scale,scale,scale, 180)
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
	local vpWidth, vpHeight = getViewportSize()
	local self = table.copy(Tests)
    self.globalLightEnabled = true
    self.pointLightCount = 0
    self.pointLights = {}
    self.shadowType = 0
    self.fogEnabled = false
	self.boxrot = 0
	self.buttons = {}
	-- quit button
	local button = makeButtonCentered(85, 50, 150, 100, "Quit")
	button.onClickUp = function()
		self:cleanUp()
		game:setMode(MODE_MAINMENU)
	end
	table.insert(self.buttons, button)
	-- global light button
	local button = makeButtonCentered(150, 200, 300, 100, "GLight: on")
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
	local button = makeButtonCentered(150, 300, 300, 100, "PLights: 0")
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
	local button = makeButtonCentered(150, 400, 300, 100, "Shadow: Off")
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
	-- fog button
	local button = makeButtonCentered(150, 500, 300, 100, "Fog: Off")
	button.onClickUp = function()
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
	self.wasInput = false
	return self
end

function Tests:cleanUp()
	game:clearLocalLights()
	game:clearParticleEmitters()
    self.pointLights = {}
    self.pointLightCount = 0
end

function Tests:show()
	self.x = 0
	self.y = 0
	self.dir = math.pi
	self.state = PLAY_STATE_START
	self:setupLevel()
	self.controlFwd = 0
	self.controlTurn = 0
	self.panelAnim = 0
	playSound("sounds/whoosh.ogg")
	if ENABLE_PARTICLES then
		local emitterId = game:addParticleEmitter()
		game:setParticleEmitterTextureAsset(emitterId,"textures/particle.png")
		game:setParticleEmitterPosition(emitterId, 10,-20,1.0)
		game:setParticleEmitterDirection(emitterId, 0,0,1)
		game:setParticleEmissionRate(emitterId, 40)
		game:setParticleEmitterTimeRange(emitterId,1.5,3.0)
		game:setParticleEmitterConeRange(emitterId,0.1)
		game:setParticleInitialScale(emitterId,2.0)
		game:setParticleAlphaSpeedRange(emitterId, -0.5, -0.5)
		game:startParticleEmitter(emitterId)
	end
    addTestResources()
    game:loadNewResources()
end

function Tests:setupLevel()
	self.boxes = {}
    self:makeBox(11,-25, {1.0, 0, 1.0, 1.0})
    self:makeBox(5,-10, {1.0, 1.0, 1.0, 0.2}, true)
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
    if not self.seymour then
        self.seymour = AnimatedModel.new()
    else
        self.seymour:update(tickDelta)
    end
    self:updatePointLights(tickDelta)
    for i,v in ipairs(self.buttons) do
        v:update(tickDelta)
    end
    self.boxrot = self.boxrot + tickDelta * 45
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
            game:setLocalLightParam(lightIdx, "specular", 0,0,0,0);
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
	-- CAMERA PARAM
	-- setCameraParams(0,0,100, 0,0)
	setCameraParams(self.x, self.y, 5, 90, TO_DEGREES * self.dir)
	setCameraNearFarFOV(1, 100, 60)
	game:setShadowLightOrigin(-14, -14, 30)
	game:setShadowColorAndEpsilon(0.5, 0.5, 0.5, 0.002)
	game:setShadowLightFrustumNearFar(10, 50)
	game:setShadowType(self.shadowType)
	game:setGlobalLightDir(-0.2, 0.2, .7)
	game:setGlobalLightAmbient(.7, .7, .7, 1)
	game:setGlobalLightDiffuse(.7, .7, .7, 1)
	game:setGlobalLightSpecular(.5, .5, .5, 1)
	game:setGlobalLightEnabled(self.globalLightEnabled)
	-- draw playing surface - preserve order to optimize
	local idx = game:renderAssimpM(nil, 0, "models/box.obj", nil, "textures/box_star.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1, 50.0,50.0,1.0, 0)
    game:setRenderItemParam(idx, "drawfirst", "true")
    game:setRenderItemParam(idx, "maxPointLights", self.pointLightCount)
    game:setRenderItemParam(idx, "noshadowgen", "true")
	-- draw boxes
	for i = 1, #self.boxes do
		local box = self.boxes[i]
		local idx = game:renderAssimpM(nil, 0, "models/box.obj", nil, "textures/box_star.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,box.x,box.y,PLAY_BOX_SIZE/2,1, PLAY_BOX_SIZE,PLAY_BOX_SIZE,PLAY_BOX_SIZE, self.boxrot)
        game:setRenderItemParam(idx, "colorFilter", box.color[1], box.color[2], box.color[3], box.color[4])
        if box.twosided then
            game:setRenderItemParam(idx, "twosided", "true")
        end
		game:setRenderItemParam(idx, "maxPointLights", self.pointLightCount)
	end
    if self.seymour then
        self.seymour:render(self.pointLightCount)
    end
    -- draw point lights
    for i = 1, #self.pointLights do
        local pl = self.pointLights[i]
        pl:render()
    end
	-- this will debug the shadowmap
    -- TODO - add debug shadow button
    if self.shadowType ~= 0 then
        local vpWidth, vpHeight = getViewportSize()
        -- game:render2D("shadowmap",vpWidth - vpWidth/4,vpHeight - vpHeight/4,vpWidth/2,vpHeight/2,0)
	end
	-- buttons
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	
end
