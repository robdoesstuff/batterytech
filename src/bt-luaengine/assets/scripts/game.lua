-- Game Script

GAMESTATE_NONE = 0
GAMESTATE_RUNNING = 1
GAMESTATE_LOADING = 2

MODE_MAINMENU = 0
MODE_PLAY = 1
MODE_OPTIONS = 2
MODE_3D_TESTS = 3
MODE_PHYSICS_TESTS = 4

MUSIC = 0

-- called from engine when the Game is initializing
function Game:init()
	logmsg("Game init()")
	self.gameState = GAMESTATE_NONE
	self.mode = MODE_MAINMENU
	self.mainMenuModule = MainMenu.new()
	self.playModule = Play.new()
	self.optionsModule = Options.new()
    self.testsModule = Tests.new()
    self.physicsTestsModule = PhysicsTests.new()
	self.currentModule = self.mainMenuModule
	math.randomseed(os.time())
    math.random(); math.random(); math.random()
	setSoundEnabled(true)
	setVibrationEnabled(true)
	showFPS(true)
	setShadowType(0)
    self.waves = {Wave.new(0.1, 0.1),Wave.new(0.2, 0.2),Wave.new(0.3, 0.3),Wave.new(0.4, 0.4),Wave.new(0.5, 0.5),
    Wave.new(0.6, 0.6),Wave.new(0.15, 0.7),Wave.new(0.25, 0.8),Wave.new(0.35, 0.9),Wave.new(0.45, 1.0)}
end

function Game:reset(force)
	self:setMode(MODE_MAINMENU)
end

function Game:setMode(mode)
	self.mode = mode
	if mode == MODE_PLAY then
		self.currentModule = self.playModule
 	elseif mode == MODE_MAINMENU then
 		self.currentModule = self.mainMenuModule
 	elseif mode == MODE_OPTIONS then
 		self.currentModule = self.optionsModule
    elseif mode == MODE_3D_TESTS then
        self.currentModule = self.testsModule
    elseif mode == MODE_PHYSICS_TESTS then
        self.currentModule = self.physicsTestsModule
 	end
	self.currentModule:show()
end

-- called from engine to update the game
function Game:update(tickDelta)
	-- logmsg("updating")
	self.currentModule:update(tickDelta)
    self:updateWaves(tickDelta)
end

-- called from engine to render the game
function Game:render(state)
	self.currentModule:render()
end

-- ScreenControls are just C versions of TouchControls.  
-- Use either, but lua-managed ones may be easier for you
function Game:screenControlPointerDown(name, x, y)
end

function Game:screenControlPointerMove(name, x, y)
end

function Game:screenControlPointerUp(name, x, y)
end

function Game:updateWaves(tickDelta)
    for i = 1,#self.waves do
        local wave = self.waves[i]
        wave:update(tickDelta)
    end
end

-- Some common stuff to multiple modules
function Game:renderWaves()
    for i = 1,#self.waves do
        local wave = self.waves[i]
        wave:render()
    end
end
