-- Game Script

GAMESTATE_NONE = 0
GAMESTATE_RUNNING = 1
GAMESTATE_LOADING = 2

MODE_MAINMENU = 0

-- called from engine when the Game is initializing
function Game:init()
	logmsg("Game init()")
	self.gameState = GAMESTATE_NONE
	self.mode = MODE_MAINMENU
	math.randomseed(os.time())
	setSoundEnabled(true)
	setVibrationEnabled(true)
	showFPS(true)
	setShadowType(0)
end

function Game:setMode(mode)
	self.mode = mode
	if mode == MODE_MAINMENU then
		-- do something if you care
 	end
end

-- called from engine to update the game
function Game:update(tickDelta)
end

-- called from engine to render the game
function Game:render(state)
end

function Game:screenControlPointerDown(name, x, y)
end

function Game:screenControlPointerMove(name, x, y)
end

function Game:screenControlPointerUp(name, x, y)
end
