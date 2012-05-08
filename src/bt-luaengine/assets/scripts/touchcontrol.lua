
TouchControl = {
	left = 0,
	top = 0,
	width = 0,
	height = 0,
	normalAsset = nil,
	pressedAsset = nil,
	label = nil,
	isPressed = false,
	fontTag = nil
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
	if self.isPressed then asset = self.pressedAsset end
	if asset then
		game:render2D(asset, self.left + self.width/2, self.top + self.height/2, self.width, self.height, 0)
	end
	if self.label then
		local fontTag = "ui"
		if self.fontTag then fontTag = self.fontTag end
		local tWidth, tHeight = game:measureText(fontTag, self.label, 1.0)
		game:renderText2D(fontTag, self.label, self.left + self.width/2 - tWidth/2, self.top + self.height/2 + tHeight/2)		
	end
end

