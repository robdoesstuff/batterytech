
function makeButton(left, top, width,height, label)
	local sLeft, sTop = scaleXY(left,top)
	local sWidth, sHeight = scaleForUI(width,height)
	local button = UIButton.new(sLeft, sTop, sWidth, sHeight)
	button.normalAsset = "ui/button1_tex.png"
	button.pressedAsset = "ui/button1_pressed_tex.png"
	button.label = label
	return button
end

function makeButtonCentered(x,y, width,height, label)
	local sX, sY = scaleXY(x,y)
	local sWidth, sHeight = scaleForUI(width,height)
	local button = UIButton.new(sX-sWidth/2, sY-sHeight/2, sWidth, sHeight)
	button.normalAsset = "ui/button1_tex.png"
	button.pressedAsset = "ui/button1_pressed_tex.png"
	button.label = label
	return button
end