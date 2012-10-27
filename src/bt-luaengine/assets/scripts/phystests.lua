-- Physics Tests module

PHYS_WORLD_WIDTH = 160
PHYS_WORLD_HEIGHT = 100

local DRAW_MODE = false

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

function StaticChain.new(verts)
	local self = allocMeta(StaticChain.createInstance(), StaticChain)
	self:cInit()
	self.objType = "Static"
	local bodyId = self:physics_createBody(type, 0, 0)
	local fixtureId = self:physics_createChainFixture(bodyId, true, unpack(verts))
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
	self.wasDown = false
	self.chainPoints = {}
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
	local verts = {76.875,86.125, 80,89.375, 83.875,92.25, 88.25,93.75, 92.75,94.875, 97.25,95.25, 101.875,95.25, 106.5,95.625,
	111,96, 115.5,96.125, 119.875,95, 124.25,93.625, 128.5,91.75, 132.625,89.625, 136.5,87.375, 139.625,84, 142.25,80.25, 
	143.375,75.875, 143.375,71.375, 143.375,66.75, 143,62.25,143.5, 57.75,143.625, 53.25,143.75, 48.75,144.375, 44.125,144.375,
	39.625,144, 35.125,143.75, 30.5,143,25.875, 141.875,21.5, 140.625,17.125, 139.875,12.5, 138,8.25,134.5, 5,130.75,2.375, 126.375,0.875,
	121.625,0.75, 117.125,1.375, 112.625,1, 108,0.875, 103.375,1.125, 98.625,1.125, 94.125,1.5, 89.625,1.375, 85.125,0.875, 80.375,0.75,
	75.625,0.625, 71.125,1.125, 66.625,0.125, 47.875,0, 43.375,0.375, 38.75,0.75, 34.125,1.625, 29.625,2.75, 25.25,3.75, 21.625,6.5, 18.875,10.25,
	17.25,14.5, 16.25,18.875, 15.25,23.25, 14.625,28.125, 14.875,32.75, 14.375,37.25, 14.25,41.75, 15,46.25, 14.5,50.875, 14.5,55.5, 14.375,60,
	14.375,64.5, 14.375,69, 14.875,73.5, 15.875,77.875, 17,82.25, 20.25,85.5, 22.5,89.375, 26.25,92.125, 30,94.625, 34.375,96.25, 39,96.875,
	43.5,97.25,48, 97.5,54.75, 97.75,61.25, 97.75,63.75, 96.5,66,95, 70.125,92.75, 73.5,89.625, 76.625,86.25}
	self.groundbox = StaticChain.new(verts)
	table.insert(self.objects, self.groundbox)
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
	    if DRAW_MODE then
		    if not self.wasDown then
		    	-- start drawing
		    	self.chainPoints[#self.chainPoints+1] = wx
		    	self.chainPoints[#self.chainPoints+1] = wy
		    	self.wasDown = true
		    else
		    	lastX = self.chainPoints[#self.chainPoints-1] 
		    	lastY = self.chainPoints[#self.chainPoints] 
		    	if (lastX-wx)*(lastX-wx)+(lastY-wy)*(lastY-wy) > 20 then
			    	self.chainPoints[#self.chainPoints+1] = wx
			    	self.chainPoints[#self.chainPoints+1] = wy
			    	logmsg("New Point " .. wx .. " " .. wy)
		    	end
		    end
	    else
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
    	if self.wasDown then
    		-- make chain, output array for use
    		if #self.chainPoints > 2 then
    			table.insert(self.objects, StaticChain.new(self.chainPoints))
    			logtable(self.chainPoints)
    		end
    		self.chainPoints = {}
    	end
    	self.wasDown = false
    end
end

function PhysicsTests:updateTestsState(tickDelta)
end

function PhysicsTests:render()
    local vpWidth, vpHeight = getViewportSize()
    if self.shouldRender then
    	game:render2D("textures/physics_bg.jpg", vpWidth/2, vpHeight/2, vpWidth, vpHeight)
    end
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
