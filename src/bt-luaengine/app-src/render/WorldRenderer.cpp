/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

#include "WorldRenderer.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Logger.h>
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/util/strx.h>
#include <batterytech/render/MenuRenderer.h>
#include <batterytech/render/ShaderProgram.h>
#include <batterytech/render/AssetTexture.h>
#include <batterytech/render/QuadRenderer.h>
#include <batterytech/video/VideoManager.h>
#include "AssimpRenderer.h"
#include "ScreenControlRenderer.h"
#include "ParticleEmitterRenderer.h"
#ifdef BATTERYTECH_INCLUDE_BOX2D
#include "B2DebugRenderer.h"
#endif
#include "RenderItem.h"
#include "../UIConstants.h"
#include "../GameUtil.h"
#include "../Game.h"
#include "../GameContext.h"
#include <math.h>
#include <time.h>

using namespace BatteryTech;

WorldRenderer::WorldRenderer(GameContext *context) {
	this->context = context;
	this->gConfig = context->gConfig;
	context->world->camera = new Camera(context, Vector3f(0, -20, 65), 15, 0);
	fps = 0;
	frameSamplesCollected = 0;
	frameSampleTimeTotal = 0.0f;
    sortedItems = new RenderItem*[MAX_RENDERITEMS];
	textRenderers = new StrHashTable<TextRasterRenderer*>(13);
	spriteRenderer = new SimpleSpriteRenderer(context);
	objRenderer = new ObjRenderer(context);
	assimpRenderer = new AssimpRenderer(context);
	screenControlRenderer = new ScreenControlRenderer(context, FONT_TAG_UI);
	shadowMap = new ShadowMap(context);
	particleRenderer = new ParticleEmitterRenderer(context);
#ifdef BATTERYTECH_INCLUDE_BOX2D
	b2DebugRenderer = new B2DebugRenderer(context);
#endif
	loadingScreenDisplayed = FALSE;
	preLoad = FALSE;
	loadingTex = NULL;
	srand(time(NULL));
    rand();rand();
    pickLoadingTexture();
	loadingSize = Vector2f(512, 256);
	Property *p = context->appProperties->get("loading_width");
	if (p) {
		loadingSize.x = p->getFloatValue();
	}
	p = context->appProperties->get("loading_height");
	if (p) {
		loadingSize.y = p->getFloatValue();
	}
	p = context->appProperties->get("clear_color");
	if (p) {
		clearColor = p->getVector4fValue();
	} else {
		clearColor = Vector4f(1.0, 1.0, 1.0, 1.0);
	}
	context->renderContext->userValues->put("shadowMVP", new Matrix4f);
	context->renderContext->userValues->put("shadowLookupMatrix", new Matrix4f);
	context->renderContext->userValues->put("renderDefaults", new RenderDefaults);
}

WorldRenderer::~WorldRenderer() {
	logmsg("Releasing WorldRenderer");
	delete spriteRenderer;
	delete objRenderer;
	delete screenControlRenderer;
	delete loadingTex;
	delete assimpRenderer;
	delete particleRenderer;
	textRenderers->deleteElements();
	delete textRenderers;
#ifdef BATTERYTECH_INCLUDE_BOX2D
	delete b2DebugRenderer;
#endif
}

void WorldRenderer::pickLoadingTexture() {
	F32 r = (float)(rand()%RAND_MAX) / (float)RAND_MAX;
    char loadingKeyName[255];
    S32 i = 0;
    while (TRUE) {
    	if (i == 0) {
        	strcpy(loadingKeyName, "loading_texture");
        	loadingKeyName[15] = '\0';
    	} else {
    		sprintf(loadingKeyName, "loading_texture%d", i);
    	}
    	if (!context->appProperties->get(loadingKeyName)) {
    		break;
    	}
    	i++;
    }
    // i is already 1 past what we actually have, so no need to add one here
    S32 whichLoadingKey = (S32)(r * i) % i;
    if (whichLoadingKey == 0) {
       	strcpy(loadingKeyName, "loading_texture");
        loadingKeyName[15] = '\0';
    } else {
   		sprintf(loadingKeyName, "loading_texture%d", whichLoadingKey);
    }
    if (loadingTex != NULL) {
    	loadingTex->unload();
    	delete loadingTex;
    }
	loadingTex = new AssetTexture(context, context->appProperties->get(loadingKeyName)->getValue(), FALSE);
}


void WorldRenderer::addTextRenderer(const char *tag, TextRasterRenderer *renderer) {
	if (!textRenderers->contains(tag)) {
		textRenderers->put(tag, renderer);
	}
}

TextRasterRenderer* WorldRenderer::getTextRenderer(const char *tag) {
	return textRenderers->get(tag);
}

void WorldRenderer::setupGL() {
	logmsg("WorldRenderer::setupGL()");
	if (context->gConfig->useShaders) {
	} else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	}
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
	context->quadRenderer->init(TRUE);
}

// init won't be called until loading screen is displayed, but render is always called so get that ready there
void WorldRenderer::init(BOOL32 newContext) {
	if (newContext) {
		logmsg("WorldRenderer: New Context - Invalidating all previous GL resources");
		context->glResourceManager->invalidateGL();
	}
	if (!textRenderers->get(FONT_TAG_UI)) {
		// make the menu system's font available to the game under the tag "ui"
		textRenderers->put(FONT_TAG_UI, context->menuRenderer->getTextRenderer());
	}
	for (StrHashTable<TextRasterRenderer*>::Iterator i = textRenderers->getIterator(); i.hasNext;) {
		TextRasterRenderer *textRenderer = textRenderers->getNext(i);
		if (!strEquals(i.key, FONT_TAG_UI)) {
			// don't init the UI font renderer - that is handled by the menu renderer
			textRenderer->init(newContext);
		}
	}
	objRenderer->init(newContext);
	screenControlRenderer->init(newContext);
	assimpRenderer->init(newContext);
	particleRenderer->init(newContext);
#ifdef BATTERYTECH_INCLUDE_BOX2D
	b2DebugRenderer->init(newContext);
#endif
	logmsg("Initializing dynamic stuff");
	//btDebugRenderer->init(newContext);
	if (newContext) {
		// TODO - instead of fully unloading/reloading, keep the CPU-side stuff and just reload the bindings
		context->glResourceManager->unloadObjScenes();
	}
	context->glResourceManager->loadShaderPrograms();
	context->glResourceManager->loadTextures();
	context->glResourceManager->loadObjScenes();
	context->glResourceManager->loadAssimps();
	checkGLError("WorldRenderer Init");
	context->world->renderersReady = TRUE;
	shadowMap->init(newContext);
	Texture::textureSwitches = 0;
}

void WorldRenderer::render() {
	World *world = context->world;
	ShaderProgram::binds = 0;
	// clear the shader program cache
	ShaderProgram::currentProgram = NULL;
	checkGLError("WorldRenderer Start");
	glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
    
	if (world->gameState == GAMESTATE_LOADING) {
		if (preLoad) {
			// just forcing a two-frame wait to load
			loadingScreenDisplayed = TRUE;
			logmsg("loadingScreenDisplayed set");
		} else if (context->newGraphicsContext || !loadingTex->isLoaded()) {
			loadingScreenDisplayed = FALSE;
			ShaderProgram *quadShader = context->glResourceManager->getShaderProgram("quad");
			if (quadShader) {
				quadShader->invalidateGL();
			}
			loadingTex->invalidateGL();
			setupGL();
			logmsg("Loading Loading Texture");
			loadingTex->load(FALSE);
			preLoad = TRUE;
			logmsg("preLoad set");
		} else if (world->lastGameState != GAMESTATE_LOADING) {
			preLoad = TRUE;
			logmsg("preLoad set");
		} else {
			loadingScreenDisplayed = TRUE;
		}
	} else {
		loadingScreenDisplayed = FALSE;
		preLoad = FALSE;
	}
	// BOOL32 renderDebug = world->wfMode;
	if (world->gameState == GAMESTATE_READY || world->gameState == GAMESTATE_RUNNING) {
		// call out to lua to receive the list of rendering instructions.
		world->renderItemsUsed = 0;
		if (!context->game->isInError) {
			if (world->gameState == GAMESTATE_READY) {
				context->game->luaBinder->render(0);
			} else {
				context->game->luaBinder->render(1);
			}
		}
		// we should know or camera position now
        context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
        world->camera->update();
        context->renderContext->projMatrix = world->camera->proj;
        context->renderContext->mvMatrix = world->camera->matrix;
	   	checkGLError("WorldRenderer After Camera Update");
        BOOL32 has3DObjects = FALSE;
		for (S32 i = 0; i < world->renderItemsUsed; i++) {
			RenderItem *item = &world->renderItems[i];
			if (item->renderType == RenderItem::RENDERTYPE_ASSIMP) {
				has3DObjects = TRUE;
				break;
			}
		}
        // first thing we should do is deal with any FBO rendering
		if (context->gConfig->shadowType != GraphicsConfiguration::SHADOWTYPE_NONE && has3DObjects) {
            shadowMap->bindForMapCreation();
    		for (S32 i = 0; i < world->renderItemsUsed; i++) {
    			RenderItem *item = &world->renderItems[i];
     			if (item->renderType == RenderItem::RENDERTYPE_ASSIMP) {
    				assimpRenderer->renderShadow(item);
    			}
    		}
    		shadowMap->unbindAfterMapCreation();
    		// shadowmap resets our clear color
    		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		}
		// FBO stuff done, resume normal rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   	checkGLError("WorldRenderer After Clear");
		BOOL32 has2DBG = FALSE;
		for (S32 i = 0; i < world->renderItemsUsed; i++) {
			RenderItem *item = &world->renderItems[i];
			if (item->renderType == RenderItem::RENDERTYPE_2DBG) {
				has2DBG = TRUE;
			}
		}
		if (has2DBG) {
			if (has3DObjects) {
			    glFrontFace(GL_CW);
			}
			// just for 2D Backgrounds behind the 3D
			BOOL32 blendEnabled = FALSE;
			//glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			context->renderContext->projMatrix.identity();
			context->renderContext->mvMatrix.identity();
			context->renderContext->projMatrix.ortho(0, gConfig->width, gConfig->height, 0, -1, 1);
			context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
			// any 2D background renderers here
			spriteRenderer->startBatch();
			for (S32 i = 0; i < world->renderItemsUsed; i++) {
				RenderItem *item = &world->renderItems[i];
				if (item->renderType == RenderItem::RENDERTYPE_2DBG) {
					// use the item's opaque flag to determine if we should blend or not
					BOOL32 needsBlend = ((item->flags & RENDERITEM_FLAG_IS_OPAQUE) == 0);
					if (needsBlend != blendEnabled) {
						if (blendEnabled) {
							glDisable(GL_BLEND);
						} else {
							glEnable(GL_BLEND);
						}
						blendEnabled = !blendEnabled;
					}
					spriteRenderer->render(item);
				}
			}
			spriteRenderer->endBatch();
			if (has3DObjects) {
		        context->renderContext->projMatrix = world->camera->proj;
		        context->renderContext->mvMatrix = world->camera->matrix;
		        if (blendEnabled) {
		        	glDisable(GL_BLEND);
		        }
			}
		}
		context->videoManager->render();
	   	checkGLError("WorldRenderer After 2DBG");
		// now 3D
		if (context->gConfig->shadowType != GraphicsConfiguration::SHADOWTYPE_NONE && has3DObjects) {
			shadowMap->bindForSceneRender();
		}
		render3D();
        if (context->gConfig->shadowType != GraphicsConfiguration::SHADOWTYPE_NONE && has3DObjects) {
        	shadowMap->unbindAfterSceneRender();
        }
		checkGLError("WorldRenderer After 3D");
		render2D();
	} else if (world->gameState == GAMESTATE_LOADING) {
		renderLoadingScreen();
	}
//	char buf[50];
//	sprintf(buf, "binds = %d", ShaderProgram::binds);
//	logmsg(buf);
    ShaderProgram::binds = 0;

	Texture::textureSwitches = 0;
	checkGLError("WorldRenderer End");
	if (world->gameState != GAMESTATE_LOADING) {
		context->menuRenderer->render();
	}
}

void WorldRenderer::renderLoadingScreen() {
	logmsg("Rendering Loading Screen");
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	// glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	context->renderContext->projMatrix.identity();
	context->renderContext->mvMatrix.identity();
	context->renderContext->projMatrix.ortho(0, gConfig->width, gConfig->height, 0, -1, 1);
	context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
	//char buf[255];
	//sprintf(buf, "Loading textureID = %d, gConfig dimensions=%d, %d", loadingTex->textureId, gConfig->width, gConfig->height);
	//logmsg(buf);
	//render loading image centered scaled up from 512x256
	// default scale mode is uiScale
	F32 loadWidth = this->loadingSize.x * gConfig->uiScale;
	F32 loadHeight = this->loadingSize.y * gConfig->uiScale;
	Property *prop = context->appProperties->get("loading_scale_mode");
	if (prop) {
		if (strEquals(prop->getValue(), "scaleAspectX")) {
			loadWidth = this->loadingSize.x * gConfig->scaleX2d;
			loadHeight = this->loadingSize.y * gConfig->scaleX2d;
		} else if (strEquals(prop->getValue(), "scaleAspectY")) {
			loadWidth = this->loadingSize.x * gConfig->scaleY2d;
			loadHeight = this->loadingSize.y * gConfig->scaleY2d;
		} else if (strEquals(prop->getValue(), "scaleXY")) {
			loadWidth = this->loadingSize.x * gConfig->scaleX2d;
			loadHeight = this->loadingSize.y * gConfig->scaleY2d;
		}
	}
	context->quadRenderer->render(loadingTex, gConfig->height/2 - loadHeight/2, gConfig->width/2 + loadWidth/2, gConfig->height/2 + loadHeight/2, gConfig->width/2 - loadWidth/2);
   	checkGLError("WorldRenderer After loading render");
}

void WorldRenderer::render3D() {
    // Note - draw order here is decent enough for most applications, however we need to try to include the particle clouds in the sort to fix some of the
    // rendering bugs with them and other transparent objects
	World *world = context->world;
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    U32 count = sortRenderItemsOfType(RenderItem::RENDERTYPE_ASSIMP | RenderItem::RENDERTYPE_BB);
   	checkGLError("WorldRenderer Before Opaque 3D");
    // opaque are ordered from closest to furthest
	for (U32 i = 0; i < count; i++) {
		RenderItem *item = sortedItems[i];
        if (item->renderType == RenderItem::RENDERTYPE_ASSIMP) {
            if (item->viewport.z != 0 && item->viewport.w != 0) {
                // use alternate viewport, update camera
                glViewport(item->viewport.x, item->viewport.y, item->viewport.z, item->viewport.w);
                Matrix4f projMatrix;
                projMatrix.perspective(60, (item->viewport.z) / (item->viewport.w), 2.0f, 5000.0f);
                context->renderContext->projMatrix = projMatrix;
            }
            assimpRenderer->render(item, FALSE);
            if (item->viewport.z != 0 && item->viewport.w != 0) {
                // now reset
                glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
                context->renderContext->projMatrix = world->camera->proj;
            }
        }
	}
   	checkGLError("WorldRenderer After Opaque 3D");
  	glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    // shut off depth writes for particle renderer
    glDepthMask(FALSE);
    particleRenderer->render(FALSE);
    glFrontFace(GL_CCW);
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.1f);
    // do alpha pass in reverse from furthest to closest
    BOOL32 cullingEnabled = FALSE;
    BOOL32 inQuadBatch = FALSE;
	for (S32 i = count-1; i >= 0; i--) {
		RenderItem *item = sortedItems[i];
        if (item->flags & RENDERITEM_FLAG_TWO_SIDED) {
            if (cullingEnabled) {
                cullingEnabled = FALSE;
                glDisable(GL_CULL_FACE);
            }
        } else {
            if (!cullingEnabled) {
                cullingEnabled = TRUE;
                glEnable(GL_CULL_FACE);
            }
        }
        if (item->renderType == RenderItem::RENDERTYPE_ASSIMP) {
            if (inQuadBatch) {
                inQuadBatch = FALSE;
                spriteRenderer->endBatch();
            }
             if (item->viewport.z != 0 && item->viewport.w != 0) {
                // use alternate viewport, update camera
                glViewport(item->viewport.x, item->viewport.y, item->viewport.z, item->viewport.w);
                Matrix4f projMatrix;
                projMatrix.perspective(60, (item->viewport.z) / (item->viewport.w), 2.0f, 5000.0f);
                context->renderContext->projMatrix = projMatrix;
            }
            // render parts with transparency last
            assimpRenderer->render(item, TRUE);
            if (item->viewport.z != 0 && item->viewport.w != 0) {
                // now reset
                glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
                context->renderContext->projMatrix = world->camera->proj;
            }
        } else if (item->renderType == RenderItem::RENDERTYPE_BB) {
            if (!inQuadBatch) {
                inQuadBatch = TRUE;
                spriteRenderer->startBatch();
            }
            spriteRenderer->render(item);
        }
        context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
	}
    if (inQuadBatch) {
        spriteRenderer->endBatch();
    }
    assimpRenderer->unbind();
    // go back to default
    glDepthMask(TRUE);
   	checkGLError("WorldRenderer After Transparent 3D");
}

void WorldRenderer::render2D() {
	// calc FPS
	frameSampleTimeTotal += context->tickDelta;
	frameSamplesCollected++;
	if (frameSamplesCollected == 10) {
		if (frameSampleTimeTotal == 0) {
			fps = 0;
		} else {
			fps = 10 / frameSampleTimeTotal;
		}
		frameSamplesCollected = 0;
		frameSampleTimeTotal = 0;
	}
	World *world = context->world;
	//glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	context->renderContext->projMatrix.identity();
	context->renderContext->mvMatrix.identity();
	context->renderContext->projMatrix.ortho(0, gConfig->width, gConfig->height, 0, -1, 1);
	context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
    glFrontFace(GL_CW);
	// now 2D in front of the 3D
    TextRasterRenderer *curTextRenderer = NULL;
    char curFontTag[255];
    curFontTag[0] = '\0';
    spriteRenderer->startBatch();
	for (S32 i = 0; i < world->renderItemsUsed; i++) {
		RenderItem *item = &world->renderItems[i];
		if (item->renderType == RenderItem::RENDERTYPE_2D) {
			if (curTextRenderer) {
				curTextRenderer->finishText();
				curTextRenderer = NULL;
				curFontTag[0] = '\0';
                spriteRenderer->startBatch();
			}
			context->renderContext->colorFilter = Vector4f(1,1,1,1);
			if (strEquals(item->textureName, "shadowmap")) {
				item->textureName[0] = '\0';
                spriteRenderer->endBatch();
                spriteRenderer->startBatch();
				shadowMap->bindAsTexture();
			}
			spriteRenderer->render(item);
		} else if (item->renderType == RenderItem::RENDERTYPE_TEXT2D) {
			if (!curFontTag[0]) {
                spriteRenderer->endBatch();
				strcpy(curFontTag, item->attr2);
				curTextRenderer = textRenderers->get(curFontTag);
				if (!curTextRenderer) {
					char buf[255];
					sprintf(buf, "Render Error - Font Tag \"%s\" not found", curFontTag);
					logmsg(buf);
					strcpy(curFontTag, FONT_TAG_UI);
					curTextRenderer = textRenderers->get(curFontTag);
				}
				curTextRenderer->startText();
			} else {
				if (!strEquals(curFontTag, item->attr2) && curTextRenderer) {
					curTextRenderer->finishText();
					strcpy(curFontTag, item->attr2);
					curTextRenderer = textRenderers->get(curFontTag);
					if (!curTextRenderer) {
						char buf[255];
						sprintf(buf, "Render Error - Font Tag \"%s\" not found", curFontTag);
						logmsg(buf);
						strcpy(curFontTag, FONT_TAG_UI);
						curTextRenderer = textRenderers->get(curFontTag);
					}
					curTextRenderer->startText();
				}
			}
			if (curTextRenderer) {
                BOOL32 isMultiline = item->flags & RENDERITEM_FLAG_MULTILINE_TEXT;
                F32 posX = item->pos.x;
                F32 posY = item->pos.y;
                F32 scale = item->scale.x;
				if (item->alignment == RenderItem::ALIGN_LEFT) {
                    // noop
				} else if (item->alignment == RenderItem::ALIGN_CENTER) {
					S32 width = curTextRenderer->measureWidth(item->attr1, scale);
                    posX = item->pos.x - width/2;
				}  else if (item->alignment == RenderItem::ALIGN_RIGHT) {
					S32 width = curTextRenderer->measureWidth(item->attr1, scale);
                    posX = item->pos.x - width;
				}
                if (isMultiline) {
                	// scale y and scale z are the max width and height
                    curTextRenderer->renderMultiline(item->attr1, posX, posY, posX+item->scale.y, posY+item->scale.z, scale);
                } else {
                    curTextRenderer->render(item->attr1, posX, posY, scale);
                }
			}
		} else if (item->renderType == RenderItem::RENDERTYPE_2DPROJ) {
			if (curTextRenderer) {
				curTextRenderer->finishText();
				curTextRenderer = NULL;
			}
		    spriteRenderer->endBatch();
		    Matrix4f *m = &item->mat;
            context->renderContext->projMatrix.identity();
            context->renderContext->projMatrix.ortho(m->data[0], m->data[1], m->data[2], m->data[3], m->data[4], m->data[5]);
        }
	}
	if (curTextRenderer) {
		curTextRenderer->finishText();
		curTextRenderer = NULL;
	}
    spriteRenderer->endBatch();
    particleRenderer->render(TRUE);
    // end on CW front face because 2D particles switch it to CCW
    glFrontFace(GL_CW);
#ifdef BATTERYTECH_INCLUDE_BOX2D
    if (world->physicsDrawDebug) {
        if (world->physicsDrawDebugUsingProjection) {
            F32 *p = world->physicsDrawDebugProjection;
            context->renderContext->projMatrix.identity();
            context->renderContext->projMatrix.ortho(p[0], p[1], p[2], p[3], p[4], p[5]);
        }
    	b2DebugRenderer->render(world);
    }
#endif
	if (context->showFPS) {
        context->renderContext->projMatrix.identity();
        context->renderContext->projMatrix.ortho(0, gConfig->width, gConfig->height, 0, -1, 1);
		char fpsText[50];
		sprintf(fpsText, "FPS: %d", fps);
		TextRasterRenderer *textRenderer = textRenderers->get(FONT_TAG_UI);
		if (textRenderer) {
			textRenderer->startText();
			textRenderer->render(fpsText, 5, context->gConfig->height - 5);
			textRenderer->finishText();
		}
	}
	context->renderContext->colorFilter = Vector4f(1,1,1,1);
	screenControlRenderer->render();
   	checkGLError("WorldRenderer After screenControlRender");
}

// sorts renderitems front-to-back
static int renderitem_sort_function(const void *a, const void *b) {
    const RenderItem *ra = *(const RenderItem**)a;
    const RenderItem *rb = *(const RenderItem**)b;
    if (ra->sortValue > rb->sortValue) {
        return 1;
    }
    return -1;
}

U32 WorldRenderer::sortRenderItemsOfType(U32 renderTypeMask) {
    World *world = context->world;
    U32 sortIdx = 0;
    Vector3f camPos = world->camera->pos;
    F32 nextFirstVal = 0.0f;
    for (S32 i = 0; i < world->renderItemsUsed; i++) {
		RenderItem *item = &world->renderItems[i];
        if (item->renderType & renderTypeMask) {
            // we preserve the order of the first-to-draw items by keeping them as close to 0 distance as possible with small increments
            if (item->flags & RENDERITEM_FLAG_DRAW_FIRST) {
                item->sortValue = nextFirstVal;
                nextFirstVal += 0.0001f;
            } else {
                // this will work most of the time - a more expensive but more accurate method would be to use the translated AABB center
                item->sortValue = (camPos - item->getTranslation()).lengthSq();
            }
            sortedItems[sortIdx] = item;
            sortIdx++;
        }
    }
    qsort(sortedItems, sortIdx, sizeof(RenderItem*), renderitem_sort_function);
    return sortIdx;
}

