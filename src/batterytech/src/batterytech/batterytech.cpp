//============================================================================
// Name        : batterytech.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "batterytech.h"

F32 theta = 0.0f;
GLuint textureId;

void loadTexture();

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
}

void loadTexture() {
	log("Loading texture");
	int x,y,n;
	unsigned char *data = stbi_load("assets\\text_bg_tex.jpg", &x, &y, &n, 0);
	if (data) {
		int bytes = x * y * n * sizeof(unsigned char);
		char buf[50];
		sprintf(buf, "Image is %ix%i color components=%i bytes=%i",x,y,n, bytes);
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
}
