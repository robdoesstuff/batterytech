/*
 * MenuRenderer.cpp
 *
 *  Created on: Oct 7, 2010
 *      Author: rgreen
 */

#include "MenuRenderer.h"

MenuRenderer::MenuRenderer(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
}

S32 MenuRenderer::addTextureAsset(const char *asset) {

}

void MenuRenderer::render(UIManager *uiManager) {
	if (!texturesLoaded) {
		// load textures
	}
}

MenuRenderer::~MenuRenderer() {
}
