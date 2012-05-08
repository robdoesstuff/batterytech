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
	return self
end

function MainMenu:show()
end

function MainMenu:update(tickDelta)
	for i,v in ipairs(self.buttons) do
		v:update(tickDelta)
	end
end

function MainMenu:render(state)
	for i,v in ipairs(self.buttons) do
		v:render()
	end
end

function MainMenu:screenControlPointerDown(name, x, y)
end

function MainMenu:screenControlPointerMove(name, x, y)
end

function MainMenu:screenControlPointerUp(name, x, y)
end

