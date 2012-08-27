-- Physics Tests module

Circle = table.copy(GameObject)

function Circle.new(x, y, radius)
	local self = allocMeta(Circle.createInstance(), Circle)
	self.objType = "Circle"
	self:physics_allocConfigs(1)
	self:physics_createCircleShape(0, 50.0)
    self:physics_setBodyTransform(0, x, y, 0)
    if not isStatic then
        self:physics_setBodyType(0, 2)
    else
        self:physics_setBodyType(0, 0)
    end
	self:cInit()
	return o
end

function Circle:render()
end

function Circle:onCollision(other)
end

Box = table.copy(GameObject)

function Box.new(x, y, width, height, isStatic)
	local self = allocMeta(Box.createInstance(), Box)
	self.objType = "Box"
	self:physics_allocConfigs(1)
	self:physics_createPolygonShape(0, -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, height/2)
    self:physics_setBodyTransform(0, x, y, 0)
    if not isStatic then
        self:physics_setBodyType(0, 2)
    else
        self:physics_setBodyType(0, 0)
    end
	self:cInit()
	return o
end

function Box:render()
end

-- TODO - need collision points
function Box:onCollision(other)
end

PhysicsTests = {}

function PhysicsTests.new()
	local self = table.copy(PhysicsTests)
	self.wasInput = false
	self.objects = {}
	return self
end

function PhysicsTests:cleanUp()
    self.objects = {}
    game:destroyPhysicsWorld()
end

function PhysicsTests:show()
    self:setupUI()
    self:setupScene()
end

function PhysicsTests:setupUI()
	local vpWidth, vpHeight = getViewportSize()
	self.buttons = {}
	-- quit button
	local button = makeButtonCentered(85, 50, 150, 100, "Quit")
	button.onClickUp = function()
		self:cleanUp()
		game:setMode(MODE_MAINMENU)
	end
	table.insert(self.buttons, button)
    local settingButtonWidth = 350
	-- global light button
	local button = makeButtonCentered(settingButtonWidth/2, 200, settingButtonWidth, 100, "Debug: off")
	button.onClickUp = function(button)
        self.debugPhysics = not self.debugPhysics
        if self.debugPhysics then
            button.label = "Debug: on"
        else
            button.label = "Debug: off"
        end
	end
	table.insert(self.buttons, button)
end

function PhysicsTests:setupScene()
	game:createPhysicsWorld()
	game:setPhysicsDrawDebug(true)
	table.insert(self.objects, Circle.new(300, 300, 50))
	table.insert(self.objects, Circle.new(350, 200, 50))
	table.insert(self.objects, Circle.new(400, 300, 50))
	table.insert(self.objects, Box.new(200, 200, 50, 50))
	table.insert(self.objects, Box.new(300, 200, 50, 50))
	table.insert(self.objects, Box.new(400, 200, 50, 50))
	table.insert(self.objects, Box.new(500, 200, 50, 50))
	table.insert(self.objects, Box.new(600, 200, 50, 50))
	table.insert(self.objects, Box.new(640, 750, 1280, 100, true))
    game:setPhysicsGravity(0, 100)   
end

function PhysicsTests:makeBox(x,y,color,twosided)
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

function PhysicsTests:update(tickDelta)
    game:updatePhysics(tickDelta, 10, 20)
    for i,v in ipairs(self.buttons) do
        v:update(tickDelta)
    end
    self:updateTestsState(tickDelta)
    self.wasInput = (getPointerState(0) or getKeyState(0))
end

function PhysicsTests:updateTestsState(tickDelta)
end

function PhysicsTests:render()
    local vpWidth, vpHeight = getViewportSize()
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
