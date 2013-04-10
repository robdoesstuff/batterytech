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

#include "ShadowMap.h"
#include "../GameContext.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/Logger.h>
#include "../World.h"
#include "Camera.h"
#include "GlobalLight.h"
#include <batterytech/render/Renderer.h>
#include <batterytech/render/Texture.h>
#include <stdio.h>
#include <batterytech/util/strx.h>

#define SHADOWMAP_WIDTH    512
#define SHADOWMAP_HEIGHT   512

#define SHADOWMAP_HQ_WIDTH    1024
#define SHADOWMAP_HQ_HEIGHT   1024

// bias moves from unit cube [-1,1] to [0,1]
const Matrix4f biasMatrix(
	        0.5, 0.0, 0.0, 0.0,
	        0.0, 0.5, 0.0, 0.0,
	        0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);

const Matrix4f biasMatrix2(
	        0.5, 0.0, 0.0, 0.5,
	        0.0, 0.5, 0.0, 0.5,
	        0.0, 0.0, 0.5, 0.5,
			0.0, 0.0, 0.0, 1.0);

ShadowMap::ShadowMap(GameContext *context) {
	this->context = context;
    shadowFrameBuffer = 0;
    defaultFrameBuffer = 0;
   	renderBuffer = 0;
    shadowTexture = 0;
    hasDepthTexture = FALSE;
	shadowTextureType = SHADOWTEXTURE_NONE;
}

ShadowMap::~ShadowMap() {

}

void ShadowMap::init(BOOL32 newContext) {
	if (newContext) {
		shadowTexture = 0;
		renderBuffer = 0;
		shadowFrameBuffer = 0;
		generateShadowFBO();    
	}
}

void ShadowMap::generateShadowFBO() {
	Renderer::checkGLError("ShadowMap start generateShadowFBO()");
	ShadowMap::ShadowTextureType *textureType = (ShadowMap::ShadowTextureType*) context->renderContext->userValues->get("shadowmap_texture_type");
	if (!textureType) {
		textureType = new ShadowMap::ShadowTextureType;
		*textureType = SHADOWTEXTURE_NONE;
		context->renderContext->userValues->put("shadowmap_texture_type", textureType);
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP) {
		shadowWidth = SHADOWMAP_WIDTH;
		shadowHeight = SHADOWMAP_HEIGHT;
	} else if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_HQ) {
		shadowWidth = SHADOWMAP_HQ_WIDTH;
		shadowHeight = SHADOWMAP_HQ_HEIGHT;
	} else if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_CUSTOM) {
		Vector2f *size = (Vector2f*)context->renderContext->userValues->get("shadowmap_size");
		if (size) {
			shadowWidth = (GLuint)size->x;
			shadowHeight = (GLuint)size->y;
		} else {
			shadowWidth = SHADOWMAP_WIDTH;
			shadowHeight = SHADOWMAP_HEIGHT;
		}
	} else {
		// no shadowmap
		shadowWidth = 0;
		shadowHeight = 0;
		((Matrix4f*)context->renderContext->userValues->get("shadowMVP"))->identity();
		((Matrix4f*)context->renderContext->userValues->get("shadowLookupMatrix"))->identity();
	}
	currentShadowType = context->gConfig->shadowType;
	if (!context->gConfig->supportsFBOs) {
		if (shadowWidth || shadowHeight) {
			logmsg("ShadowMap requires FBOs which are not supported on this GPU/Driver.");
		}
		return;
	}
	if (shadowTexture) {
		glDeleteTextures(1, &shadowTexture);
		shadowTexture = 0;
	}
	if (renderBuffer) {
		glDeleteRenderbuffers(1, &renderBuffer);
		renderBuffer = 0;
	}
	if (shadowFrameBuffer) {
		glDeleteFramebuffers(1, &shadowFrameBuffer);
		shadowFrameBuffer = 0;
	}
	if (!shadowWidth || !shadowHeight) {
		return;
	}

    // get the current framebuffer and set as default
    GLint fboID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fboID);
    defaultFrameBuffer = (GLuint)fboID;
	// Create the texture we'll use to store our depth values
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Remove artifact on the edges of the shadowmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
	// create a framebuffer object
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	BOOL32 tryAlpha = FALSE;
	const char *renderer = (const char*)glGetString(GL_RENDERER);
	// little hack to use an 8 bit alpha channel shadowmap on Tegra 3 - is 25% faster and doesn't look too different
	if (strEquals(renderer, "NVIDIA Tegra 3")) {
		tryAlpha = TRUE;
	}
	BOOL32 done = FALSE;
	while (!done) {
	    if (context->gConfig->supportsDepthTextures) {
	    	logmsg("Shadowmap: using depth texture");
	    	*textureType = SHADOWTEXTURE_DEPTH;
	#ifdef OPENGL
	        //desktop GL requires this for a depth texture, ES does not
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
	#endif
			glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
			// attach the texture to FBO depth attachment point
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
			done = TRUE;
	    } else {
	    	if (tryAlpha) {
		    	logmsg("Shadowmap: using alpha texture");
		    	*textureType = SHADOWTEXTURE_ALPHA;
		        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, shadowWidth, shadowHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
	    	} else {
		    	logmsg("Shadowmap: using rgb texture");
		    	*textureType = SHADOWTEXTURE_RGB;
		        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, shadowWidth, shadowHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	    	}
	        Texture::lastTextureId = shadowTexture;
	        Renderer::checkGLError("ShadowMap create shadow texture");

	        // Clear previous render buffer attempt
	    	if (renderBuffer) {
	    		glDeleteRenderbuffers(1, &renderBuffer);
	    		renderBuffer = 0;
	    	}
	        // Create a render buffer
	        glGenRenderbuffers(1, &renderBuffer);
	        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	        // Allocate 16-bit depth buffer
	        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, shadowWidth, shadowHeight);
	        // clear bound renderbuffer
	        glBindRenderbuffer(GL_RENDERBUFFER, 0);
	        Renderer::checkGLError("ShadowMap create renderbuffer");

	        // Attach the render buffer as depth buffer
	        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	        Renderer::checkGLError("ShadowMap attach depth renderbuffer to framebuffer");
	        // Attach the texture to FBO color attachment point
	        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowTexture, 0);
	        Renderer::checkGLError("ShadowMap attach texture to framebuffer");
	    }
		// check FBO status
		GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (FBOstatus != GL_FRAMEBUFFER_COMPLETE) {
			if (done || !tryAlpha) {
				logmsg("ShadowMap ERROR - GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO!!");
				done = TRUE;
			} else {
				logmsg("ShadowMap falling back to safe mode (RGB)");
				if (tryAlpha) {
					tryAlpha = FALSE;
				}
			}
		} else {
			done = TRUE;
		}
	}

    // clear bound texture
	glBindTexture(GL_TEXTURE_2D, 0);
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	Renderer::checkGLError("ShadowMap end generateShadowFBO()");
}

void ShadowMap::bindForMapCreation() {
	if (currentShadowType != context->gConfig->shadowType || context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_CUSTOM) {
		if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_CUSTOM) {
			Vector2f *size = (Vector2f*)context->renderContext->userValues->get("shadowmap_size");
			if (!size) {
				size = new Vector2f(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
				context->renderContext->userValues->put("shadowmap_size", size);
			}
			if (shadowWidth != (GLuint)size->x || shadowHeight != (GLuint)size->y) {
				generateShadowFBO();
			}
		} else {
			generateShadowFBO();
		}
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
	if (!context->gConfig->supportsFBOs) {
		return;
	}
	ShadowMap::ShadowTextureType *textureType = (ShadowMap::ShadowTextureType*) context->renderContext->userValues->get("shadowmap_texture_type");
	Renderer::checkGLError("ShadowMap start bindForMapCreation()");
	//First step: Render from the light POV to a FBO, store depth values only
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);	//Rendering offscreen
	// Viewport is now the size of the shadowmap texture
	glViewport(0,0,shadowWidth,shadowHeight);
    if (*textureType == SHADOWTEXTURE_DEPTH) {
        //only if using depth texture!
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glClear(GL_DEPTH_BUFFER_BIT);
   } else {
        // Clear previous frame values
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // we clear depth at unbind for driver optimization
        glClear(GL_COLOR_BUFFER_BIT);
    }
    // this is setting up a projection and mv for the light's view
	Matrix4f proj;
	Matrix4f mv;
	Vector3f lightOrigin = context->world->globalLight->origin;
	Vector2f nearFar = context->world->globalLight->shadowFrustumNearFar;
	Vector3f lookDir = context->world->globalLight->direction * -1.0f;
	// app specifies near and far Z
    if (context->world->globalLight->shadowUsePerspective) {
    	// TODO - currently not working right.  Ortho works fine.
        proj.perspective(context->world->globalLight->shadowPerspectiveFOV,(float)shadowWidth/(float)shadowHeight, nearFar.x, nearFar.y);
    } else {
        Vector4f orthoSize = context->world->globalLight->shadowOrthoSize;
        proj.ortho(orthoSize.x, orthoSize.y, orthoSize.z, orthoSize.w, nearFar.x, nearFar.y);
    }
    lookDir.normalize();
    // lookAt is a point extended from lightOrigin that is the scene center
	Vector3f lookAt = (lightOrigin + lookDir);
	// default up
    Vector3f up = Vector3f(0,0,1);
	if (lookDir.z == 1.0f || lookDir.z == -1.0f) {
	   	// if light is directly up or down, set up to right/left
		up = Vector3f(0,1,0);
	}
	// The matrix will apply rotation making Z the direct line of site distance from world origin to light origin
	mv.lookAt(lightOrigin, lookAt, up);

	// we'll be using the ec position of the vert to lookup the shadowmap uv,
	// so we'll want the inverse camera on there to remove the view from it
	// so that we only get world position of the vertex
	Matrix4f invCam = context->world->camera->matrix.inverse();
	Matrix4f shadowMvp = proj * mv;
	Matrix4f shadowLookup = biasMatrix * shadowMvp * invCam;
	/*
	Vector4f testPt = shadowLookup * Vector4f(0,0,0,1);
	char buf[1024];
	sprintf(buf, "test point = %f %f %f %f", testPt.x, testPt.y, testPt.z, testPt.w );
	logmsg(buf);
	*/

	*(Matrix4f*)context->renderContext->userValues->get("shadowMVP") = shadowMvp;
	*(Matrix4f*)context->renderContext->userValues->get("shadowLookupMatrix") = shadowLookup;
	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	S32 cullMode = context->world->globalLight->shadowCullMode;
	if (cullMode == 1) {
		// cull front
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);
	} else if (cullMode == 2) {
		// cull back
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	} else {
		// cull none
		glDisable(GL_CULL_FACE);
	}
	glEnable(GL_DEPTH_TEST);
	Renderer::checkGLError("ShadowMap end bindForMapCreation()");
}

void ShadowMap::unbindAfterMapCreation() {
	if (!context->gConfig->supportsFBOs) {
		return;
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
	// Back to default viewport
	glViewport(0,0,context->gConfig->viewportWidth,context->gConfig->viewportHeight);
	// Now rendering from the camera POV, using the FBO to generate shadows
	ShadowMap::ShadowTextureType *textureType = (ShadowMap::ShadowTextureType*) context->renderContext->userValues->get("shadowmap_texture_type");
	if (*textureType != SHADOWTEXTURE_DEPTH) {
		// we can clear the depth buffer here but not if using a depth texture
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	// clear the depth buffer because we don't need it to resolve and the driver can optimize here when switching back to FB0
	glBindFramebuffer(GL_FRAMEBUFFER,defaultFrameBuffer);
    // re-enable normal backface culling
	glCullFace(GL_BACK);
    if (*textureType == SHADOWTEXTURE_DEPTH) {
        //only if using depth texture!
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    Renderer::checkGLError("ShadowMap unbindAfterMapCreation()");
}

void ShadowMap::bindForSceneRender() {
	if (!context->gConfig->supportsFBOs) {
		return;
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
	Renderer::checkGLError("ShadowMap start bindForSceneRender()");
	// bind our shadowmap to texture 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    // now switch back to normal diffuse texture 0
    glActiveTexture(GL_TEXTURE0);
	Renderer::checkGLError("ShadowMap end bindForSceneRender()");
}

void ShadowMap::unbindAfterSceneRender() {
	if (!context->gConfig->supportsFBOs) {
		return;
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    Renderer::checkGLError("ShadowMap unbindAfterSceneRender()");
}

void ShadowMap::bindAsTexture() {
	if (!context->gConfig->supportsFBOs) {
		return;
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,shadowTexture);
    Texture::lastTextureId = shadowTexture;
}
