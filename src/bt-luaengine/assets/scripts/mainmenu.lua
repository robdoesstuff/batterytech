-- MainMenu module

MainMenu = {}

function MainMenu.new()
	local self = table.copy(MainMenu)
	self.buttons = {}
	local button = makeButtonCentered(640, 400, 380, 100, "Play")
	button.onClickUp = function()
		game:setMode(MODE_PLAY)
	end
	table.insert(self.buttons, button)
	button = makeButtonCentered(640, 510, 380, 100, "Options")
	button.onClickUp = function()
		game:setMode(MODE_OPTIONS)
	end
	table.insert(self.buttons, button)
	button = makeButtonCentered(640, 620, 380, 100, "Reload")
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
	game.smileyX = game.smileyX + tickDelta * .1
	game.smileyX = game.smileyX % 1
	game.smileyRotation = game.smileyRotation + tickDelta * 2.0
	if self.showAnim < 1 then
		self.showAnim = self.showAnim + tickDelta * 2
		if self.showAnim > 1 then
			self.showAnim = 1
		end
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
	-- game:render2D("textures/smiley_tex.png", 200, 200, 200, 200, 0)
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	game:drawSmileys()
end

function MainMenu:screenControlPointerDown(name, x, y)
end

function MainMenu:screenControlPointerMove(name, x, y)
end

function MainMenu:screenControlPointerUp(name, x, y)
end

