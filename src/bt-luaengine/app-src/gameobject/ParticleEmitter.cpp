/*
 *  ParticleEmitter.cpp
 */

#include "ParticleEmitter.h"
#include <batterytech/Logger.h>

static S32 nextEmitterID = 1;
#define frand() (rand() / (F32)RAND_MAX)

F32 randomNumberWithinRange(Vector2f range) {
	F32 mn = range.x;
	F32 mx = range.y;
	if(mx<mn) 	{
		F32 tmp = mx;
		mx = mn;
		mn = tmp;
	}
	return frand()*(mx-mn) + mn;
}


ParticleEmitter::ParticleEmitter() {
	this->running				= FALSE;
	this->numActiveParticles	= 0;
	this->sourceLoc				= Vector3f(0,0,0);
	this->lastSourceLoc			= Vector3f(0,0,0);
	this->emissionRange			= 0.1f;
	// particles per second
	this->emissionRate			= 30.0f;
	this->emissionDirection		= Vector3f(0,0,1.0f);
	this->ltRange				= Vector2f(5,4);
	this->scaleSpeedRange		= Vector2f(0.5f,0.1f);
	this->alphaSpeedRange		= Vector2f(-0.1f,-0.01f);
	this->particleSpeedRange	= Vector2f(5,4);
	this->alpha					= 1.0f;
	this->scale					= 1.0f;
	this->nextEmissionTimeLeft	= 0;
	this->emitterID				= nextEmitterID++;
    this->textureAssetName      = NULL;
    this->rotationRange         = Vector2f(0.0, TAU);
    this->rotationSpeedRange    = Vector2f(-5.0, 5.0);
    this->gravity               = 0;
    this->stopped               = FALSE;
}

ParticleEmitter::~ParticleEmitter() {
	running = FALSE;
	if (textureAssetName) {
		delete [] textureAssetName;
	}
}

void ParticleEmitter::addParticle(Vector3f position) {
	if (this->numActiveParticles>=MAX_PARTICLES) {
		logmsg("No more particles available");
		return;
	}
	Vector3f dir = emissionDirection;
	// subtract half the emission range so that we center on the emission direction
	F32 halfEmissionRangeDeg = emissionRange * 180.0f;
	F32 emissionRangeDegrees = emissionRange * 360.0f;
	dir.rotate(frand() * emissionRangeDegrees - halfEmissionRangeDeg, frand() * emissionRangeDegrees - halfEmissionRangeDeg, frand() * emissionRangeDegrees - halfEmissionRangeDeg);
	dir.normalize();

	F32 speed		= randomNumberWithinRange(this->particleSpeedRange);	// 0.7*maxspeed < spd < maxspeed
    F32 rotation    = randomNumberWithinRange(this->rotationRange);
	F32 lifetime	= randomNumberWithinRange(this->ltRange);				// *0.3*drand48() + 0.7;
	F32 scalerate	= randomNumberWithinRange(this->scaleSpeedRange);
	F32 alpharate	= randomNumberWithinRange(this->alphaSpeedRange);
    F32 rotaterate  = randomNumberWithinRange(this->rotationSpeedRange);
    
	int i = numActiveParticles++;
	this->particles[i].setupParticle(position,dir, rotation,this->alpha,this->scale,
									 speed, lifetime, scalerate, alpharate, rotaterate);
}

void ParticleEmitter::removeParticle(int i) {
    // swap last with this position to keep the list defragmented
	if (numActiveParticles > 1)	{
		this->particles[i] = this->particles[this->numActiveParticles-1];
	}
    this->numActiveParticles--;
}

void ParticleEmitter::startEmitter() {
	running = TRUE;
}

void ParticleEmitter::update(F32 delta) {
	if (!running) {
		return;
	}
	nextEmissionTimeLeft -= delta;
	// interpolate if we need to emit more than one for this update
	if ( !stopped && (nextEmissionTimeLeft <= 0)) {
		F32 timeRemainder = -nextEmissionTimeLeft;
		F32 rateTimeLeft = 1.0f/emissionRate;
		Vector3f locDelta = sourceLoc - lastSourceLoc;
        
		if (timeRemainder > rateTimeLeft) {
			// need to interpolate now
			while (timeRemainder > rateTimeLeft) {
				// interpolating from lastSourceLoc up to sourceLoc
				addParticle((locDelta * ((delta-timeRemainder)/delta)) + lastSourceLoc);
				timeRemainder -= rateTimeLeft;
			}
		}
		// last particle should be emitted as close to sourceLoc as possible (and accurate to keep it smooth)
		addParticle((locDelta * ((delta-timeRemainder)/delta)) + lastSourceLoc);
		// emission rate is particles per second and timeleft is seconds so convert
		this->nextEmissionTimeLeft = 1.0f/this->emissionRate - timeRemainder;
	}
    // get rid of any inactive particles
    for (U32 i = 0; i < numActiveParticles; i++) {
        Particle *p = &this->particles[i];
        p->lifeLeft -= delta;
        if (p->lifeLeft <= 0) {
            removeParticle(i);
        }
    }
    for (U32 i = 0; i < numActiveParticles; i++) {
        Particle *p = &this->particles[i];
        // all particles remaining are now currently active
        // update the particle
        p->pos = p->pos + p->dir*p->speed*delta;
        p->pos.z = p->pos.z + gravity*delta*delta;
        
        p->rotation = p->rotation - delta*0.5;
        p->alpha = p->alpha+p->alphaSpeed*delta;
        if(p->alpha > 1.0) { p->alpha=1.0; }
        if(p->alpha < 0.0) {
            p->alpha=0.0;
            // can't see it, get rid of it
            p->lifeLeft = 0;
        }
        p->scale = p->scale+p->scaleSpeed*delta;
        if (p->scale < 0) p->scale = 0;
        p->rotation=p->rotation+p->rotationSpeed*delta;
    }
    // char buf[255];
    // sprintf(buf, "%d particles active", numActiveParticles);
    // logmsg(buf);

}

void ParticleEmitter::stopEmitter() {
	stopped = TRUE;
}

void ParticleEmitter::setEmitterSourceLocation(Vector3f srcLoc) {
	// if it's running, we use the previous location so we have a delta to interpolate against,
	// otherwise assume it's moving hidden so far and we don't want to interpolate when it starts.
	if (running) {
		lastSourceLoc = this->sourceLoc;
	} else {
		lastSourceLoc = srcLoc;
	}
	this->sourceLoc = srcLoc;
}


void ParticleEmitter::setEmissionConeRange(F32 range)
{
	if (range>1) range = 1.0;
	if (range<0) range = 0.0;	
	this->emissionRange = range;
}

void ParticleEmitter::setParticleLifeTimeRange(F32 mx, F32 mn)
{
	
	this->ltRange = Vector2f(mx,mn);
}

void ParticleEmitter::setParticleScaleSpeedRange(F32 mx, F32 mn)
{
	this->scaleSpeedRange = Vector2f(mx,mn);	
}

void ParticleEmitter::setParticleAlphaSpeedRange(F32 mx, F32 mn)
{
	this->alphaSpeedRange = Vector2f(mx,mn);		
}

void ParticleEmitter::setParticleMaxSpeedRange(F32 mx, F32 mn)
{
	this->particleSpeedRange = Vector2f(mx,mn);		
}

void ParticleEmitter::setParticleInitialAlpha(F32 initAlpha)
{
	if(initAlpha>=0 && initAlpha<=1)
		this->alpha = initAlpha;
}

void ParticleEmitter::setParticleInitialScale(F32 initScale)
{
	if(initScale>=0 && initScale<=1)
		this->scale = initScale;
}

void ParticleEmitter::setParticleRotationSpeedRange(F32 mx, F32 mn)
{
    this->particleSpeedRange = Vector2f(mx,mn);
}

void ParticleEmitter::setEmissionDirection(Vector3f emissiondir)
{
	this->emissionDirection = emissiondir;
}

void ParticleEmitter::setEmissionRate(F32 rate)
{
    emissionRate = rate;
}

void ParticleEmitter::setGravity(F32 zGravity)
{
    gravity = zGravity;
}

