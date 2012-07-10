-- Game Script

GAMESTATE_NONE = 0
GAMESTATE_RUNNING = 1
GAMESTATE_LOADING = 2

MODE_MAINMENU = 0
MODE_PLAY = 1
MODE_OPTIONS = 2
MODE_TESTS = 3

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
	self.currentModule = self.mainMenuModule
	math.randomseed(os.time())
	setSoundEnabled(true)
	setVibrationEnabled(true)
	showFPS(true)
	setShadowType(0)
	self.smileyX = 0
	self.smileyRotation = 0
end

function Game:setMode(mode)
	self.mode = mode
	if mode == MODE_PLAY then
		self.currentModule = self.playModule
 	elseif mode == MODE_MAINMENU then
 		self.currentModule = self.mainMenuModule
 	elseif mode == MODE_OPTIONS then
 		self.currentModule = self.optionsModule
    elseif mode == MODE_TESTS then
        self.currentModule = self.testsModule
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

-- Some common stuff to multiple modules
function Game:drawSmileys()
	local vpWidth, vpHeight = getViewportSize()
	sw, sh = scaleForUI(60, 60)
	for i = 0, 5 do
		local myX = self.smileyX + i * .2
		myX = myX % 1
		local smileyScaleX = vpWidth * myX
		local smileyScaleY = vpHeight - sh/2
		game:render2D("textures/smiley_tex.png", smileyScaleX, smileyScaleY, sw, sh, self.smileyRotation)
	end
end
