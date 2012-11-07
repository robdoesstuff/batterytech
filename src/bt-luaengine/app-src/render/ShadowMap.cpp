/*
 * ShadowMap.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: rgreen
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

ShadowMap::ShadowMap(GameContext *context) {
	this->context = context;
    shadowFrameBuffer = 0;
    defaultFrameBuffer = 0;
   	renderBuffer = 0;
    shadowTexture = 0;
    hasDepthTexture = FALSE;
}

ShadowMap::~ShadowMap() {

}

void ShadowMap::init(BOOL32 newContext) {
	if (newContext) {
		shadowTexture = 0;
		renderBuffer = 0;
		shadowFrameBuffer = 0;
		logmsg("Initializing ShadowMap");
		generateShadowFBO();    
	}
}

void ShadowMap::generateShadowFBO() {
	Renderer::checkGLError("ShadowMap start generateShadowFBO()");
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP) {
		shadowWidth = SHADOWMAP_WIDTH;
		shadowHeight = SHADOWMAP_HEIGHT;
	} else if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_HQ) {
		shadowWidth = SHADOWMAP_HQ_WIDTH;
		shadowHeight = SHADOWMAP_HQ_HEIGHT;
	} else {
		// no shadowmap
		shadowWidth = 0;
		shadowHeight = 0;
		((Matrix4f*)context->renderContext->userValues->get("shadowMVP"))->identity();
		((Matrix4f*)context->renderContext->userValues->get("shadowLookupMatrix"))->identity();
	}
	currentShadowType = context->gConfig->shadowType;
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
	// We'll use an RGBA texture to store the depths in the shadow map
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadowMapWidth, shadowMapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, shadowWidth, shadowHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	// clear bound texture
	// glBindTexture(GL_TEXTURE_2D, 0);
	Texture::lastTextureId = shadowTexture;
	Renderer::checkGLError("ShadowMap create shadow texture");

 	// Create a render buffer
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
     // Allocate 16-bit depth buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, shadowWidth, shadowHeight);
    // clear bound renderbuffer 
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
	Renderer::checkGLError("ShadowMap create renderbuffer");

	// create a framebuffer object
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
    // Attach the render buffer as depth buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	Renderer::checkGLError("ShadowMap attach depth renderbuffer to framebuffer");
    // Attach the RGBA texture to FBO color attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowTexture, 0);
	Renderer::checkGLError("ShadowMap attach texture to framebuffer");
	// check FBO status
	GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE) {
		logmsg("ShadowMap ERROR - GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO!!");
	}
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	Renderer::checkGLError("ShadowMap end generateShadowFBO()");
}

void ShadowMap::bindForMapCreation() {
	if (currentShadowType != context->gConfig->shadowType) {
		generateShadowFBO();
	}
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
	Renderer::checkGLError("ShadowMap start bindForMapCreation()");
	//First step: Render from the light POV to a FBO, store depth values only
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);	//Rendering offscreen
	// Viewport is now the size of the shadowmap texture
	glViewport(0,0,shadowWidth,shadowHeight);
	// Clear previous frame values
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// depth buffer is cleared before binding back to main fb
	glClear(GL_COLOR_BUFFER_BIT);
	//Disable color rendering, we only want to write to the Z-Buffer
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // this is setting up a projection and mv for the light's view
	Matrix4f proj;
	Matrix4f mv;
	Vector3f lightOrigin = context->world->globalLight->origin;
	Vector2f nearFar = context->world->globalLight->shadowFrustumNearFar;
	Vector3f lookDir = context->world->globalLight->direction * -1.0f;
	// app specifies near and far Z
    if (context->world->globalLight->shadowUsePerspective) {
        proj.perspective(context->world->globalLight->shadowPerspectiveFOV,(float)shadowWidth/(float)shadowHeight, nearFar.x, nearFar.y);
    } else {
        Vector4f orthoSize = context->world->globalLight->shadowOrthoSize;
        proj.ortho(orthoSize.x, orthoSize.y, orthoSize.z, orthoSize.w, nearFar.x, nearFar.y);
    }
	Vector3f lookAt = (lightOrigin + lookDir*30);
	Vector3f up(0,0,1);
    if (context->world->globalLight->direction.y > 0) {
        up = Vector3f(0,0,-1);
    }
    F32 zDir = context->world->globalLight->direction.z;
	if (zDir == 1.0f || zDir == -1.0f) {
		up = Vector3f(0,1,0);
	}
	// recalc up to be orthogonal
	Vector3f right = lookDir.cross(up);
	up = right.cross(lookDir);
	up.normalize();
	mv.lookAt(lightOrigin.x, lightOrigin.y, lightOrigin.z ,lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z);
    //char buf[1024];
    //sprintf(buf, "%f %f %f, %f %f %f, %f %f %f, %f %f %f", mv.data[0], mv.data[1], mv.data[2], mv.data[4], mv.data[5], mv.data[6], mv.data[8], mv.data[9], mv.data[10], mv.data[12], mv.data[13], mv.data[14]);
    //logmsg(buf);
	// char buf[255];
	// sprintf(buf, "oxyz=%f %f %f ... lxyz=%f %f %f ... upxyz=%f %f %f", lightOrigin.x, lightOrigin.y, lightOrigin.z, lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z);
	// sprintf(buf, "deltaY=%f, deltaZ=%f", lightOrigin.y-lookAt.y, lightOrigin.z-lookAt.z);
	// sprintf(buf, "mv xyz=%f %f %f", mv.data[13], mv.data[14], mv.data[15]);
	// logmsg(buf);
	// we'll be using the ec position of the vert to lookup the shadowmap uv,
	// so we'll want the inverse camera on there to remove the view from it
	// so that we only get world position of the vertex
	Matrix4f invCam = context->world->camera->matrix.inverse();
	Matrix4f shadowMvp = proj * mv;
	*(Matrix4f*)context->renderContext->userValues->get("shadowMVP") = shadowMvp;
	*(Matrix4f*)context->renderContext->userValues->get("shadowLookupMatrix") = biasMatrix * shadowMvp * invCam;
	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	Renderer::checkGLError("ShadowMap end bindForMapCreation()");
}

void ShadowMap::unbindAfterMapCreation() {
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
	// Back to default viewport
	glViewport(0,0,context->gConfig->viewportWidth,context->gConfig->viewportHeight);
	// Now rendering from the camera POV, using the FBO to generate shadows
	// clear the depth buffer because we don't need it to resolve and the driver can optimize here when switching back to FB0
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER,defaultFrameBuffer);
    // re-enable normal backface culling
	glCullFace(GL_BACK);
    Renderer::checkGLError("ShadowMap unbindAfterMapCreation()");
}

void ShadowMap::bindForSceneRender() {
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
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    Renderer::checkGLError("ShadowMap unbindAfterSceneRender()");
}

void ShadowMap::bindAsTexture() {
	if (context->gConfig->shadowType == GraphicsConfiguration::SHADOWTYPE_NONE) {
		return;
	}
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,shadowTexture);
    Texture::lastTextureId = shadowTexture;
}
