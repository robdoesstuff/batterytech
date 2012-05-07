-- Options module

Options = {}

function Options.new()
	local self = table.copy(Options)
	return self
end

function Options:show()
end

function Options:update(tickDelta)
end

function Options:render(state)
end

function Options:screenControlPointerDown(name, x, y)
end

function Options:screenControlPointerMove(name, x, y)
end

function Options:screenControlPointerUp(name, x, y)
end

