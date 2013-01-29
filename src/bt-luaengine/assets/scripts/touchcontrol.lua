
TouchControl = {
	left = 0,
	top = 0,
	width = 0,
	height = 0,
	normalAsset = nil,
	pressedAsset = nil,
	selectedAsset = nil,
	label = nil,
	isPressed = false,
	isSelected = false,
	uvs = nil,
	fontTag = nil,
	rotation = 0
}

function TouchControl.new(left, top, width, height)
	local self = table.copy(TouchControl)
	self.left = left
	self.top = top
	self.width = width
	self.height = height
	return self
end

function TouchControl:update(delta)
	self.isPressed = false
	for i = 0,9 do
		local isDown, x,y = getPointerState(i)
		if isDown then
			if x < self.left or x > self.left + self.width or y < self.top or y > self.top + self.height then
				-- nothing
			else
				self.isPressed = true
			end
		end
	end
end

function TouchControl:render()
	local asset = self.normalAsset
	if self.isPressed then 
		asset = self.pressedAsset 
	else
		if self.isSelected then
			asset = self.selectedAsset
		end
	end
	if asset then
		local itemIdx = game:render2D(asset, self.left + self.width/2, self.top + self.height/2, self.width, self.height, self.rotation * (math.pi/180))
		if self.uvs then
			local uvs = self.uvs
			game:setRenderItemParam(itemIdx, "uvs", uvs[1],uvs[2],uvs[3],uvs[4])
		end
	end
	if self.label then
		local fontTag = "ui"
		if self.fontTag then fontTag = self.fontTag end
		local tWidth, tHeight = game:measureText(fontTag, self.label, 1.0)
		game:renderText2D(fontTag, self.label, self.left + self.width/2 - tWidth/2, self.top + self.height/2 + tHeight/2)		
	end
end

-- Sets if this control is currently in a selected (not necessarily pressed) state
-- selection is used for keyboard highlight/control
function TouchControl:setSelected(selected)
	self.isSelected = selected
end

-------------- UI Button specifics ---------------
-- UIButton is a TouchControl which behaves more like standard buttons on a UI.  
-- You have to press down in it, not slide into it.
-- It will also fire click events
-- It only responds to pointer 0

UIButton = table.copy(TouchControl)

function UIButton.new(left, top, width, height)
	local self = table.copy(UIButton)
	self.left = left
	self.top = top
	self.width = width
	self.height = height
	self.pointerWasDown = false
	self.onClickDown = nil
	self.onClickUp = nil
	self.lastX = 0
	self.lastY = 0
	return self
end

function UIButton:update(delta)
	local isDown, x,y = getPointerState(0)
	if isDown and not self.pointerWasDown then
		-- touch down
		if x < self.left or x > self.left + self.width or y < self.top or y > self.top + self.height then
			-- nothing
		else
			playSound("sounds/click.ogg")
			-- fire down event
				if self.onClickDown then
					self:onClickDown()
				end
			self.isPressed = true
		end
	elseif not isDown and self.pointerWasDown then
		-- touch up
		if self.isPressed then
			if self.lastX < self.left or self.lastX > self.left + self.width or self.lastY < self.top or self.lastY > self.top + self.height then
				-- nothing
			else
				-- fire up event
				if self.onClickUp then
					self:onClickUp()
				end
			end
		end
		self.isPressed = false
	end
	self.pointerWasDown = isDown
	self.lastX = x
	self.lastY = y
end


------ TouchControlFocusManager is a class that will control keyboard-based focus of TouchControls -------

TouchControlFocusManager = {}
-- nextKeys and prevKeys need to be hashed lists, so use {30=30, 35=35}, etc
function TouchControlFocusManager.new(prevKeys, nextKeys, selectionKeys)
	local self = table.copy(TouchControlFocusManager)
	self.controls = {}
	self.focusedControlIdx = 0
	self.nextKeys = nextKeys
	self.prevKeys = prevKeys
	self.selectionKeys = selectionKeys
	self.wasDown = false
	return self
end

function TouchControlFocusManager:update(delta)
	-- check for key selection change
	local isDown, keyCode = getKeyState(0)
	if isDown and not self.wasDown then
		if self.nextKeys[keyCode] then
			self:focusNext()
		elseif self.prevKeys[keyCode] then
			self:focusPrev()
		elseif self.selectionKeys[keyCode] then
			if self.controls[self.focusedControlIdx] then
				if self.controls[self.focusedControlIdx].onClickDown then
					self.controls[self.focusedControlIdx]:onClickDown()
				end
				if self.controls[self.focusedControlIdx].onClickUp then
					self.controls[self.focusedControlIdx]:onClickUp()
				end
			end
		end
	end
	self.wasDown = isDown
end

function TouchControlFocusManager:addControl(touchControl)
	self.controls[#self.controls+1] = touchControl
end

function TouchControlFocusManager:clear()
	self.controls = {}
	self.focusedControlIdx = 0
end

function TouchControlFocusManager:focusNext()
	if self.focusedControlIdx ~= 0 then
		self.controls[self.focusedControlIdx]:setSelected(false)
	end
	if #self.controls == 0 then
		self.focusedControlIdx = 0
	else
		self.focusedControlIdx = self.focusedControlIdx + 1
		if self.focusedControlIdx > #self.controls then
			self.focusedControlIdx = 1
		end
		if self.focusedControlIdx ~= 0 then
			self.controls[self.focusedControlIdx]:setSelected(true)
		end
	end
end

function TouchControlFocusManager:focusPrev()
	if self.focusedControlIdx ~= 0 then
		self.controls[self.focusedControlIdx]:setSelected(false)
	end
	if #self.controls == 0 then
		self.focusedControlIdx = 0
	else
		self.focusedControlIdx = self.focusedControlIdx - 1
		if self.focusedControlIdx <= 0 then
			self.focusedControlIdx = #self.controls
		end
		if self.focusedControlIdx ~= 0 then
			self.controls[self.focusedControlIdx]:setSelected(true)
		end
	end
end