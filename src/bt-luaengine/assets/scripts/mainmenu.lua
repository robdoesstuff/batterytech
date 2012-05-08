-- MainMenu module

MainMenu = {}

function MainMenu.new()
	local self = table.copy(MainMenu)
	self.buttons = {}
	local button = makeButton(500, 300, 280, 100, "Play")
	button.onClickUp = function()
		game:setMode(MODE_PLAY)
	end
	table.insert(self.buttons, button)
	button = makeButton(500, 410, 280, 100, "Options")
	button.onClickUp = function()
		game:setMode(MODE_OPTIONS)
	end
	table.insert(self.buttons, button)
	button = makeButton(500, 520, 280, 100, "Reload")
	button.onClickUp = function()
		logmsg("Reload")
		game:engineReset()
	end
	table.insert(self.buttons, button)
	self.smileyX = 0
	self.smileyRotation = 0
	return self
end

function MainMenu:show()
end

function MainMenu:update(tickDelta)
	for i,v in ipairs(self.buttons) do
		v:update(tickDelta)
	end
	self.smileyX = self.smileyX + tickDelta * .1
	self.smileyX = self.smileyX % 1
	self.smileyRotation = self.smileyRotation + tickDelta * 2.0
end

function MainMenu:render(state)
	local vpWidth, vpHeight = getViewportSize()
	local sw, sh = scaleForUI(1024, 256)
	game:render2D("textures/btluaenginelogo.png", vpWidth/2, scaleY(150), sw, sh)
	-- game:render2D("textures/smiley_tex.png", 200, 200, 200, 200, 0)
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	sw, sh = scaleForUI(60, 60)
	for i = 0, 5 do
		local smileyX = self.smileyX + i * .2
		smileyX = smileyX % 1
		local smileyScaleX = vpWidth * smileyX
		local smileyScaleY = vpHeight - sh/2
		game:render2D("textures/smiley_tex.png", smileyScaleX, smileyScaleY, sw, sh, self.smileyRotation)
	end
end

function MainMenu:screenControlPointerDown(name, x, y)
end

function MainMenu:screenControlPointerMove(name, x, y)
end

function MainMenu:screenControlPointerUp(name, x, y)
end

