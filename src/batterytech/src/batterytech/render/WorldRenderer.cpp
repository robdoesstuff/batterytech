/*
 * WorldRenderer.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#include "WorldRenderer.h"
#include "TextRasterRenderer.h"
#include "GraphicsConfiguration.h"
#include "../decoders/stb_image.h"
#include "../platform/platformgl.h"
#include "../platform/platformgeneral.h"
#include "../Logger.h"
#include <Math.h>

#define FOV 45.0f
#define ZNEAR 0.1f
#define ZFAR 50.0f

GLuint loadTexture(const char *name);
GLuint bgTextureId;
GLuint cubeTextureId;
void createCube(S32 detail);

WorldRenderer::WorldRenderer(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
}

void WorldRenderer::init() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	textRenderer = new TextRasterRenderer(gConfig, "lcdmn.ttf", 12.0f);
	textRenderer->init();
	cubeTextureId = loadTexture("bpg_256_tex.jpg");
	bgTextureId = loadTexture("earth_horizon_tex.jpg");
}

void WorldRenderer::unloadLevel() {
}
void WorldRenderer::loadLevel() {
}

void WorldRenderer::render(World *world) {

	// OpenGL animation code goes here
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1, 1, 1, 1);
	// draw background
	// projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, gConfig->viewportWidth, gConfig->viewportHeight, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float bgVerts[] = {
			// top left
			0,0,0,
			// top right
			gConfig->viewportWidth, 0, 0,
			// bottom right
			gConfig->viewportWidth, gConfig->viewportHeight, 0,
			// bottom left
			0, gConfig->viewportHeight, 0
	};
	float bgUvs[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
	glDisable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, bgTextureId);
	glVertexPointer(3, GL_FLOAT, 0, &bgVerts);
	glTexCoordPointer(2, GL_FLOAT, 0, &bgUvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// do cube
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
	glLoadIdentity();
	F32 aspectRatio = gConfig->viewportWidth / (F32)gConfig->viewportHeight;
	gles_gluPerspective(FOV, aspectRatio, ZNEAR, ZFAR);
	//glOrthof(0, gConfig->viewportWidth, gConfig->viewportHeight, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// lights

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnableClientState(GL_NORMAL_ARRAY);
	// Create light components
	float ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specularLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float position[] = { 0, 0, 0, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	GLfloat spot_direction[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 25.0);

	float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
	float specReflection[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess[] = {50.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	unsigned short indices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22,
			23 };
	float verts[] = {
	// front
			-1, 1, 1, 1, 1, 1, 1, -1, 1, -1, -1, 1,
			// back
			1, 1, -1, -1, 1, -1, -1, -1, -1, 1, -1, -1,
			// right
			1, 1, 1, 1, 1, -1, 1, -1, -1, 1, -1, 1,
			// left
			-1, 1, -1, -1, 1, 1, -1, -1, 1, -1, -1, -1,
			// top
			-1, 1, -1, 1, 1, -1, 1, 1, 1, -1, 1, 1,
			//bottom
			1, -1, -1, -1, -1, -1, -1, -1, 1, 1, -1, 1, };
	float uvs[] = { 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0,
			1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0,
			1, 0, 1, 1, 0, 1, };
	float normals[] = {
			// front
			0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
			// back
			0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
			// right
			1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
			// left
			-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
			// top
			0, 1, 0, 	0, 1, 0, 	0, 1, 0, 	0, 1, 0,
			//bottom
			0, -1, 0, 	0, -1, 0, 	0, -1, 0, 	0, -1, 0,  };

	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, cubeTextureId);
	//glColorPointer(4, GL_FLOAT, 0, &colors);
	glNormalPointer(GL_FLOAT, 0, &normals);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	// t1
	glPushMatrix();
	glTranslatef(2.5f, 0, -20);
	glRotatef(world->theta, 0.0f, 1.0f, 0.0f);
	//glRotatef(45 + world->theta, 1.0f, 0.0f, 0.0f);
	glRotatef(45, 1.0f, 0.0f, 0.0f);
	glScalef(3, 3, 3);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, &indices);
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisableClientState(GL_NORMAL_ARRAY);
	textRenderer->render(world);
}

WorldRenderer::~WorldRenderer() {
	delete textRenderer;
}

GLuint loadTexture(const char *name) {
	GLuint textureId;
	logmsg("Loading texture:");
	logmsg(name);
	int x, y, n;
	int assetSize = 0;
	unsigned char *fileData = _platform_load_asset(name,
			&assetSize);
	if (fileData) {
		char buf[50];
		sprintf(buf, "Loaded %i bytes of raw image data", assetSize);
		logmsg(buf);
	}
	unsigned char *data = stbi_load_from_memory(fileData, assetSize, &x, &y,
			&n, 0);
	//unsigned char *data = stbi_load("assets\\text_bg_tex.jpg", &x, &y, &n, 0);
	if (data) {
		int bytes = x * y * n * sizeof(unsigned char);
		char buf[50];
		sprintf(buf, "Bitmap is %ix%i color components=%i bytes=%i", x, y, n,
				bytes);
		logmsg(buf);
		GLuint textureIds[1];
		glGenTextures(1, textureIds);
		textureId = textureIds[0];
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (n == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB,
					GL_UNSIGNED_BYTE, data);
		} else if (n == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, data);
		}
		GLenum error = glGetError();
		if (error) {
			logmsg("GL Error");
		}
		stbi_image_free(data);
	}
	_platform_free_asset(fileData);
	return textureId;
}

void createCube(S32 detail) {
	S32 columns = detail;
	S32 rows = detail;
	char buf[50];
	//sprintf(buf, "Creating cube with %d faces per side", facesPerSide);
	logmsg(buf);
	S32 indexCount = columns * rows * 6 * 6;
	S32 vertCount = (columns + 1) * (rows + 1);
	//indices = new unsigned short[indexCount];
	// 4, 9, 16, 25, etc..
	//verts = new float[vertCount * 3];
	//normals = new float[vertCount * 3];

	S32 count = -1;
	S32 i,j;
	// front
	for (i = 0; i < rows + 1; i++) {
		for (j = 0; j < columns + 1; j++) {
		}
	}

}
