-- MainMenu module

MainMenu = {}

function MainMenu.new()
	local self = table.copy(MainMenu)

	self.focusMgr = TouchControlFocusManager.new(PREV_KEYS, NEXT_KEYS, SELECTION_KEYS)
	self.buttons = {}
    local x = 640
    local y = 400
    local width = 440
    local height = 100
    local yNext = 110
	local button = makeButtonCentered(x, y, width, height, "Example Game")
	button.onClickUp = function()
		game:setMode(MODE_PLAY)
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)
	
    y = y + yNext
	local button = makeButtonCentered(x, y, width, height, "Physics Demo")
	button.onClickUp = function()
		game:setMode(MODE_PHYSICS_TESTS)
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

    y = y + yNext
	local button = makeButtonCentered(x, y, width, height, "Basic 3D Tests")
	button.onClickUp = function()
		game:setMode(MODE_3D_TESTS)
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

	button = makeButtonCentered(180, 620, 280, 100, "Options")
	button.onClickUp = function()
		game:setMode(MODE_OPTIONS)
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

	button = makeButtonCentered(1100, 620, 280, 100, "Reload")
	button.onClickUp = function()
		logmsg("Reload")
		game:engineReset()
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

	self.showAnim = 0
	return self
end

function MainMenu:show()
	self.showAnim = 0
end

function MainMenu:update(tickDelta)
	self.focusMgr:update(tickDelta)
	for i,v in ipairs(self.buttons) do
		v:update(tickDelta)
	end
	if self.showAnim < 1 then
		self.showAnim = self.showAnim + tickDelta * 2
		if self.showAnim > 1 then
			self.showAnim = 1
		end
	end
	-- check for back button
	local c, sc = getLastCharacterTyped()
	-- special key 1 is "back" button when available
	if sc == 1 then
		-- this will always emulate the exit button
		platformExit()
	end
	
end

function MainMenu:render()
	local vpWidth, vpHeight = getViewportSize()
	local sw, sh = scaleForUI(1024, 256)
	local animAmt = self.showAnim + 0.15
	if animAmt > 1 then
		animAmt = animAmt - (animAmt % 1) * 2
	end
	game:render2D("textures/btluaenginelogo.png", vpWidth/2, scaleY(150), sw*animAmt * 1.1, sh*animAmt * 1.1, 0)
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	game:renderWaves()
end


