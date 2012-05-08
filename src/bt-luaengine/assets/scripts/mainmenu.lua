-- MainMenu module

MainMenu = {}

function MainMenu.new()
	local self = table.copy(MainMenu)
	self.buttons = {}
	local button = makeButton(500, 300, 280, 100, "test")
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

