/*
 *  ParticleEmitter.h
 *
 */


#ifndef PARTICLE_EMITTER_H_
#define PARTICLE_EMITTER_H_
#include <stdio.h>
#include <stdlib.h>
#include <batterytech/primitives.h>
#include <batterytech/math/Vector3.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/math/Matrix4.h>
#include "../GameConstants.h"
#include <batterytech/util/strx.h>

using namespace BatteryTech;

struct Particle {
	
	Vector3f	pos;
	Vector3f	dir;
	F32			rotation;
	F32			scale;
	F32			alpha;
	F32			speed;
	F32			lifeLeft;
	F32			scaleSpeed;
	F32			alphaSpeed;
    F32         rotationSpeed;
    F32         sortValue;
	
	Particle(){
		this->pos		= Vector3f(0.0f,0.0f,0.0f);
		this->dir		= Vector3f(0.0f,0.0f,0.0f);
		this->rotation  = 0.0f;
		this->alpha		= 1.0f;
		this->scale		= 1.0f;
		this->speed		= 1.0f; 
		this->lifeLeft	= 0.0f;
		this->scaleSpeed= 1.0f;
		this->alphaSpeed= 1.0f;
        this->rotationSpeed = 0.5f;
	}

	void setupParticle(Vector3f position,Vector3f direction, F32 rotation, F32 alphaValue, F32 scaleValue, F32 speedValue, F32 lifeTime, F32 scaleSpeedValue, F32 alphaSpeedValue, F32 rotationSpeed)
	{
		this->pos		= position;
		this->dir		= direction;
		this->rotation  = rotation;
		this->alpha		= alphaValue;
		this->scale		= scaleValue;
		this->speed		= speedValue; 
		this->lifeLeft	= lifeTime;
		this->scaleSpeed= scaleSpeedValue;
		this->alphaSpeed= alphaSpeedValue;
        this->rotationSpeed = rotationSpeed;
	}

};


class ParticleEmitter{
	
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();
	void addParticle(Vector3f position);
	void removeParticle(int i);
	void startEmitter();
	void stopEmitter();
	void update(F32 delta);
	void setEmitterSourceLocation(Vector3f srcLoc);
	void setEmissionConeRange(F32 range);
	void setEmissionDirection(Vector3f emissiondir);
	void setParticleLifeTimeRange(F32 mx, F32 mn);
	void setParticleScaleSpeedRange(F32 mx, F32 mn);
	void setParticleAlphaSpeedRange(F32 mx, F32 mn);	
	void setParticleMaxSpeedRange(F32 mx, F32 mn);
	void setParticleInitialAlpha(F32 initAlpha);
	void setParticleInitialScale(F32 initScale);
    void setEmissionRate(F32 rate); // particles per second
    void setParticleRotationSpeedRange(F32 mx, F32 mn);
    void setGravity(F32 zGravity);
    
    S32 getID() { return emitterID; }
	Vector3f	sourceLoc;
	U32		 numActiveParticles;
	Particle particles[MAX_PARTICLES];
	const char* getTextureAssetName() { return textureAssetName; }
    void setTextureAssetName(const char *textureAssetName) {
    	if (this->textureAssetName) {
    		delete [] this->textureAssetName;
    	}
    	this->textureAssetName = strDuplicate(textureAssetName);
    }
    
private:
    BOOL32		running;
	Vector3f	lastSourceLoc;
	F32			emissionRange;
	F32			emissionRate;
	F32			nextEmissionTimeLeft;

	Vector3f	emissionDirection;
	
	F32			alpha;
	F32			scale;	
    F32         gravity;
	Vector2f	ltRange;
	Vector2f	scaleSpeedRange;
	Vector2f	alphaSpeedRange;
	Vector2f	particleSpeedRange;	
    Vector2f    rotationRange;
	Vector2f	rotationSpeedRange;	
	
    S32         emitterID;
    char*    	textureAssetName;
};

#endif
