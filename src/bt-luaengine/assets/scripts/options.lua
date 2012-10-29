-- Options module

Options = {}

function Options.new()
	local self = table.copy(Options)
	self.soundEnabled = true
	self.showFPS = true
	self.buttons = {}
	local button = makeButtonCentered(640, 400, 380, 100, "Sound: On")
	button.onClickUp = function(button)
		self.soundEnabled = not self.soundEnabled
		setSoundEnabled(self.soundEnabled)
		if self.soundEnabled then
			button.label = "Sound: On"
		else
			button.label = "Sound: Off"
		end
	end
	table.insert(self.buttons, button)
	button = makeButtonCentered(640, 510, 380, 100, "ShowFPS: On")
	button.onClickUp = function(button)
		self.showFPS = not self.showFPS
		showFPS(self.showFPS)
		if self.showFPS then
			button.label = "ShowFPS: On"
		else
			button.label = "ShowFPS: Off"
		end
	end
	table.insert(self.buttons, button)
	button = makeButtonCentered(640, 620, 380, 100, "Back")
	button.onClickUp = function()
		game:setMode(MODE_MAINMENU)
	end
	table.insert(self.buttons, button)
	return self
end

function Options:show()
end

function Options:update(tickDelta)
	for i,v in ipairs(self.buttons) do
		v:update(tickDelta)
	end
	local c, sc = getLastCharacterTyped()
	-- special key 1 is "back" button when available
	if sc == 1 then
		-- this will always emulate the exit button
		game:setMode(MODE_MAINMENU)
	end
end

function Options:render()
	local vpWidth, vpHeight = getViewportSize()
	local itemIdx = game:renderText2D("title", "Options", vpWidth/2, scaleY(100))
	game:setRenderItemParam(itemIdx, "align", "center")
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	game:renderWaves()
end


