-- Wave script

local alphaSpeed = 0.5
local colorTime = 3

Wave = {}

function Wave.new(speed, startX)
    local self = table.copy(Wave)
    self.speed = speed
    self.x = startX
    self.alpha = math.random()
    if math.random() > .5 then self.alphaDir = 1 else self.alphaDir = 0 end
    self.curColorR = math.random()
    self.curColorG = math.random()
    self.curColorB = math.random()
    self.lastColorR = self.curColorR
    self.lastColorG = self.curColorG
    self.lastColorB = self.curColorB
    self.colorTimeLeft = 0
    return self
end

function Wave:update(delta)
    self.x = self.x + delta * self.speed / 8
    if self.alphaDir == 1 then
        self.alpha = self.alpha + delta * alphaSpeed
        if self.alpha >= 1.0 then
            self.alphaDir = 0
            self.alpha = 1.0
        end
    else
        self.alpha = self.alpha - delta * alphaSpeed
        if self.alpha <= 0.0 then
            self.alphaDir = 1
            self.alpha = 0
        end
    end
    self.colorTimeLeft = self.colorTimeLeft - delta
    if self.colorTimeLeft <= 0 then
        self.colorTimeLeft = colorTime
        self.lastColorR = self.curColorR
        self.lastColorG = self.curColorG
        self.lastColorB = self.curColorB
        self.curColorR = math.random()
        self.curColorG = math.random()
        self.curColorB = math.random()
    end
end

function Wave:render()
	local vpWidth, vpHeight = getViewportSize()
	sw, sh = scaleForUI(60, 60)
    local renderItem = game:render2D("textures/sinewave1.png", vpWidth/2, vpHeight - sh/2, vpWidth, sh, 0)
    game:setRenderItemParam(renderItem, "uvs", self.x,0,1+self.x,1)
    local lerpMix = 1-(self.colorTimeLeft / colorTime)
    local r = lerp1(self.curColorR, self.lastColorR, lerpMix)
    local g = lerp1(self.curColorG, self.lastColorG, lerpMix)
    local b = lerp1(self.curColorB, self.lastColorB, lerpMix)
    game:setRenderItemParam(renderItem, "colorFilter", r,g,b, self.alpha)

end

