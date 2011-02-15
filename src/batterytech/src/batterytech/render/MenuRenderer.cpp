/*
 * MenuRenderer.cpp
 *
 *  Created on: Oct 7, 2010
 *      Author: rgreen
 */

#include "MenuRenderer.h"
#include "../decoders/stb_image.h"
#include "../platform/platformgeneral.h"
#include "../Logger.h"
#include "../ui/Layout.h"
#include <string.h>
#include "../../demo-app/UIConstants.h"

MenuRenderer::MenuRenderer(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
	assetNames = new ManagedArray<const char>(100);
	textRenderer = new TextRasterRenderer(gConfig, UI_MENU_FONT, 12.0f);
}

/** Adds a texture asset or returns the ID of the one already added if equal */
S32 MenuRenderer::addTextureAsset(const char *asset) {
	S32 id = findTextureAsset(asset);
	if (id == -1) {
		return assetNames->add(asset);
	}
	return id;
}

S32 MenuRenderer::findTextureAsset(const char *asset) {
	S32 i;
	for (i = 0; i < assetNames->getSize(); i++) {
		if (strcmp(asset, assetNames->array[i]) == 0) {
			return i;
		}
	}
	return -1;
}

void MenuRenderer::init(UIManager *uiManager) {
	textRenderer->init(TRUE);
	textureIds = new GLuint[assetNames->getSize()];
	glGenTextures(assetNames->getSize(), textureIds);
	S32 i;
	for (i = 0; i < assetNames->getSize(); i++) {
		glBindTexture(GL_TEXTURE_2D, textureIds[i]);
		loadTexture(assetNames->array[i]);
	}
}

void MenuRenderer::render(UIManager *uiManager) {
	// set the menu projection
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
	glLoadIdentity();
	glOrthof(0, gConfig->width, gConfig->height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glColor4f(1, 1, 1, 1);
	S32 i = 0;
	for (i = 0; i < uiManager->activeMenuStack->getSize(); i++) {
		Menu *menu = uiManager->activeMenuStack->array[i];
		UIComponent *root = menu->getRootComponent();
		activeResourceId = -1;
		render(root, uiManager);
	}
	glDisable(GL_BLEND);
}

void MenuRenderer::render(UIComponent *component, UIManager *uiManager) {
	UIAnimator *animator = component->getActiveAnimator();
	if (animator) {
		animator->drawPreComponent(gConfig);
	}
	if (!component->isEnabled) {
		glColor4f(0.5f,0.5f,0.5f,1.0f);
	} else {
		glColor4f(1,1,1,1);
	}
	// draw this component first then recurse children
	S32 menuResourceId = component->backgroundMenuResourceId;
	if (component->isPressed) {
		if (component->pressedBackgroundMenuResourceId != NO_RESOURCE) {
			menuResourceId = component->pressedBackgroundMenuResourceId;
		}
	} else {
		if (uiManager->selectedComponent == component) {
			if (component->selectedBackgroundMenuResourceId != NO_RESOURCE) {
				menuResourceId = component->selectedBackgroundMenuResourceId;
			}
		}
	}
	if (menuResourceId != NO_RESOURCE) {
		if (menuResourceId != activeResourceId) {
			glBindTexture(GL_TEXTURE_2D, textureIds[menuResourceId]);
			activeResourceId = menuResourceId;
		}
		drawTexturedQuad(component->top, component->right, component->bottom, component->left);
	}
	if (component->text) {
		if (component->isEnabled) {
			glColor4f(component->textR,component->textB,component->textG,component->textA);
		} else {
			glColor4f(component->textR*0.5f,component->textB*0.5f,component->textG*0.5f,component->textA);

		}
		activeResourceId = -1;
		// TODO - optimize for 1 pass text
		F32 textWidth = textRenderer->measureWidth(component->text);
		F32 textHeight = textRenderer->getHeight();
		F32 componentWidth = component->right - component->left;
		F32 componentHeight = component->bottom - component->top;
		F32 textLeft = component->left;
		F32 textBottom = component->bottom;
		if (component->textHorizontalAlignment == UIComponent::LEFT) {
			textLeft += component->paddingLeftDips * gConfig->uiScale;
		} else if (component->textHorizontalAlignment == UIComponent::HORIZONTAL_CENTER) {
			textLeft += (componentWidth / 2 - textWidth / 2);
		} else if (component->textHorizontalAlignment == UIComponent::RIGHT) {
			textLeft += (componentWidth - textWidth) - component->paddingRightDips * gConfig->uiScale;
		}
		if (component->textVerticalAlignment == UIComponent::BOTTOM) {
			textBottom -= component->paddingBottomDips * gConfig->uiScale;
		} else if (component->textVerticalAlignment == UIComponent::VERTICAL_CENTER) {
			textBottom -= (componentHeight / 2 - textHeight / 2);
		} else if (component->textVerticalAlignment == UIComponent::TOP) {
			textBottom -= (componentHeight - textHeight) - component->paddingTopDips * gConfig->uiScale;
		}
		textRenderer->startText();
		textRenderer->render(component->text, textLeft, textBottom);
		textRenderer->finishText();
	}
	if (component->components->getSize() > 0) {
		// recurse through layout's components
		UIComponent **components = component->components->array;
		int i;
		for (i = 0; i < component->components->getSize(); i++) {
			render(components[i], uiManager);
		}
	}
	glColor4f(1,1,1,1);
	if (animator) {
		animator->drawPostComponent(gConfig);
	}
}

void MenuRenderer::drawTexturedQuad(S32 top, S32 right, S32 bottom, S32 left) {
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	glFrontFace(GL_CW);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void MenuRenderer::loadTexture(const char *assetName) {
	logmsg("Loading texture");
	int x,y,n;
	int assetSize = 0;
	unsigned char *fileData = _platform_load_asset(assetName, &assetSize);
	if (fileData) {
		char buf[50];
		sprintf(buf, "Loaded %i bytes of raw image data", assetSize);
		logmsg(buf);
	}
	unsigned char *data = stbi_load_from_memory(fileData, assetSize, &x, &y, &n, 0);
	//unsigned char *data = stbi_load("assets\\text_bg_tex.jpg", &x, &y, &n, 0);
	if (data) {
		int bytes = x * y * n * sizeof(unsigned char);
		char buf[50];
		sprintf(buf, "Bitmap is %ix%i color components=%i bytes=%i",x,y,n, bytes);
		logmsg(buf);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		GLenum error = glGetError();
		if (error) {
			logmsg("GL Error");
		}
		stbi_image_free(data);
	}
	_platform_free_asset(fileData);
}

MenuRenderer::~MenuRenderer() {
	delete assetNames;
	delete [] textureIds;
}