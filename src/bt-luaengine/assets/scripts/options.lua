-- Options module

Options = {}

function Options.new()
	local self = table.copy(Options)
	self.focusMgr = TouchControlFocusManager.new(PREV_KEYS, NEXT_KEYS, SELECTION_KEYS)
	self.buttons = {}
	local label = "Sound: On"
	if not saveState.soundEnabled then
		label = "Sound: Off"
	end
	local button = makeButtonCentered(640, 400, 380, 100, label)
	button.onClickUp = function(button)
		saveState.soundEnabled = not saveState.soundEnabled
		setSoundEnabled(saveState.soundEnabled)
		if saveState.soundEnabled then
			button.label = "Sound: On"
		else
			button.label = "Sound: Off"
		end
		saveGame()
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

	local label = "ShowFPS: On"
	if not saveState.showFPS then
		label = "ShowFPS: Off"
	end
	button = makeButtonCentered(640, 510, 380, 100, label)
	button.onClickUp = function(button)
		saveState.showFPS = not saveState.showFPS
		showFPS(saveState.showFPS)
		if saveState.showFPS then
			button.label = "ShowFPS: On"
		else
			button.label = "ShowFPS: Off"
		end
		saveGame()
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

	button = makeButtonCentered(640, 620, 380, 100, "Back")
	button.onClickUp = function()
		game:setMode(MODE_MAINMENU)
	end
	table.insert(self.buttons, button)
	self.focusMgr:addControl(button)

	return self
end

function Options:show()
end

function Options:update(tickDelta)
	self.focusMgr:update(tickDelta)
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
	local itemIdx = game:renderText2D("ui", "Options", vpWidth/2, scaleY(100), 1.5)
	game:setRenderItemParam(itemIdx, "align", "center")
	for i,v in ipairs(self.buttons) do
		v:render()
	end
	game:renderWaves()
end


