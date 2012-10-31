-- MainMenu module

MainMenu = {}

function MainMenu.new()
	local self = table.copy(MainMenu)
	self.buttons = {}
	local button = makeButtonCentered(640, 400, 440, 100, "Physics Demo")
	button.onClickUp = function()
		game:setMode(MODE_PHYSICS_TESTS)
	end
	table.insert(self.buttons, button)
	local button = makeButtonCentered(640, 510, 440, 100, "Basic 3D Tests")
	button.onClickUp = function()
		game:setMode(MODE_3D_TESTS)
	end
	table.insert(self.buttons, button)
	local button = makeButtonCentered(640, 620, 440, 100, "Example Game")
	button.onClickUp = function()
		game:setMode(MODE_PLAY)
	end
	table.insert(self.buttons, button)
	button = makeButtonCentered(180, 620, 280, 100, "Options")
	button.onClickUp = function()
		game:setMode(MODE_OPTIONS)
	end
	table.insert(self.buttons, button)
	button = makeButtonCentered(1100, 620, 280, 100, "Reload")
	button.onClickUp = function()
		logmsg("Reload")
		game:engineReset()
	end
	table.insert(self.buttons, button)
	self.showAnim = 0
	return self
end

function MainMenu:show()
	self.showAnim = 0
end

function MainMenu:update(tickDelta)
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


