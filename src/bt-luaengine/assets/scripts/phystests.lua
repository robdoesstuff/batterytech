-- Physics Tests module

PHYS_WORLD_WIDTH = 160
PHYS_WORLD_HEIGHT = 100

function unprojectScreenToWorld(x,y)
	vpw, vph = getViewportSize()
	return x * (PHYS_WORLD_WIDTH / vpw), y * (PHYS_WORLD_HEIGHT / vph)
end

Circle = table.copy(GameObject)

function Circle.new(x, y, radius)
	local self = allocMeta(Circle.createInstance(), Circle)
	self:cInit()
	self.objType = "Circle"
	self.radius = radius
	local bodyId = self:physics_createBody(type, x, y)
	self:physics_setBodyType(bodyId, 2)
	local fixtureId = self:physics_createCircleFixture(bodyId, radius);
	self.fixtureId2 = self:physics_createCircleFixture(bodyId, radius/2, 5, 5);
	
--    self:setPhysicsCallbackDetail(2)	
	return self
end

function Circle:update(delta)
    local c = self:countPhysicsContacts()
	-- logmsg("(Circle) " .. c .. " contacts")
	for i = 0, c-1 do
		-- local other, pointCount, x1,y1, x2,y2, isTouching, isActive, fixtureId = self:getPhysicsContact(i)
		-- logmsg("(Circle) contact is " .. other.objType .. " " .. pointCount .. " " .. x1 .. "," .. y1)
	end
end

function Circle:render()
	local x,y,angle = self:physics_getBodyTransform(0)
	-- logmsg("Circle render: " .. x .. " " .. y)
	game:render2D("textures/circle.png", x,y, self.radius * 2, self.radius * 2, angle)
	local x,y = self:physics_getBodyWorldPoints(0, 1, 5,5)
	-- logmsg("Circle render: " .. x .. " " .. y)
	game:render2D("textures/circle.png", x,y, self.radius, self.radius, angle)
end

function Circle:onCollisionStarted(other, force, velocity)
	if force then
    	logmsg("(Circle) onCollisionStarted - " .. other.objType .. " " .. force .. " " .. velocity)
    else
    	logmsg("(Circle) onCollisionStarted - " .. other.objType)
    end
end

function Circle:onCollisionUpdated(other, force, velocity)
	if force then
    	-- logmsg("(Circle) onCollisionUpdated - " .. other.objType .. " " .. force .. " " .. velocity)
    else
    	-- logmsg("(Circle) onCollisionUpdated - " .. other.objType)
    end
end

function Circle:onCollisionEnded(other)
    logmsg("(Circle) onCollisionEnded - " .. other.objType)
end

Box = table.copy(GameObject)

function Box.new(x, y, width, height, isStatic)
	local self = allocMeta(Box.createInstance(), Box)
	self:cInit()
	self.objType = "Box"
	self.width = width
	self.height = height
	local bodyId = self:physics_createBody(type, x, y)
	local fixtureId = self:physics_createPolygonFixture(bodyId, -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, height/2)
    if not isStatic then
        self:physics_setBodyType(0, 2)
    else
        self:physics_setBodyType(0, 0)
    end
	return self
end

function Box:render()
	local x,y,angle = self:physics_getBodyTransform(0)
	-- logmsg("Circle render: " .. x .. " " .. y)
	game:render2D("textures/rectangle.png", x,y, self.width, self.height, angle)
end

StaticChain = table.copy(GameObject)

function StaticChain.new()
	local self = allocMeta(StaticChain.createInstance(), StaticChain)
	self:cInit()
	self.objType = "Static"
	self.width = width
	self.height = height
	local y = 19
	local verts = {0,y, 30,y, 43,y}
	local bodyId = self:physics_createBody(type, x, y)
	local fixtureId = self:physics_createChainFixture(bodyId, false, unpack(verts))
    self:physics_setBodyType(0, 0)
	return self
end

function StaticChain:render()
end

------------------------------- Module Main Def -----------------------------

PhysicsTests = {}

function PhysicsTests.new()
	local self = table.copy(PhysicsTests)
	self.wasInput = false
	self.shouldRender = true
	self.objects = {}
	self.mouseJointId = nil
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
			game:setPhysicsDrawDebug(true, 0, PHYS_WORLD_WIDTH, PHYS_WORLD_HEIGHT, 0, -1, 1)
        else
            button.label = "Debug: off"
			game:setPhysicsDrawDebug(false)
        end
	end
	table.insert(self.buttons, button)
	local button = makeButtonCentered(settingButtonWidth/2, 300, settingButtonWidth, 100, "Render: on")
	button.onClickUp = function(button)
        self.shouldRender = not self.shouldRender
        if self.shouldRender then
            button.label = "Render: on"
        else
            button.label = "Render: off"
        end
	end
	table.insert(self.buttons, button)
end

function PhysicsTests:setupScene()
	game:createPhysicsWorld()
	table.insert(self.objects, StaticChain.new())
	table.insert(self.objects, Circle.new(30, 80, 5))
	local c = Circle.new(35, 30, 5)
    local c2 = Circle.new(40, 80, 5)
	c:setPhysicsCallbackDetail(2)
	table.insert(self.objects, c)
	table.insert(self.objects, c2)
	table.insert(self.objects, Box.new(20, 70, 5, 5))
	table.insert(self.objects, Box.new(30, 70, 5, 5))
	table.insert(self.objects, Box.new(40, 70, 5, 5))
	table.insert(self.objects, Box.new(50, 70, 5, 5))
	table.insert(self.objects, Box.new(60, 70, 5, 5))
    self.groundbox = Box.new(PHYS_WORLD_WIDTH/2, PHYS_WORLD_HEIGHT-2, PHYS_WORLD_WIDTH, 2, true)
    self.groundbox.objType = "Ground Box"
	table.insert(self.objects, self.groundbox)
    game:setPhysicsGravity(0, 10)
    self.jointId = game:physics_addDistanceJoint(c, 0, c2, 0, 35,30, 40, 80)
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
    -- Query for anything under cursor
    local isDown, x,y = getPointerState(0)
    if isDown then
	    local wx, wy = unprojectScreenToWorld(x,y)
	    local obj1, fixtureId1, obj2, fixtureId2 = game:queryPhysicsAABB(wx-1,wy-1, wx+1,wy+1)
	    if obj1 then
	    	-- logmsg("Clicked on " .. obj1.objType)
	    	local objX, objY = obj1:physics_getBodyTransform(0)
	    	-- obj1:physics_applyForce(0, 500,0, objX + 500, objY)
	    	if not self.mouseJointId and obj1 ~= self.groundbox then
                local bodyId = obj1:physics_getFixtureBodyId(fixtureId1)
                if bodyId then
                    self.mouseJointId = game:physics_addMouseJoint(self.groundbox, 0, obj1, bodyId, wx,wy, 5000)
                end
	    	end
	    end
	    if self.mouseJointId then
            -- logmsg("Moving mouse joint to " .. wx .. " " .. wy)
	   		game:physics_setMouseJointPosition(self.mouseJointId, wx,wy)
	    end
    else
    	if self.mouseJointId then
    		game:physics_removeJoint(self.mouseJointId)
    		self.mouseJointId = nil
    	end
    end
end

function PhysicsTests:updateTestsState(tickDelta)
end

function PhysicsTests:render()
    local vpWidth, vpHeight = getViewportSize()
	-- buttons
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	game:start2DProjection(0, PHYS_WORLD_WIDTH, PHYS_WORLD_HEIGHT, 0, -1, 1)
	if self.shouldRender then
		for i,v in ipairs(self.objects) do
			v:render()
		end
	    local x1,y1, x2,y2 = game:physics_getJointAnchorPoints(self.jointId)
	    self:renderLineObject("textures/rectangle.png", 1, x1,y1, x2,y2)
    end
	game:end2DProjection()
	-- local idx = game:renderText2D("test", "Test... gpBMFont-Render!^", 400, 400)
	-- local idx = game:renderText2D("test2", "Test... gpBMFont-Render!^", 400, 400)
 	-- local idx = game:renderText2D("ui", "Test... gpBMFont-Render!^", 400, 400)
    -- local idx = game:renderText2D("test2", "This Is\nMultiline\nText", 400, 400)
    -- game:setRenderItemParam(idx, "multiline", true)
end

function PhysicsTests:renderLineObject(texture, width, x1,y1, x2,y2)
    local xd = x2-x1
    local yd = y2-y1
    local angle = math.atan(yd/xd)
    local length = math.sqrt(xd*xd+yd*yd)
    local centerX = x1+(xd)/2
    local centerY = y1+(yd)/2
    game:render2D(texture, centerX, centerY, length, width, angle)
end
