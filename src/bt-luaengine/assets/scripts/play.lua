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

Box = {
	x = 0,
	y = 0
}

Play = {}

function Play.new()
	local vpWidth, vpHeight = getViewportSize()
	local self = table.copy(Play)
	self.boxrot = 0
	self.buttons = {}
	-- quit button
	local button = makeButtonCentered(85, 50, 150, 100, "Quit")
	button.onClickUp = function()
		game:setMode(MODE_MAINMENU)
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
	self.success = false
	self.level = 1
	return self
end

function Play:show()
	self.x = 0
	self.y = 0
	self.dir = math.pi
	self.state = PLAY_STATE_START
	self:setupLevel()
	self.controlFwd = 0
	self.controlTurn = 0
	self.panelAnim = 0
end

function Play:setupLevel()
	self.boxesTotal = 5 + self.level * 5
	self.timeLeft = 60 - self.level * 2
	if self.timeLeft < 30 then
		self.timeLeft = 30
	end
	self.boxesTotal = 1
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
		self:updatePlayState(tickDelta)
	elseif self.state == PLAY_STATE_START then
		if self.panelAnim < 1 then
			self.panelAnim = self.panelAnim + tickDelta * 3
			if self.panelAnim > 1 then self.panelAnim = 1 end
		end
		-- on key or mouse start
		if not self.wasInput and (getPointerState(0) or getKeyState(0)) then
			self:startGame()
		end
	elseif self.state == PLAY_STATE_FINISH then
		if not self.wasInput and (getPointerState(0) or getKeyState(0)) then
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
	self.wasInput = (getPointerState(0) or getKeyState(0))
end

function Play:updatePlayState(tickDelta)
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
end

function Play:startGame()
	self.state = PLAY_STATE_PLAY
end

function Play:collectBox()
	self.boxesCollected = self.boxesCollected + 1
end

function Play:levelComplete()
	self.state = PLAY_STATE_FINISH
	self.success = true
end

function Play:levelFailed()
	self.state = PLAY_STATE_FINISH
	self.success = false
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
		game:renderAssimpM(nil, 0, "models/box.obj", nil, "textures/box_star.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,box.x,box.y,PLAY_BOX_SIZE/2,1, PLAY_BOX_SIZE,PLAY_BOX_SIZE,PLAY_BOX_SIZE, self.boxrot)
	end
	-- draw playing surface
	game:renderAssimpM(nil, 0, "models/box.obj", nil, "textures/box_surface.jpg", true, 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1, 100.0,100.0,1.0, 0)
	local timeString = string.format("Time: %2d", self.timeLeft)
	local renderIdx = game:renderText2D("ui", timeString, scaleX(640), scaleY(50))
	game:setRenderItemParam(renderIdx, "align", "center")
	renderIdx = game:renderText2D("ui", self.boxesCollected .. "/" .. self.boxesTotal, scaleX(1250), scaleY(50))
	game:setRenderItemParam(renderIdx, "align", "right")
	if self.state == PLAY_STATE_PLAY then
		-- minimap
		local mmx,mmy = scaleXY(105, 200)
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
		local x,y = scaleXY(650, 400)
		local w,h = scaleForUI(self.panelAnim * 500, self.panelAnim * 500)
		game:render2D("textures/play_panel_bg.png", x,y,w,h, 0)
		if self.panelAnim == 1 then
			x,y = scaleXY(640, 220)
			local renderIdx = game:renderText2D("ui", "Level " .. self.level, x,y)
			game:setRenderItemParam(renderIdx, "align", "center")
			x,y = scaleXY(640, 320)
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
	
	-- buttons
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	
end
