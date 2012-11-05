-- This is the first script that will be run by BT Lua Engine.

execScript("scripts/game.lua")
execScript("scripts/util.lua")
execScript("scripts/mainmenu.lua")
execScript("scripts/options.lua")
execScript("scripts/play.lua")
execScript("scripts/resources.lua")
execScript("scripts/touchcontrol.lua")
execScript("scripts/uifactory.lua")
execScript("scripts/tests.lua")
execScript("scripts/phystests.lua")
execScript("scripts/wave.lua")

-- place any other scripts you want executed here


-- instantiate the global game instance (this stays with us until lua is shut down)
-- this should always be the last thing after loading all scripts!
game = allocMeta(Game.getInstance())
