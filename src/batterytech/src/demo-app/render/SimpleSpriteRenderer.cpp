/*
 * SimpleSpriteRenderer.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#include "SimpleSpriteRenderer.h"
#include "../GameConstants.h"
#include <stdio.h>
#include <string.h>
#include <batterytech/Logger.h>
#include <batterytech/util/esTransform.h>

char vShaderStr[] =
		"uniform mat4 projection_matrix; \n" \
		"uniform mat4 modelview_matrix; \n" \
	   "attribute vec3 vPosition;   \n" \
	   "attribute vec2 uvMap;   \n" \
	   "void main()                 \n" \
	   "{                           \n" \
	   " gl_TexCoord[0] = vec4(uvMap, 1.0, 1.0); \n" \
	   "   gl_Position = projection_matrix * modelview_matrix * vec4(vPosition, 1.0); \n" \
	   "};                          \n";

char fShaderStr[] =
	   "precision mediump float;                  \n" \
	   "uniform sampler2D tex;				      \n" \
	   "void main()                               \n" \
	   "{                                         \n" \
	   " gl_FragColor = texture2D(tex, gl_TexCoord[0].st); \n" \
	   "}                                         \n";

GLuint loadShader(GLenum type, const char *shaderSrc);
void logShaderInfo(GLuint obj);
void logProgramInfo(GLuint obj);
GLuint vShader, fShader, program, projMatrixUniform, mvMatrixUniform, vertPosition, texPosition, uvmapPosition;

SimpleSpriteRenderer::SimpleSpriteRenderer(Context *context, const char *spriteAssetName) {
	this->context = context;
	this->spriteAssetName = spriteAssetName;
	textureId = 0;
}

SimpleSpriteRenderer::~SimpleSpriteRenderer() {
}

void SimpleSpriteRenderer::init(BOOL32 newContext) {
	if (!newContext && textureId) {
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	if (spriteAssetName && strlen(spriteAssetName) > 0) {
		textureId = loadTexture(spriteAssetName);
	}
	GLint status = 0;
	vShader = loadShader(GL_VERTEX_SHADER, vShaderStr);
	fShader = loadShader(GL_FRAGMENT_SHADER, fShaderStr);
	program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		logProgramInfo(program);
	}
	vertPosition = glGetAttribLocation(program, "vPosition");
	uvmapPosition = glGetAttribLocation(program, "uvMap");
	projMatrixUniform = glGetUniformLocation(program, "projection_matrix");
	mvMatrixUniform = glGetUniformLocation(program, "modelview_matrix");
	texPosition = glGetUniformLocation(program, "tex");
	//char buf[50];
	//sprintf(buf, "Loaded texture Id=%d", textureId);
	//logmsg(buf);
}

void SimpleSpriteRenderer::setSpriteAssetname(const char* spriteAssetName) {
	this->spriteAssetName = spriteAssetName;
}

void SimpleSpriteRenderer::render(F32 top, F32 right, F32 bottom, F32 left) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	drawTexturedQuad(top, right, bottom, left);
}

void SimpleSpriteRenderer::render(F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
	// this one
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUseProgram(program);
	F32 top = height/2;
	F32 right = width/2;
	F32 bottom = -height/2;
	F32 left = -width/2;
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	ESMatrix projMatrix, mvMatrix;
	esMatrixLoadIdentity(&projMatrix);
	esMatrixLoadIdentity(&mvMatrix);
	esOrtho(&projMatrix, WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, 1, -1);
	esTranslate(&mvMatrix, x, y, 0);
	esRotate(&mvMatrix, angleRads * (180 / PI), 0, 0, -1.0f);
	glEnableVertexAttribArray(vertPosition);
	glEnableVertexAttribArray(uvmapPosition);
	glVertexAttribPointer(vertPosition, 3, GL_FLOAT, GL_FALSE, 0, verts);
	glVertexAttribPointer(uvmapPosition, 2, GL_FLOAT, GL_FALSE, 0, uvs);
	glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, (GLfloat*) &projMatrix.m[0][0]);
	glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, (GLfloat*) &mvMatrix.m[0][0]);
	glUniform1i(texPosition, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableVertexAttribArray(vertPosition);
	glDisableVertexAttribArray(uvmapPosition);
	glUseProgram(0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// GL1 rendering branch
	//glFrontFace(GL_CW);
	//glPushMatrix();
	//glTranslatef(x, y, 0);
	//glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	//glVertexPointer(3, GL_FLOAT, 0, &verts);
	//glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glPopMatrix();

}

void SimpleSpriteRenderer::renderUVLimited(F32 x, F32 y, F32 width, F32 height, F32 angleRads, F32 uvTop, F32 uvBottom) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	F32 top = height/2;
	F32 right = width/2;
	F32 bottom = -height/2;
	F32 left = -width/2;
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, uvTop, 1, uvTop, 1, uvBottom, 0, uvBottom
	};
	glFrontFace(GL_CW);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void SimpleSpriteRenderer::renderUVLimitedSpecific(F32 top, F32 right, F32 bottom, F32 left, F32 x, F32 y, F32 angleRads, F32 uvTop, F32 uvBottom) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, uvTop, 1, uvTop, 1, uvBottom, 0, uvBottom
	};
	glFrontFace(GL_CW);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void SimpleSpriteRenderer::drawTexturedQuad(F32 top, F32 right, F32 bottom, F32 left) {
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

GLuint loadShader(GLenum type, const char *shaderSrc) {
   GLuint shader;
   GLint compiled;
   // Create the shader object
   shader = glCreateShader(type);
   if(shader == 0) {
	   return 0;
   }
   // Load the shader source
   glShaderSource(shader, 1, &shaderSrc, NULL);
   // Compile the shader
   glCompileShader(shader);
   // Check the compile status
   glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
   if(!compiled) {
	   logShaderInfo(shader);
	   glDeleteShader(shader);
      return 0;
   }
   return shader;
}

void logShaderInfo(GLuint obj) {
    GLint infoLen = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 0) {
  	  char infoLog[1024];
  	  glGetShaderInfoLog(obj, (infoLen > 1024 ? 1024 : infoLen), NULL, infoLog);
  	  logmsg(infoLog);
    }
}

void logProgramInfo(GLuint obj) {
    GLint infoLen = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 0) {
  	  char infoLog[1024];
  	  glGetProgramInfoLog(obj, (infoLen > 1024 ? 1024 : infoLen), NULL, infoLog);
  	  logmsg(infoLog);
    }
}

