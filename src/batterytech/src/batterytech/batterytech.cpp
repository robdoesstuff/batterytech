//============================================================================
// Name        : batterytech.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include "batterytech.h"
#include "logger.h"
#include "sound/SoundManager.h"
#include "render/TextRasterRenderer.h"
#include "decoders/stb_image.h"
#include "platform/platformgeneral.h"
#include "platform/platformgl.h"
#include "primitives.h"

static SoundManager *soundManager;
static TextRasterRenderer *textRenderer;

F32 theta = 0.0f;
GLuint textureId;

void loadTexture();
void loadSound();
void initTextRenderer();

void btInit() {
	log("BatteryTech 1.0 Initializing...");
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
	log("Ready");
	loadTexture();
	loadSound();
	initTextRenderer();
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

void loadSound() {
	log("Loading sound");
	soundManager = new SoundManager;
	soundManager->init(10);
	U16 sndId = soundManager->loadSound("level_1_song.ogg");
	//U16 sndId2 = soundManager->loadSound("battery_powered_splash.ogg");
	//U16 sndId3 = soundManager->loadSound("score_session_end_big.ogg");
	//soundManager->playSound(sndId, -1, 1.0f);
	//soundManager->playSound(sndId2, -1, 1.0f);
	//soundManager->playSound(sndId3, -1, 1.0f);
}

void initTextRenderer() {
	textRenderer = new TextRasterRenderer("lcdmn.ttf", 12.0f);
	textRenderer->init();
}

void btUpdate(F32 delta) {
	theta += delta * 60;
	//char buf[32];
	//sprintf(buf, "update %3.5g theta is %3.5g", delta, theta);
	//log(buf);
}


void btDraw() {
	// OpenGL animation code goes here
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glViewport(0,0,800,480);
	glLoadIdentity();
	glOrthof(-1.6f, 1.6f, -.96f, .96f, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glColor4f(1, 1, 1, 1);
	glPushMatrix();
	glRotatef( theta, 0.0f, 0.0f, 1.0f );
	glFrontFace(GL_CW);
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
	glBindTexture(GL_TEXTURE_2D, textureId);
	glColorPointer(4, GL_FLOAT, 0, &colors);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glPopMatrix();
	textRenderer->render();
}

void btRelease() {
	if (soundManager) {
		soundManager->release();
		delete soundManager;
	}
	soundManager = NULL;
}
