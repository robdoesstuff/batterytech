-- Physics Tests module

PHYS_WORLD_WIDTH = 160
PHYS_WORLD_HEIGHT = 100

local DRAW_MODE = false

local CAT_DEFAULT = 1
local CAT_GROUND = 2

local TYPE_STATIC = 0
local TYPE_KINEMATIC = 1
local TYPE_DYNAMIC = 2

local CALLBACK_DETAIL_NONE = 0
local CALLBACK_DETAIL_BROAD = 1
local CALLBACK_DETAIL_NARROW = 2

-- run physics at fixed timestep 120fps
local PHYSICS_UPDATE_TIME = 1.0/120.0

local EXIT_KEYS = {[97]=97, [27]=27}

function unprojectScreenToWorld(x,y)
	vpw, vph = getViewportSize()
	return x * (PHYS_WORLD_WIDTH / vpw), y * (PHYS_WORLD_HEIGHT / vph)
end

------------------- Physics Objects -----------------

Circle = table.copy(GameObject)

function Circle.new(x, y, radius)
	local self = allocMeta(Circle.createInstance(), Circle)
	self:cInit()
	self.objType = "Circle"
	self.radius = radius
	self.bodyId = self:physics_createBody(TYPE_DYNAMIC, x, y)
	local fixtureId = self:physics_createCircleFixture(self.bodyId, radius);
	return self
end

function Circle:update(delta)
    -- local c = self:countPhysicsContacts()
	-- logmsg("(Circle) " .. c .. " contacts")
	-- for i = 0, c-1 do
		-- local other, pointCount, x1,y1, x2,y2, isTouching, isActive, fixtureId = self:getPhysicsContact(i)
		-- logmsg("(Circle) contact is " .. other.objType .. " " .. pointCount .. " " .. x1 .. "," .. y1)
	-- end
end

function Circle:render()
	local x,y,angle = self:physics_getBodyTransform(0)
	-- logmsg("Circle render: " .. x .. " " .. y)
	game:render2D("textures/circle.png", x,y, self.radius * 2, self.radius * 2, angle)
end

Box = table.copy(GameObject)

-- angle is in degrees
function Box.new(x, y, width, height, angle, isStatic)
	local self = allocMeta(Box.createInstance(), Box)
	self:cInit()
	self.objType = "Box"
	self.width = width
	self.height = height
	self.x = x
	self.y = y
	self.type = TYPE_STATIC
    if not isStatic then
        self.type = TYPE_DYNAMIC
    end
	local bodyId = self:physics_createBody(self.type, x, y)
	local fixtureId = self:physics_createPolygonFixture(bodyId, -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, height/2)
	if not isStatic then
		self:physics_setFixtureDensity(fixtureId, 0.1)
	end
    self:physics_setBodyTransform(bodyId, x, y, math.rad(angle))
	return self
end

function Box:render()
	local x,y,angle = self:physics_getBodyTransform(0)
	-- logmsg("Circle render: " .. x .. " " .. y)
	if self.type == TYPE_STATIC then
		game:render2D("textures/rock.png", x,y, self.width, self.height, angle)
	else
		game:render2D("textures/crate.jpg", x,y, self.width, self.height, angle)
	end
end

Pinwheel = table.copy(GameObject)

-- angle is in degrees
function Pinwheel.new(x, y, size)
	local self = allocMeta(Pinwheel.createInstance(), Pinwheel)
	self:cInit()
	self.objType = "Pinwheel"
	self.size = size
	self.nSize = size / 8
	-- two bodies - one is static circle in center, other is compound fixture dynamic attached to static via revolute joint
	self.staticBody = self:physics_createBody(TYPE_STATIC, x, y)
	local circleFixture = self:physics_createCircleFixture(staticBody, 1);
	self:physics_setFixtureDensity(circleFixture, 0)
	self.dynBody = self:physics_createBody(TYPE_DYNAMIC, x, y)
	local nSize = self.nSize
	local fixtureId = self:physics_createPolygonFixture(self.dynBody, -size/2, -nSize/2, size/2, -nSize/2, size/2, nSize/2, -size/2, nSize/2)
	local fixtureId = self:physics_createPolygonFixture(self.dynBody, -nSize/2, -size/2, nSize/2, -size/2, nSize/2, size/2, -nSize/2, size/2)
	local revJoint = game:physics_addRevoluteJoint(self, self.staticBody, self, self.dynBody, x, y)
	game:physics_setRevoluteJointMotor(revJoint, true, -1.0, 10000.0)
	return self
end

function Pinwheel:render()
	local x,y,angle = self:physics_getBodyTransform(self.dynBody)
	-- logmsg("Circle render: " .. x .. " " .. y)
	game:render2D("textures/pinwheel.png", x,y, self.size, self.nSize, angle)
	game:render2D("textures/pinwheel.png", x,y, self.size, self.nSize, angle+math.rad(90))
end

StaticChain = table.copy(GameObject)

function StaticChain.new(verts)
	local self = allocMeta(StaticChain.createInstance(), StaticChain)
	self:cInit()
	self.objType = "Static"
	local bodyId = self:physics_createBody(TYPE_STATIC, 0, 0)
	local fixtureId = self:physics_createChainFixture(bodyId, true, unpack(verts))
	self:physics_setFixtureFilter(fixtureId, CAT_GROUND, 0xffff, 0)
	return self
end

SensorBox = table.copy(GameObject)

function SensorBox.new(x, y, width, height, angle)
	local self = allocMeta(SensorBox.createInstance(), SensorBox)
	self:cInit()
	self.objType = "SensorBox"
	self.width = width
	self.height = height
	self.bodyId = self:physics_createBody(TYPE_STATIC, x, y)
	local fixtureId = self:physics_createPolygonFixture(bodyId, -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, height/2)
	self:physics_setFixtureIsSensor(fixtureId, true)
    self:physics_setBodyTransform(bodyId, x, y, math.rad(angle))
	return self
end

function SensorBox:update(delta)
	local num = self:queryPhysicsContacts(self.bodyId, "queryCallback")
	if num > 0 then
		-- logmsg("sb " .. num)
	end
end

function SensorBox:queryCallback(otherObj, otherFixtureId, pointCount, x1,y1, x2,y2)
	if otherObj.objType == "Circle" then
		self:startPusher()
	end
end

Pusher = table.copy(GameObject)

local PUSHER_STATE_WAIT = 0
local PUSHER_STATE_PUSH = 1
local PUSHER_STATE_RETRACT = 2
local PUSHER_TIME = 1

-- angle is in degrees
function Pusher.new(x, y, width, height, angle)
	local self = allocMeta(Pusher.createInstance(), Pusher)
	self:cInit()
	self.objType = "Pusher"
	self.initX = x
	self.initY = y
	self.width = width
	self.height = height
	self.angle = angle
	self.state = PUSHER_STATE_WAIT
	self.pushT = 0
	-- two bodies - one is static circle in center, other is box attached by prismatic joint
	self.staticBody = self:physics_createBody(TYPE_STATIC, x, y)
	local circleFixture = self:physics_createCircleFixture(self.staticBody, 1);
	self:physics_setFixtureDensity(circleFixture, 0)
	self.dynBody = self:physics_createBody(TYPE_DYNAMIC, x, y)
	local fixtureId = self:physics_createPolygonFixture(self.dynBody, -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, height/2)
	-- don't collide with 0x0002 (ground)
	self:physics_setFixtureFilter(fixtureId, CAT_DEFAULT, 0xfffd, 0)
	self.pJoint = game:physics_addPrismaticJoint(self, self.staticBody, self, self.dynBody, x,y, 1,0, false)
    self:physics_setBodyTransform(bodyId, x, y, math.rad(self.angle))
    game:physics_setPrismaticJointLimits(self.pJoint, true, 0, 40)
	return self
end

function Pusher:update(delta)
	if self.state == PUSHER_STATE_PUSH then
		local translation, speed, force = game:physics_getPrismaticJointValues(self.pJoint)
		if translation >= 40 then
			self.state = PUSHER_STATE_RETRACT
			game:physics_setPrismaticJointMotor(self.pJoint, true, -200, 1000)
		end
	elseif self.state == PUSHER_STATE_RETRACT then
		local translation, speed, force = game:physics_getPrismaticJointValues(self.pJoint)
		if translation <= 0 then
			self.state = PUSHER_STATE_WAIT
			game:physics_setPrismaticJointMotor(self.pJoint, false, 0, 0)
		end
	end
end

function Pusher:render()
	local x,y,angle = self:physics_getBodyTransform(self.dynBody)
	game:render2D("textures/pusher.png", x,y, self.width, self.height, angle)
end

function Pusher:push()
	if self.state ~= PUSHER_STATE_PUSH then
		self.state = PUSHER_STATE_PUSH
    	game:physics_setPrismaticJointMotor(self.pJoint, true, 300, 2000)
	end
end

------------------------------- Other Objects -------------------------------

SmokePuff = {}

function SmokePuff.new(x,y)
	local self = table.copy(SmokePuff)
	self.x = x
	self.y = y
	self.scale = 1
	self.spin = math.random()
	self.alpha = 1
	self.isDone = false
	return self
end

function SmokePuff:update(delta)
	self.scale = self.scale + delta
	self.spin = self.spin + delta
	self.alpha = self.alpha - delta
	if self.alpha <= 0 then
		self.isDone = true
	end
end

function SmokePuff:render()
	local idx = game:render2D("textures/smokepuff.png", self.x, self.y, 5*self.scale, 5*self.scale, self.spin)
	game:setRenderItemParam(idx, "alpha", self.alpha)
end

ActionTimer = {}

function ActionTimer.new(time, action)
	local self = table.copy(ActionTimer)
	self.time = time
	self.action = action
	return self
end

function ActionTimer:update(delta)
	self.time = self.time - delta
	if self.time <= 0 then
		self.action()
		self.isDone = true
	end
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
	self.localObjects = {}
	self.mouseJointId = nil
	self.physicsUpdateTimeRemaining = 0
	return self
end

function PhysicsTests:quit()
	self:cleanUp()
	game:setMode(MODE_MAINMENU)
end

function PhysicsTests:cleanUp()
	-- Deactivate/deallocate objects
	for i = 1,#self.objects do
		self.objects[i]:cDeactivate()
	end
    self.objects = {}
    self.localObjects = {}
    self.mouseObj = nil
    self.ball = nil
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
		self:quit()
	end
	table.insert(self.buttons, button)
    local settingButtonWidth = 350
	-- global light button
	local button = makeButtonCentered(250 + settingButtonWidth/2, 50, settingButtonWidth, 100, "Debug: off")
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
	local button = makeButtonCentered(650 + settingButtonWidth/2, 50, settingButtonWidth, 100, "Render: on")
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
	-- create static world components
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
	table.insert(self.objects, Box.new(30, 25, 20, 5, 45, true))
	table.insert(self.objects, Box.new(50, 37, 20, 5, -45, true))
	table.insert(self.objects, Box.new(30, 49, 20, 5, 45, true))
	table.insert(self.objects, Box.new(50, 61, 20, 5, -45, true))
	
	table.insert(self.objects, Pinwheel.new(30, 80, 20))
	local sensorBox = SensorBox.new(45, 95, 30, 5, 0)
	table.insert(self.objects, sensorBox)
	self.pusher = Pusher.new(20, 95, 10, 5, 0)
	table.insert(self.objects, self.pusher)
	sensorBox.startPusher = function(sb)
		self.pusher:push()
	end
	self:resetDynamicObjects()
    game:setPhysicsGravity(0, 40)
    self.countingDown = false
end

function PhysicsTests:resetDynamicObjects()
	-- ball
	if self.ball then
		-- mark previous for removal
		self.ball.remove = true
		-- remove body ahead of time to not interfere with anything else
		self.ball:physics_removeBody(self.ball.bodyId)
	end
	self.ball = Circle.new(35, 10, 2)
	table.insert(self.objects, self.ball)
	-- dynamic boxes
	local bottomY = 92.7
	local x = 100
	self:addBreakableBox(x, bottomY)
	self:addBreakableBox(x, bottomY-5)
	self:addBreakableBox(x, bottomY-10)
	self:addBreakableBox(x, bottomY-15)
	self:addBreakableBox(x, bottomY-20)
	self:addBreakableBox(x, bottomY-25)
	self:addBreakableBox(x, bottomY-30)
	local x = 105
	local bottomY = 93
	self:addBreakableBox(x, bottomY)
	self:addBreakableBox(x, bottomY-5)
	self:addBreakableBox(x, bottomY-10)
	self:addBreakableBox(x, bottomY-15)
	self:addBreakableBox(x, bottomY-20)
	self:addBreakableBox(x, bottomY-25)
	self:addBreakableBox(x, bottomY-30)
	self:addBreakableBox(x, bottomY-35)
	local x = 111
	local bottomY = 93.5
	self:addBreakableBox(x, bottomY)
	self:addBreakableBox(x, bottomY-5)
	self:addBreakableBox(x, bottomY-10)
	self:addBreakableBox(x, bottomY-15)
	self:addBreakableBox(x, bottomY-20)
	self:addBreakableBox(x, bottomY-25)
	self:addBreakableBox(x, bottomY-30)
end

-------------- Main Update Function -----------------

function PhysicsTests:update(tickDelta)
	self.physicsUpdateTimeRemaining = self.physicsUpdateTimeRemaining + tickDelta
	while self.physicsUpdateTimeRemaining > PHYSICS_UPDATE_TIME do
    	game:updatePhysics(PHYSICS_UPDATE_TIME, 5, 2)
    	self.physicsUpdateTimeRemaining = self.physicsUpdateTimeRemaining - PHYSICS_UPDATE_TIME
	end
    game:clearPhysicsForces()
    for i,v in ipairs(self.buttons) do
        v:update(tickDelta)
    end
    for i = 1,#self.localObjects do
		self.localObjects[i]:update(tickDelta)
	end
	for i = 1,#self.objects do
		local obj = self.objects[i]
    	if obj and obj.remove then
    		if self.mouseObj == obj then
    			self.mouseObj = nil
    		end
     		obj:cDeactivate()
    		table.remove(self.objects, i)
    		i = i - 1
    		if i == 0 then
    			break
    		end
    	end
	end
    for i = 1,#self.localObjects do
    	local obj = self.localObjects[i]
    	if obj and obj.isDone then
    		table.remove(self.localObjects, i)
    		i = i - 1
    		if i == 0 then
    			break
    		end
    	end
	end
    self:updateInput(tickDelta)
end

----------------- Main Render Function -------------------

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
		for i = 1,#self.objects do
			if self.objects[i].render then
				self.objects[i]:render()
			end
		end
		for i = 1,#self.localObjects do
			if self.localObjects[i].render then
				self.localObjects[i]:render()
			end
		end
    end
	game:end2DProjection()
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

function PhysicsTests:updateInput(tickDelta)
    self.wasInput = (getPointerState(0) or getKeyState(0))
    local isDown, keyCode = getKeyState(0)
	if isDown then
		-- logmsg("keyCode " .. keyCode)
		if EXIT_KEYS[keyCode] then
			--quit
			self:quit()
		end
	end
    
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
	                    self.mouseObj = obj1
	                end
		    	end
		    end
	    end
	    if self.mouseJointId then
	    	if not self.mouseObj then
	    		-- the object we were controlling was removed
	    		self.mouseJointId = nil
	    	else
	   			game:physics_setMouseJointPosition(self.mouseJointId, wx,wy)
	    	end
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

function PhysicsTests:addSmokePuff(x,y)
	self.localObjects[#self.localObjects+1] = SmokePuff.new(x,y)
end

function PhysicsTests:addActionTimer(time,action)
	self.localObjects[#self.localObjects+1] = ActionTimer.new(time,action)
end

function PhysicsTests:addBreakableBox(x,y)
	local box = Box.new(x, y, 5, 5, 0, false)
	box.onCollisionStarted = function(box, other, force, velocity)
		if force and force > 15 then
			local curX,curY = box:physics_getBodyTransform(0)
			self:addSmokePuff(curX,curY)
			-- mark for removal
			box.remove = true
			if not self.countingDown then
				self.countingDown = true
				self:addActionTimer(5, function()
					self:resetDynamicObjects()
					self.countingDown = false
				end
				)
			end
		end
		if force then
	    	-- logmsg("(Box) onCollisionStarted - " .. other.objType .. " " .. force .. " " .. velocity)
	    else
	    	-- logmsg("(Box) onCollisionStarted - " .. other.objType)
	    end
	end
	box:setPhysicsCallbackDetail(CALLBACK_DETAIL_BROAD)
	self.objects[#self.objects+1] = box
end