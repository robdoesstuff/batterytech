-- Game Script

GAMESTATE_NONE = 0
GAMESTATE_RUNNING = 1
GAMESTATE_LOADING = 2

MODE_MAINMENU = 0
MODE_PLAY = 1
MODE_OPTIONS = 2

MUSIC = 0

-- called from engine when the Game is initializing
function Game:init()
	logmsg("Game init()")
	self.gameState = GAMESTATE_NONE
	self.mode = MODE_MAINMENU
	self.mainMenuModule = MainMenu.new()
	self.playModule = Play.new()
	self.optionsModule = Options.new()
	self.currentModule = self.mainMenuModule
	math.randomseed(os.time())
	setSoundEnabled(true)
	setVibrationEnabled(true)
	showFPS(true)
	setShadowType(0)
end

function Game:setMode(mode)
	self.mode = mode
	if mode == MODE_PLAY then
		self.currentModule = self.playModule
 	elseif mode == MODE_MAINMENU then
 		self.currentModule = self.mainMenuModule
 	elseif mode == MODE_OPTIONS then
 		self.currentModule = self.optionsModule
 	end
	self.currentModule:show()
end

-- called from engine to update the game
function Game:update(tickDelta)
	-- logmsg("updating")
	self.currentModule:update(tickDelta)
end

-- called from engine to render the game
function Game:render(state)
	game:render2D("textures/smiley_tex.png", 200, 200, 200, 200, 0)
	game:render2D("textures/smiley_tex.png", 300, 200, 200, 200, 0)
	game:render2D("textures/smiley_tex.png", 400, 200, 200, 200, 0)
	self.currentModule:render()
end

function Game:screenControlPointerDown(name, x, y)
	self.currentModule:screenControlPointerDown(name, x, y)
end

function Game:screenControlPointerMove(name, x, y)
	self.currentModule:screenControlPointerMove(name, x, y)
end

function Game:screenControlPointerUp(name, x, y)
	self.currentModule:screenControlPointerUp(name, x, y)

end

