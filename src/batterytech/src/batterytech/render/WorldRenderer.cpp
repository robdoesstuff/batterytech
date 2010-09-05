/*
 * WorldRenderer.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#include "WorldRenderer.h"
#include "TextRasterRenderer.h"
#include "../decoders/stb_image.h"
#include "../platform/platformgl.h"
#include "../platform/platformgeneral.h"
#include "../logger.h"

void loadTexture();
GLuint textureId;
F32 theta = 0.0f;
S32 screenWidth = 0;
S32 screenHeight = 0;


WorldRenderer::WorldRenderer() {
}

void setScreenSize(S32 width, S32 height) {
	screenWidth = width;
	screenHeight = height;
}

void WorldRenderer::init(S32 width, S32 height) {
	screenWidth = width;
	screenHeight = height;
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	textRenderer = new TextRasterRenderer("lcdmn.ttf", 12.0f);
	textRenderer->init(width, height);
	loadTexture();
}

void WorldRenderer::unloadLevel() {}
void WorldRenderer::loadLevel() {}

void WorldRenderer::render(World *world) {

	theta += world->tickDelta * 60;

	// OpenGL animation code goes here
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glViewport(0,0,screenWidth,screenHeight);
	glLoadIdentity();
	glOrthof(0, screenWidth, screenHeight, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glColor4f(1, 1, 1, 1);

	float colors[] = {
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f
	};
	float verts[] = {
			0.0f, 1.0f, 0.0f,
			0.87f, -0.5f, 0.0f,
			-0.87f, -0.5f, 0.0f,
	};
	float uvs[] = {
			0.0f, 1.0f,
			1.0f, 0.5f,
			0.0f, 0.0f
	};
	glFrontFace(GL_CCW);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glColorPointer(4, GL_FLOAT, 0, &colors);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	if (world->down1) {
		glPushMatrix();
		glTranslatef(world->x1, world->y1, 0);
		glRotatef( theta, 0.0f, 0.0f, 1.0f );
		glScalef(100, 100, 1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glPopMatrix();
	}
	if (world->down2) {
		glPushMatrix();
		glTranslatef(world->x2, world->y2, 0);
		glRotatef( theta, 0.0f, 0.0f, 1.0f );
		glScalef(100, 100, 1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glPopMatrix();
	}
	textRenderer->render(world);
}

WorldRenderer::~WorldRenderer() {
}

void loadTexture() {
	log("Loading texture");
	int x,y,n;
	int assetSize = 0;
	unsigned char *fileData = _platform_load_asset("text_bg_tex.jpg", &assetSize);
	if (fileData) {
		char buf[50];
		sprintf(buf, "Loaded %i bytes of raw image data", assetSize);
		log(buf);
	}
	unsigned char *data = stbi_load_from_memory(fileData, assetSize, &x, &y, &n, 0);
	//unsigned char *data = stbi_load("assets\\text_bg_tex.jpg", &x, &y, &n, 0);
	if (data) {
		int bytes = x * y * n * sizeof(unsigned char);
		char buf[50];
		sprintf(buf, "Bitmap is %ix%i color components=%i bytes=%i",x,y,n, bytes);
		log(buf);
		GLuint textureIds[1];
		glGenTextures(1, textureIds);
		textureId = textureIds[0];
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		GLenum error = glGetError();
		if (error) {
			log("GL Error");
		}
		stbi_image_free(data);
	}
	_platform_free_asset(fileData);
}
