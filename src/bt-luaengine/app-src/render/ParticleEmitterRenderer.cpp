/*
 *  ParticleEmitterRenderer.cpp
 */

#include "ParticleEmitterRenderer.h"
#include "../gameobject/ParticleEmitter.h"

#include <batterytech/render/RenderContext.h>
#include "../World.h"
#include <batterytech/render/GLResourceManager.h>
#include "WorldRenderer.h"
#include <batterytech/math/Quaternion.h>
#include <batterytech/Logger.h>

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

// general rotation around a point is:
// x' = xcos0 - ysin0;
// y' = xsin0 + ycos0;
#define ROTATE_X(x, y, rotSin, rotCos) (x*rotCos - y*rotSin)
#define ROTATE_Y(x, y, rotSin, rotCos) (x*rotSin + y*rotCos)

#define SORT_PARTICLES FALSE

ParticleEmitterRenderer::ParticleEmitterRenderer(GameContext *context)
{
	this->context = context;
	shaderProgram = new ShaderProgram("particle", "shaders/particle.vert", "shaders/particle.frag");
	this->vertAttsVBOId = 0;
	this->faceIndicesVBOId = 0;
	// Allocate vertex attribute buffer and face indices
	this->vertAtts	= new GLParticleVertex[MAX_PARTICLES*4];
	this->faceIndices = new U16[MAX_PARTICLES*6];
	// generate the face indices
	for (int i =0; i<MAX_PARTICLES; i++) {
		int ii = i*6;
		int jj = i*4;
		this->faceIndices[ii+0] = jj+0;
		this->faceIndices[ii+1] = jj+1;
		this->faceIndices[ii+2] = jj+2;
		this->faceIndices[ii+3] = jj+1;
		this->faceIndices[ii+4] = jj+3;
		this->faceIndices[ii+5] = jj+2;
	}
}

ParticleEmitterRenderer::~ParticleEmitterRenderer() {
	if (this->vertAttsVBOId)
		glDeleteBuffers(1, &this->vertAttsVBOId);
	if (this->faceIndicesVBOId)
		glDeleteBuffers(1, &this->faceIndicesVBOId);
	delete shaderProgram;
	if (this->vertAtts)
		delete [] vertAtts;	
	if (this->faceIndices)
		delete [] faceIndices;	
}

void ParticleEmitterRenderer::loadVerts() {
	glBindBuffer(GL_ARRAY_BUFFER, this->vertAttsVBOId);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(GLParticleVertex) * 4, this->vertAtts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void ParticleEmitterRenderer::loadFaceIndices() {
	// Bind to buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faceIndicesVBOId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_PARTICLES * 6 * sizeof(U16), this->faceIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ParticleEmitterRenderer::init(BOOL32 newContext) {
	if(newContext) {
		this->shaderProgram->load(newContext);
		// generate new bufferIDs 
		GLuint bufferIDs[2];
		glGenBuffers(2, bufferIDs);
		this->vertAttsVBOId = bufferIDs[0];
		this->faceIndicesVBOId = bufferIDs[1];
		// load default vertex attribute
		this->loadVerts();
		// load the face indices
		this->loadFaceIndices();
	}
}

// sorts particles back-to-front
static int particle_sort_function(const void *a, const void *b) {
    const Particle *pa = (const Particle*)a;
    const Particle *pb = (const Particle*)b;
    if (pa->sortValue > pb->sortValue) {
        return -1;
    }
    return 1;
 }

void ParticleEmitterRenderer::updateVertBuffer(ParticleEmitter *emitter) {
	if (emitter->numActiveParticles < 1) return;
	Vector3f dirToCamera = emitter->sourceLoc - this->context->world->camera->pos;
	dirToCamera.normalize(); // z
	Vector3f yRot = context->world->camera->up;
	Vector3f xRot = dirToCamera.cross(yRot);
	Matrix4f bbMat;
	bbMat.data[0] = xRot.x;
	bbMat.data[1] = xRot.y;
	bbMat.data[2] = xRot.z;
	bbMat.data[4] = yRot.x;
	bbMat.data[5] = yRot.y;
	bbMat.data[6] = yRot.z;
	bbMat.data[8] = dirToCamera.x;
	bbMat.data[9] = dirToCamera.y;
	bbMat.data[10] = dirToCamera.z;
    // sort particles back to front so we can have depth test enabled
    // we'll use distance squared from camera
#ifdef SORT_PARTICLES
    Vector3f cameraPos = context->world->camera->pos;
	for (U32 j = 0; j < emitter->numActiveParticles; j++) {
		Particle *p = &emitter->particles[j];
        p->sortValue = (cameraPos-p->pos).lengthSq();
    }
    qsort(emitter->particles, emitter->numActiveParticles, sizeof(Particle), particle_sort_function);
#endif
 	for (U32 j = 0; j < emitter->numActiveParticles; j++) {
		Particle *p = &emitter->particles[j];
 		int i = j*4;
		F32 rotSin = sin(p->rotation);
		F32 rotCos = cos(p->rotation);
		F32 left = -0.5*p->scale;
		F32 right = 0.5*p->scale;
		F32 top = 0.5*p->scale;
		F32 bottom = -0.5*p->scale;
		Vector3f pt1 = Vector3f(ROTATE_X(left, top, rotSin, rotCos), ROTATE_Y(left, top, rotSin, rotCos), 0.0);
		Vector3f pt2 = Vector3f(ROTATE_X(right, top, rotSin, rotCos), ROTATE_Y(right, top, rotSin, rotCos), 0.0);
		Vector3f pt3 = Vector3f(ROTATE_X(left, bottom, rotSin, rotCos), ROTATE_Y(left, bottom, rotSin, rotCos), 0.0);
		Vector3f pt4 = Vector3f(ROTATE_X(right, bottom, rotSin, rotCos), ROTATE_Y(right, bottom, rotSin, rotCos), 0.0);
        //curStep = curStep + toCamStep;
        Vector3f pos = p->pos;
        F32 alpha = p->alpha;
		this->vertAtts[i+0].position	= pos + bbMat * pt1;
		this->vertAtts[i+0].alpha		= alpha;
		this->vertAtts[i+0].uv			= Vector2f(0,1);
		this->vertAtts[i+1].position	= pos + bbMat * pt2;
		this->vertAtts[i+1].alpha		= alpha;
		this->vertAtts[i+1].uv			= Vector2f(1,1);
		this->vertAtts[i+2].position	= pos + bbMat * pt3;
		this->vertAtts[i+2].alpha		= alpha;
		this->vertAtts[i+2].uv			= Vector2f(0,0);
		this->vertAtts[i+3].position	= pos + bbMat * pt4;
		this->vertAtts[i+3].alpha		= alpha;
		this->vertAtts[i+3].uv			= Vector2f(1,0);
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, emitter->numActiveParticles*sizeof(GLParticleVertex)*4, this->vertAtts);	
	// printf("Drawing buffer from 0 to %d\n",emitter->numActiveParticles*sizeof(GLParticleVertex)*4);
}

void ParticleEmitterRenderer::render()
{
	checkGLError("PE start render");
    HashTable<S32, ParticleEmitter*> *emitters = context->world->emitters;

	int numActiveEmitters = emitters->size();

	if (numActiveEmitters>0)
	{
		glFrontFace(GL_CW);
		shaderProgram->bind();
		checkGLError("after shaderProgram->bind()");
		
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertAttsVBOId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faceIndicesVBOId);		
		glVertexAttribPointer(this->shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(GLParticleVertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(this->shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, sizeof(GLParticleVertex), BUFFER_OFFSET(sizeof(Vector3f)));
		glVertexAttribPointer(this->shaderProgram->getVertexAttributeLoc("alpha"), 1, GL_FLOAT, GL_FALSE, sizeof(GLParticleVertex), BUFFER_OFFSET(sizeof(Vector3f)+sizeof(Vector2f)));
        
		for (HashTable<S32,ParticleEmitter*>::Iterator i = emitters->getIterator(); i.hasNext;) {
            ParticleEmitter *emitter = emitters->getNext(i);
			updateVertBuffer(emitter);

            if (emitter->getTextureAssetName()) {
            	Texture *texture = context->glResourceManager->getTexture(emitter->getTextureAssetName());
            	if (texture) {
            		texture->bind();
            	}
            }
			glDrawElements(GL_TRIANGLES, emitter->numActiveParticles*6, GL_UNSIGNED_SHORT, 0);
		}
		//printf("Render: number of Emitter %d",numActiveEmitters);
	}
	
}
