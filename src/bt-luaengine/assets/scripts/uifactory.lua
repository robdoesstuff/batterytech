
function makeButton(top,left, width,height, label)
	local sTop, sLeft = scaleForUI(top,left)
	local sWidth, sHeight = scaleForUI(width,height)
	local button = TouchControl.new(sTop, sLeft, sWidth, sHeight)
	button.normalAsset = "ui/button1_tex.png"
	button.pressedAsset = "ui/button1_pressed_tex.png"
	button.label = label
	return button
end