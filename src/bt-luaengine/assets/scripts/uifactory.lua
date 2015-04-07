
function makeButton(left, top, width,height, label)
	local sLeft, sTop = scaleXY(left,top)
	local sWidth, sHeight = scaleForUI(width,height)
	local button = UIButton.new(sLeft, sTop, sWidth, sHeight)
	button.normalAsset = "ui/button1_tex.png"
	button.pressedAsset = "ui/button1_pressed_tex.png"
	button.selectedAsset = "ui/button1_selected_tex.png"
	button.label = label
	return button
end

function makeButtonCentered(x,y, width,height, label, normalAsset, pressedAsset, selectedAsset)
	local sX, sY = scaleXY(x,y)
	local sWidth, sHeight = scaleForUI(width,height)
	local button = UIButton.new(sX-sWidth/2, sY-sHeight/2, sWidth, sHeight)
	if not normalAsset then
		button.normalAsset = "ui/button1_tex.png"
	else
		button.normalAsset = normalAsset
	end
	if not pressedAsset then
		button.pressedAsset = "ui/button1_pressed_tex.png"
	else
		button.pressedAsset = pressedAsset
	end
	if not selectedAsset then
		button.selectedAsset = "ui/button1_selected_tex.png"
	else
		button.selectedAsset = selectedAsset
	end
	button.label = label
	return button
end