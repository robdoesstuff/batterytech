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

//============================================================================
// Name        : QuadRenderer.cpp
// Description : Renders Quads
//============================================================================

#include "QuadRenderer.h"
#include <stdio.h>
#include <string.h>
#include "../Logger.h"
#include "GraphicsConfiguration.h"
#include "RenderContext.h"
#include "GLResourceManager.h"
#include "../Context.h"

// this will buffer 200 quads for batching
#define BATCH_BUFFER_SIZE 200

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

// general rotation around a point is:
// x' = xcos0 - ysin0;
// y' = xsin0 + ycos0;
#define ROTATE_X(x, y, rotSin, rotCos) (x*rotCos - y*rotSin)
#define ROTATE_Y(x, y, rotSin, rotCos) (x*rotSin + y*rotCos)

#define USE_VBOS_WHEN_AVAILBLE FALSE

namespace BatteryTech {

QuadRenderer::QuadRenderer(Context *context) {
	this->context = context;
    batchEnabled = FALSE;
    inBatch = FALSE;
    vertVBOId = 0;
    idxVBOId = 0;
    quadsBatched = 0;
    batchStatesSet = FALSE;
    vertBuffer = new GLQuadVertex[BATCH_BUFFER_SIZE*4];
    indices = new U16[BATCH_BUFFER_SIZE*6];
    // generate the face indices
    for (S32 i = 0; i < BATCH_BUFFER_SIZE; i++) {
        S32 ii = i*6;
        S32 jj = i*4;
        indices[ii+0] = jj+0;
        indices[ii+1] = jj+1;
        indices[ii+2] = jj+2;
        indices[ii+3] = jj+0;
        indices[ii+4] = jj+2;
        indices[ii+5] = jj+3;
    }
}

QuadRenderer::~QuadRenderer() {
    delete [] vertBuffer;
    vertBuffer = NULL;
    delete [] indices;
    indices = NULL;
    if (vertVBOId) {
		glDeleteBuffers(1, &vertVBOId);
    }
    vertVBOId = 0;
    if (idxVBOId) {
		glDeleteBuffers(1, &idxVBOId);
    }
    idxVBOId = 0;
}

void QuadRenderer::init(BOOL32 newContext) {
	if (context->gConfig->useShaders) {
		// this is a particularly important renderer so make sure it's ready to go
		ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram("quad");
		if (!shaderProgram) {
			shaderProgram = new ShaderProgram("quad", "shaders/quadshader.vert", "shaders/quadshader.frag");
			context->glResourceManager->addShaderProgram("quad", shaderProgram);
		}
		shaderProgram->load(FALSE);
	}
    batchEnabled = TRUE;
    if (USE_VBOS_WHEN_AVAILBLE && (context->gConfig->useShaders || context->gConfig->supportsVBOs)) {
        if (!newContext) {
            if (vertVBOId) {
                glDeleteBuffers(1, &vertVBOId);
            }
            vertVBOId = 0;
            if (idxVBOId) {
                glDeleteBuffers(1, &idxVBOId);
            }
            idxVBOId = 0;
        }
		GLuint bufferIDs[2];
		glGenBuffers(2, bufferIDs);
		vertVBOId = bufferIDs[0];
		idxVBOId = bufferIDs[1];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxVBOId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, BATCH_BUFFER_SIZE * 6 * sizeof(U16), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, vertVBOId);
        glBufferData(GL_ARRAY_BUFFER, BATCH_BUFFER_SIZE * 4 * sizeof(GLQuadVertex), vertBuffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
	checkGLError("QuadRenderer Init");
}

void QuadRenderer::render(Texture *texture, F32 top, F32 right, F32 bottom, F32 left) {
	Vector2f uvs[] = { Vector2f(0,0), Vector2f(1,0), Vector2f(1,1), Vector2f(0,1)};
	F32 desWidth = right-left;
	F32 desHeight = bottom-top;
	F32 width = desWidth;
	F32 height = desHeight;
	Vector2f offset(0,0);
	if (texture) {
        if (texture->getTextureId() != Texture::lastTextureId) {
            drawBatch();
        }
		texture->bind();
		Matrix4f tMat = texture->getMatrix();
		// transform the UVs into texture space (to support atlased images)
		for (S32 i = 0; i < 4; i++) {
			uvs[i] = tMat * uvs[i];
		}
		// take the dimensions and convert them into percentages of the desired draw size(scale)
		Vector2f scale = Vector2f(desWidth / texture->getOriginalSize().x, desHeight / texture->getOriginalSize().y);
		Vector2f actualSize = texture->getTrimmedSize() * scale;
		offset = texture->getCornerOffset() * scale;
		width = actualSize.x;
		height = actualSize.y;
	}
	F32 actualLeft = left + offset.x;
	F32 actualRight = actualLeft + width;
	F32 actualTop = top + offset.y;
	F32 actualBottom = actualTop + height;
    Vector4f colors[] = { Vector4f(1,1,1,1), Vector4f(1,1,1,1), Vector4f(1,1,1,1), Vector4f(1,1,1,1) };
	Vector3f verts[] = {
			Vector3f(actualLeft, actualTop, 0), Vector3f(actualRight, actualTop, 0), Vector3f(actualRight, actualBottom, 0), Vector3f(actualLeft, actualBottom, 0)
	};
    // if batching isn't on, we automatically enable it (with a fallback to old rendering code when unavaialble)
    // even though we're only putting one draw in the batch, we don't have to switch off VBOs because the batch code handles that
    // exactly the same way we would for a single draw.
    BOOL32 autoBatch = FALSE;
    if (!inBatch) {
        startBatch();
        autoBatch = TRUE;
    }
    if (inBatch) {
        addToBatch(verts, uvs, context->renderContext->colorFilter, Vector3f(0,0,0), 0, NULL);
        if (autoBatch) {
            endBatch();
        }
        return;
    }
	if (context->gConfig->useShaders) {
		ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram("quad");
		shaderProgram->bind();
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, 0, colors);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glColorPointer(4, GL_FLOAT, 0, &colors);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

void QuadRenderer::render(Texture *texture, F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
	render(texture, Vector3f(x,y,0), angleRads, Vector4f(0,0,1,1), Vector2f(width,height), Vector4f(1,1,1,1), TRUE, FALSE, Matrix4f());
}

void QuadRenderer::render(Texture *texture, Vector3f pos, F32 angleRads, Vector4f myUvs, Vector2f scale, Vector4f colorFilter, BOOL32 isOpaque, BOOL32 bb, Matrix4f bbMat) {
	Vector2f uvs[] = { Vector2f(myUvs.x, myUvs.y), Vector2f(myUvs.z, myUvs.y),
			Vector2f(myUvs.z, myUvs.w), Vector2f(myUvs.x, myUvs.w)
	};
    Vector4f colors[] = { Vector4f(1,1,1,1), Vector4f(1,1,1,1), Vector4f(1,1,1,1), Vector4f(1,1,1,1) };
	F32 desWidth = scale.x;
	F32 desHeight = scale.y;
	F32 width = desWidth;
	F32 height = desHeight;
	Vector2f offsetDelta(0,0);
	if (texture) {
        if (texture->getTextureId() != Texture::lastTextureId) {
            drawBatch();
        }
		texture->bind();
		Matrix4f tMat = texture->getMatrix();
		// transform the UVs into texture space (to support atlased images)
		for (S32 i = 0; i < 4; i++) {
			uvs[i] = tMat * uvs[i];
		}
		// take the dimensions and convert them into percentages of the desired draw size(scale)
		Vector2f scale = Vector2f(desWidth / texture->getOriginalSize().x, desHeight / texture->getOriginalSize().y);
		Vector2f actualSize = texture->getTrimmedSize() * scale;
		// this is the corner offset when the trimmed image is perfectly centered (which is how we lay it out by default)
		// so really what we want to know is the difference between that and the actual trim position
		Vector2f centerOffset = (texture->getOriginalSize() - texture->getTrimmedSize()) / 2.0f;
		// this is the difference between our default corner and the real corner scaled to the req draw size
		offsetDelta = (texture->getCornerOffset() - centerOffset) * scale;
		width = actualSize.x;
		height = actualSize.y;
	}
	F32 right = width/2 + offsetDelta.x;
    F32 top = -height/2 + offsetDelta.y;
    F32 bottom = height/2 + offsetDelta.y;
	F32 left = -width/2 + offsetDelta.x;
	Vector3f verts[] = {
			Vector3f(left, top, 0), Vector3f(right, top, 0), Vector3f(right, bottom, 0), Vector3f(left, bottom, 0)
	};
    // if batching isn't on, we automatically enable it (with a fallback to old rendering code when unavaialble)
    // even though we're only putting one draw in the batch, we don't have to switch off VBOs because the batch code handles that
    // exactly the same way we would for a single draw.
    BOOL32 autoBatch = FALSE;
    if (!inBatch) {
        startBatch();
        autoBatch = TRUE;
    }
    if (inBatch) {
        addToBatch(verts, uvs, colorFilter, pos, angleRads, (bb ? &bbMat : NULL));
        if (autoBatch) {
            endBatch();
        }
        return;
    }
	if (context->gConfig->useShaders) {
		Matrix4f myMvMatrix = context->renderContext->mvMatrix;
		myMvMatrix.translate(pos.x, pos.y, pos.z);
		myMvMatrix.rotate(angleRads * (180 / PI), 0, 0, -1.0f);
		if (bb) {
			myMvMatrix = myMvMatrix * bbMat;
		}
		ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram("quad");
		shaderProgram->bind();
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, 0, colors);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		// GL1 rendering branch
		Vector4f colorFilter = context->renderContext->colorFilter;
		glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glColorPointer(4, GL_FLOAT, 0, &colors);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
	}
}

    void QuadRenderer::startBatch() {
        if (batchEnabled) {
            inBatch = TRUE;
        }
        if (!USE_VBOS_WHEN_AVAILBLE && (context->gConfig->useShaders || context->gConfig->supportsVBOs)) {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);           
        }
    }
    
    void QuadRenderer::endBatch() {
        drawBatch();
        inBatch = FALSE;
        batchStatesSet = FALSE;
    }

    void QuadRenderer::addToBatch(Vector3f *quadVerts, Vector2f *quadUVs, Vector4f colorFilter, Vector3f position, F32 rotation, Matrix4f *bbMat) {
        if (quadsBatched >= BATCH_BUFFER_SIZE) {
            drawBatch();
        }
        for (S32 i = 0; i < 4; i++) {
            if (rotation) {
                F32 rotSin = sin(rotation);
                F32 rotCos = cos(rotation);
                F32 x = quadVerts[i].x;
                F32 y = quadVerts[i].y;
                F32 z = quadVerts[i].z;
                if (bbMat) {
                    vertBuffer[quadsBatched*4+i].position = *bbMat * Vector3f(ROTATE_X(x, y, rotSin, rotCos), ROTATE_Y(x, y, rotSin, rotCos), z) + position;
                } else {
                    vertBuffer[quadsBatched*4+i].position = Vector3f(ROTATE_X(x, y, rotSin, rotCos), ROTATE_Y(x, y, rotSin, rotCos), z) + position;
                }
            } else {
                if (bbMat) {
                    vertBuffer[quadsBatched*4+i].position = *bbMat * quadVerts[i] + position;
                } else {
                    vertBuffer[quadsBatched*4+i].position = quadVerts[i] + position;
                    
                }
            }
            vertBuffer[quadsBatched*4+i].uv = quadUVs[i];
            vertBuffer[quadsBatched*4+i].color = colorFilter;
       }
        quadsBatched++;
    }
    
    void QuadRenderer::drawBatch() {
        if (!inBatch || !quadsBatched) {
            return;
        }
        Vector4f colorFilter = context->renderContext->colorFilter;
        BOOL32 useVBOs = (USE_VBOS_WHEN_AVAILBLE && (context->gConfig->useShaders || context->gConfig->supportsVBOs));
        if (useVBOs) {
            // batching is only enabled if we're supporting VBOs and if we're supporting those, we use them everywhere so we don't have to switch out
            if (!batchStatesSet) {
                glBindBuffer(GL_ARRAY_BUFFER, vertVBOId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxVBOId);
            }
            glBufferSubData(GL_ARRAY_BUFFER, 0, quadsBatched*sizeof(GLQuadVertex)*4, vertBuffer);
        }
        if (context->gConfig->useShaders) {
            ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram("quad");
            if (!batchStatesSet) {
				Matrix4f myMvMatrix = context->renderContext->mvMatrix;
				shaderProgram->bind();
                if (useVBOs) {
                    glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(0));
                    glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)));
                    glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)+sizeof(Vector2f)));
                }
				glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
				glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMvMatrix.data);
				glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
				glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
            }
            if (!useVBOs) {
                glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), &vertBuffer[0].position);
                glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), &vertBuffer[0].uv);
                glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), &vertBuffer[0].color);
            }
        } else {
            // GL1 rendering branch
            glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
            if (useVBOs) {
                glVertexPointer(3, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(0));
                glTexCoordPointer(2, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)));
        		glColorPointer(4, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)+sizeof(Vector2f)));
           } else {
                glVertexPointer(3, GL_FLOAT, sizeof(GLQuadVertex), &vertBuffer[0].position);
                glTexCoordPointer(2, GL_FLOAT, sizeof(GLQuadVertex), &vertBuffer[0].uv);
        		glColorPointer(4, GL_FLOAT, sizeof(GLQuadVertex), &vertBuffer[0].color);
          }
        }
        if (useVBOs) {
            glDrawElements(GL_TRIANGLES, quadsBatched*6, GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawElements(GL_TRIANGLES, quadsBatched*6, GL_UNSIGNED_SHORT, indices);
        }
        // char buf[255];
        // sprintf(buf, "drawing batch of %d", quadsBatched);
        // logmsg(buf);
        quadsBatched = 0;
        // no need to re-set the same buffer + shader states every draw while in batch and just switching textures
        batchStatesSet = TRUE;
    }
}

