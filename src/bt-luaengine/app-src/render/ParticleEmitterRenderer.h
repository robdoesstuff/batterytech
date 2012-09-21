/*
 *  ParticleEmitterRenderer.h
 */

#ifndef _PARTICLE_RENDERER_H_
#define _PARTICLE_RENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>
#include "../gameobject/ParticleEmitter.h"
#include "../GameContext.h"

using namespace BatteryTech;

struct GLParticleVertex {
	Vector3f	position; // 12 bytes
	Vector2f	uv; // 8 bytes
	F32			alpha;
	char		padding[8];
	// exactly 32 bytes - no padding needed
	
	GLParticleVertex() {
		// printf("GLParticleVertex default constructor\n");
		position = Vector3f(0.0,0.0,0.0);
		uv = Vector2f(0.0,1.0);
		alpha = 1.0;
	}
};


class ParticleEmitterRenderer : Renderer
{
public:
	ParticleEmitterRenderer(GameContext *context);
	virtual ~ParticleEmitterRenderer();
	virtual void init(BOOL32 newContent);
	void updateVertBuffer(ParticleEmitter *emitter);
	void loadVerts();
	void loadFaceIndices();
	void render(BOOL32 do2D);
private:
	GameContext *context;
	ShaderProgram* shaderProgram;
	GLuint vertAttsVBOId;
	GLuint faceIndicesVBOId;
	GLParticleVertex *vertAtts;
	U16	*faceIndices;				
	
};


#endif
