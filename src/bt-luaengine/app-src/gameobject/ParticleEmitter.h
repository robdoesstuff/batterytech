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
	// starts emitter
	void startEmitter();
	// stops emitter
	void stopEmitter();
	// updates particles
	void update(F32 delta);
	// where the current source location is for new particles
	void setEmitterSourceLocation(Vector3f srcLoc);
	// Range of 0-1 for 360 degree emission range
	void setEmissionConeRange(F32 range);
	// The base direction that the cone is based on
	void setEmissionDirection(Vector3f emissiondir);
	// Particle life time range in seconds
	void setParticleLifeTimeRange(F32 mx, F32 mn);
	// Particle scaling speed range (how fast the particle increases/decreases in size)
	void setParticleScaleSpeedRange(F32 mx, F32 mn);
	// Particle alpha speed range (how fast it becomes transparent or opaque)
	void setParticleAlphaSpeedRange(F32 mx, F32 mn);	
	// Particle movement speed range
	void setParticleMaxSpeedRange(F32 mx, F32 mn);
    // Particle rotation speed range
    void setParticleRotationSpeedRange(F32 mx, F32 mn);
	// Starting alpha value
	void setParticleInitialAlpha(F32 initAlpha);
	// Starting scale value
	void setParticleInitialScale(F32 initScale);
	// How many particles per second we emit
    void setEmissionRate(F32 rate); // particles per second
    // Gravity
    void setGravity(Vector3f gravity);
    // How wide of an area to spawn particles from centering on source location
	void setPositionRange(Vector3f range);
	// Are we 2D or 3D
    void setIs2D(BOOL32 is2D);
    // How many particles to emit before stopping
    void setAutoStopMax(S32 max);
    
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
    BOOL32		is2D;
    
private:
    BOOL32		running;
    BOOL32      stopped;
	Vector3f	lastSourceLoc;
	F32			emissionRange;
	F32			emissionRate;
	F32			nextEmissionTimeLeft;

	Vector3f	emissionDirection;
	
	F32			alpha;
	F32			scale;
    Vector3f	gravity;
	Vector3f	positionRange;
	Vector2f	ltRange;
	Vector2f	scaleSpeedRange;
	Vector2f	alphaSpeedRange;
	Vector2f	particleSpeedRange;	
    Vector2f    rotationRange;
	Vector2f	rotationSpeedRange;
	S32			autoStopAmount;
	S32			emittedCount;
	
    S32         emitterID;
    char*    	textureAssetName;
};

#endif
