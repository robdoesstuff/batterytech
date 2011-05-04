/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : MenuRenderer.cpp
// Description : Renders Menus and UIComponents
//============================================================================

#include "MenuRenderer.h"
#include "../decoders/stb_image.h"
#include "../platform/platformgeneral.h"
#include "../Logger.h"
#include "../ui/Layout.h"
#include <string.h>
#include "../Context.h"
#include "../../demo-app/UIConstants.h"
#include "RenderContext.h"
#include "../batterytech_globals.h"
#include "ShaderProgram.h"

namespace BatteryTech {

	MenuRenderer::MenuRenderer(Context *context) : Renderer() {
		this->context = context;
		assetNames = new ManagedArray<const char>(MAX_UI_ASSET_NAMES);
		textRenderer = new TextRasterRenderer(context, UI_MENU_FONT, UI_FONT_SIZE);
		shaderProgram = new ShaderProgram("shaders/quadshader.vert", "shaders/quadshader.frag");
	}

	MenuRenderer::~MenuRenderer() {
		delete assetNames;
		delete [] textureIds;
		delete textRenderer;
		delete shaderProgram;
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

	void MenuRenderer::init(BOOL32 newContext) {
		textRenderer->init(TRUE);
		textureIds = new GLuint[assetNames->getSize()];
		glGenTextures(assetNames->getSize(), textureIds);
		S32 i;
		for (i = 0; i < assetNames->getSize(); i++) {
			glBindTexture(GL_TEXTURE_2D, textureIds[i]);
			loadTexture(assetNames->array[i]);
		}
		if (context->gConfig->useShaders) {
			shaderProgram->init(newContext);
			shaderProgram->addVertexAttributeLoc("vPosition");
			shaderProgram->addVertexAttributeLoc("uvMap");
			shaderProgram->addUniformLoc("projection_matrix");
			shaderProgram->addUniformLoc("modelview_matrix");
			shaderProgram->addUniformLoc("tex");
			shaderProgram->addUniformLoc("colorFilter");
		}
		checkGLError("MenuRenderer init");
	}

	void MenuRenderer::render() {
		checkGLError("MenuRenderer Start");
		// set the menu projection
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, context->gConfig->viewportWidth, context->gConfig->viewportHeight);
		if (context->gConfig->useShaders) {
			context->renderContext->projMatrix.identity();
			context->renderContext->projMatrix.ortho(0, context->gConfig->width, context->gConfig->height, 0, -1, 1);
			context->renderContext->mvMatrix.identity();
		} else {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			// top left is 0,0, so scissor will be inverted y
			glOrthof(0, context->gConfig->width, context->gConfig->height, 0, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glColor4f(1, 1, 1, 1);
		}
		S32 i = 0;
		for (i = 0; i < context->uiManager->activeMenuStack->getSize(); i++) {
			Menu *menu = context->uiManager->activeMenuStack->array[i];
			UIComponent *root = menu->getRootComponent();
			activeResourceId = -1;
			render(root);
		}
		glDisable(GL_BLEND);
		checkGLError("MenuRenderer End");
	}

	void MenuRenderer::render(UIComponent *component) {
		UIAnimator *animator = component->getActiveAnimator();
		UIManager *uiManager = context->uiManager;
		if (animator) {
			animator->drawPreComponent(context);
		}
		// if virtual size bigger than drawable area, scissor to drawable
		if (component->isVirtualLargerThanDrawable()) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(component->left, context->gConfig->viewportHeight - component->bottom, component->right - component->left, component->bottom - component->top);
		}
		if (!component->isEnabled) {
			context->renderContext->colorFilter = Vector4f(0.5f,0.5f,0.5f,1.0f);
		} else {
			context->renderContext->colorFilter = Vector4f(1,1,1,1);
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
			drawTexturedQuad(component->virtualTop, component->virtualRight, component->virtualBottom, component->virtualLeft);
		}
		if (component->text) {
			if (component->isEnabled) {
				context->renderContext->colorFilter = Vector4f(component->textR,component->textB,component->textG,component->textA);
			} else {
				context->renderContext->colorFilter = Vector4f(component->textR*0.5f,component->textB*0.5f,component->textG*0.5f,component->textA);
			}
			activeResourceId = -1;
			// TODO - optimize for 1 pass text
			F32 componentWidth = component->virtualRight - component->virtualLeft;
			F32 componentHeight = component->virtualBottom - component->virtualTop;
			F32 textWidth = textRenderer->measureWidth(component->text);
			F32 textLineHeight = textRenderer->getHeight();
			F32 totalTextHeight = textLineHeight;
			if (component->isTextMultiline) {
				// take padding into account
				totalTextHeight = textRenderer->measureMultilineHeight(component->text, componentWidth - (component->paddingLeftDips + component->paddingRightDips) * context->gConfig->uiScale);
			}
			F32 textBottomOffset = totalTextHeight - textLineHeight;
			F32 textLeft = component->virtualLeft;
			// confusing - textBottom refers to the bottom of the _first_ row of text, which is the top row in multiline.
			F32 textBottom = component->virtualBottom;
			if (component->textHorizontalAlignment == UIComponent::LEFT) {
				textLeft += component->paddingLeftDips * context->gConfig->uiScale;
			} else if (component->textHorizontalAlignment == UIComponent::HORIZONTAL_CENTER) {
				textLeft += (componentWidth / 2 - textWidth / 2);
			} else if (component->textHorizontalAlignment == UIComponent::RIGHT) {
				textLeft += (componentWidth - textWidth) - component->paddingRightDips * context->gConfig->uiScale;
			}
			if (component->textVerticalAlignment == UIComponent::BOTTOM) {
				textBottom -= textBottomOffset;
				textBottom -= component->paddingBottomDips * context->gConfig->uiScale;
			} else if (component->textVerticalAlignment == UIComponent::VERTICAL_CENTER) {
				textBottom -= (componentHeight / 2 + totalTextHeight / 2) - textLineHeight;
			} else if (component->textVerticalAlignment == UIComponent::TOP) {
				textBottom -= (componentHeight - textLineHeight) - component->paddingTopDips * context->gConfig->uiScale;
			}
			if (textLeft < component->virtualLeft + component->paddingLeftDips * context->gConfig->uiScale) {
				textLeft = component->virtualLeft + component->paddingLeftDips * context->gConfig->uiScale;
			}
			if (textBottom > component->virtualBottom - component->paddingBottomDips * context->gConfig->uiScale) {
				textBottom = component->virtualBottom - component->paddingBottomDips * context->gConfig->uiScale;
			}
			textRenderer->startText();
			if (component->isTextMultiline) {
				textRenderer->renderMultiline(component->text, textLeft, textBottom, component->virtualRight - component->paddingRightDips * context->gConfig->uiScale, component->virtualBottom - component->paddingBottomDips * context->gConfig->uiScale);
			} else {
				textRenderer->render(component->text, textLeft, textBottom);
			}
			textRenderer->finishText();
		}
		if (component->components->getSize() > 0) {
			// recurse through layout's components
			UIComponent **components = component->components->array;
			int i;
			for (i = 0; i < component->components->getSize(); i++) {
				render(components[i]);
			}
		}
		context->renderContext->colorFilter = Vector4f(1,1,1,1);
		if (component->isVirtualLargerThanDrawable()) {
			glDisable(GL_SCISSOR_TEST);
		}
		if (animator) {
			animator->drawPostComponent(context);
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
		if (context->gConfig->useShaders) {
			// this is less than efficient because of the layered text rendering
			shaderProgram->bind();
			glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
			glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
			glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
			glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
			glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
			Vector4f colorFilter = context->renderContext->colorFilter;
			glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			shaderProgram->unbind();
		} else {
			Vector4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
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
			if (!context->gConfig->useShaders) {
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			if (n == 3) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			} else if (n == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			checkGLError("MenuRenderer Load Texture");
			stbi_image_free(data);
		}
		_platform_free_asset(fileData);
	}

	BOOL32 MenuRenderer::isMultiline(const char *text, F32 availableWidth) {
		S32 length = strlen(text);
		for (S32 i = 0; i < length; i++) {
			if (text[i] == '\n' || text[i] == '\r') {
				return true;
			}
		}
		F32 width = measureTextWidth(text);
		if (width > availableWidth) {
			return true;
		}
		return false;
	}

	F32 MenuRenderer::measureMultilineHeight(const char *text, F32 availableWidth) {
		return textRenderer->measureMultilineHeight(text, availableWidth);
	}

	F32 MenuRenderer::measureTextWidth(const char *text) {
		return textRenderer->measureWidth(text);
	}

	F32 MenuRenderer::getTextHeight() {
		return textRenderer->getHeight();
	}

}
