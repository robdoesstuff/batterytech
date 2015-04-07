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

NEXT_KEYS = {[39]=39, [40]=40, [119]=119, [121]=121}
PREV_KEYS = {[37]=37, [38]=38, [118]=118, [120]=120}
SELECTION_KEYS = {[13]=13, [96]=96}


gameSettings = nil

-- called from engine when the Game is initializing
function Game:init()
	logmsg("Game init()")
	math.randomseed(os.time())
    math.random(); math.random(); math.random()
    loadGame()
	self.gameState = GAMESTATE_NONE
	self.mode = MODE_MAINMENU
	self.mainMenuModule = MainMenu.new()
	self.playModule = Play.new()
	self.optionsModule = Options.new()
    self.testsModule = Tests.new()
    self.physicsTestsModule = PhysicsTests.new()
	self.currentModule = self.mainMenuModule
    self.waves = {Wave.new(0.1, 0.1),Wave.new(0.2, 0.2),Wave.new(0.3, 0.3),Wave.new(0.4, 0.4),Wave.new(0.5, 0.5),
    Wave.new(0.6, 0.6),Wave.new(0.15, 0.7),Wave.new(0.25, 0.8),Wave.new(0.35, 0.9),Wave.new(0.45, 1.0)}
    self.fpsCounter = FPSCounter.new()
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
	self.currentModule:update(tickDelta)
    self:updateWaves(tickDelta)
    self.fpsCounter:update(tickDelta)
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

function loadGame()
	local savepath = getApplicationStorageDirectoryName()..getPathSeparator().."savegame.data"
	if pathExists(savepath) then
		logmsg("loading from: "..savepath)
		execScript(savepath, true, true)
	end
	if not saveState then
		saveState = {}
		saveState.soundEnabled = true
		saveState.vibrationEnabled = true
		saveState.showFPS = false
	end
	setSoundEnabled(saveState.soundEnabled)
	setVibrationEnabled(saveState.vibrationEnabled)
	showFPS(saveState.showFPS)
	setShadowType(0)
end

function saveGame()
	local savepath = getApplicationStorageDirectoryName()..getPathSeparator().."savegame.data"
	local save = table.toString(saveState, "saveState")
	io.output(savepath)
	io.write(save)
	io.close()
end


FPSCounter = {}
function FPSCounter.new()
	local self = table.copy(FPSCounter)
	self.sampleCount = 0
	self.timeTotal = 0
	self.fps = 0
	return self
end

function FPSCounter:update(delta)
	self.sampleCount = self.sampleCount + 1
	self.timeTotal = self.timeTotal + delta
	if self.sampleCount == 10 then
		self.sampleCount = 0
		self.fps = math.ceil(10.0/self.timeTotal)
		self.timeTotal = 0
	end
end

function FPSCounter:reset()
	self.sampleCount = 0
	self.fps = 0
	self.timeTotal = 0
end
