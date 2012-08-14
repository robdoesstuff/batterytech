-- Physics Tests module

Circle = table.copy(GameObject)

function Circle.new(subclass)
	local self = allocMeta(Circle.createInstance(), subclass or Circle)
	self.objType = "Circle"
	self:physics_allocConfigs(1)
	self:physics_createCircleShape(0, 50.0)
	self:cInit()
	return o
end

function Circle:render()
end

function Circle:onCollision(other)
end

PhysicsTests = {}

function PhysicsTests.new()
	local self = table.copy(PhysicsTests)
	self.wasInput = false
	self.objects = {}
	return self
end

function PhysicsTests:cleanUp()
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
	table.insert(self.objects, Circle.new())
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
    for i,v in ipairs(self.buttons) do
        v:update(tickDelta)
    end
    self:updateTestsState(tickDelta)
    self.wasInput = (getPointerState(0) or getKeyState(0))
    game:updatePhysics(tickDelta, 5, 5)
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
