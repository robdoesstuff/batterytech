-- List all resources here
--------------------------
-- addTexture(assetName)
-- addSound(assetName)
-- addDynamic(assetName)
-- addFont(tag, assetName, size, inner stroke, outer stroke, r,g,b,a)


addTexture("textures/smiley_tex.png")
addTexture("textures/box_surface.jpg")
addTexture("textures/box_star.jpg")
addTexture("textures/btluaenginelogo.png")
addTexture("textures/minimap_atlas.png")
addTexture("textures/arrow.png")
addTexture("textures/arrow_pressed.png")
addTexture("textures/play_panel_bg.png")
addTexture("textures/particle.png")
addTexture("ui/button1_tex.png")
addTexture("ui/button1_pressed_tex.png")
addTexture("textures/circle.png")
addTexture("textures/rectangle.png")
addAssimp("models/box.obj")


-- tag, assetname, size, innerstroke, outerstroke, color
addFont("title", "ui/digital.ttf", 100, 1, 1, 255,255,255,255)
-- addFont("test", "ui/bm-alt.fnt", 32, 0, 0, 255,255,255,255)
-- addFont("test2", "ui/bm46.fnt", 46, 0, 0, 255,255,255,255)

addSound("sounds/click.ogg")
addSound("sounds/laser_echo.ogg")
addSound("sounds/space_noises.ogg")
addSound("sounds/whoosh.ogg")

function addTestResources()
    addAssimp("models/Seymour.dae")
    addTexture("models/Seymour.jpg")
end