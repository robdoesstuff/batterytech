/*
 * Ball.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#include "Ball.h"
#include <bt-box2d/Collision/Shapes/b2CircleShape.h>
#include <bt-box2d/Dynamics/b2Fixture.h>
#include <string.h>
#include <batterytech/audio/AudioManager.h>

#define SOUND_TRIGGER_INTERVAL 0.100f;

// static field initialization
GameObjectMeta* Ball::meta = NULL;

Ball::Ball(GameContext *context) : GameObject(context) {
}

Ball::Ball(GameContext *context, F32 x, F32 y, F32 angle) : GameObject(context) {
	init(x, y, angle);
}

Ball::~Ball() {
	destroyBody();
}

void Ball::init(F32 x, F32 y, F32 angle) {
	type = GAMEOBJECT_TYPE_BALL;
	this->x = x;
	this->y = y;
	this->angle = angle;
	b2BodyDef cBodyDef;
	cBodyDef.type = b2_dynamicBody;
	cBodyDef.position.Set(x, y);
	cBodyDef.angle = angle;
	cBodyDef.bullet = false;
	boxBody = getWorld()->boxWorld->CreateBody(&cBodyDef);
	b2CircleShape dCircle;
	dCircle.m_radius = BALL_RADIUS;
	b2FixtureDef cFixtureDef;
	cFixtureDef.shape = &dCircle;
	cFixtureDef.density = 2.0f;
	cFixtureDef.friction = 0.1f;
	cFixtureDef.restitution = 0.21f;
	cFixtureDef.filter.categoryBits = COLLISION_BITS_BALL;
	boxBody->CreateFixture(&cFixtureDef);
	boxBody->SetUserData(this);
	soundTriggerTimeLeft = 0;
}

GameObjectMeta* Ball::getMeta() {
	if (!meta) {
		meta = new GameObjectMeta(1);
	}
	return meta;
}

void Ball::update() {
	if (soundTriggerTimeLeft > 0) {
		soundTriggerTimeLeft -= context->tickDelta;
		if (soundTriggerTimeLeft < 0) {
			soundTriggerTimeLeft = 0;
		}
	}
	b2Vec2 pos = boxBody->GetPosition();
	x = pos.x;
	y = pos.y;
	angle = boxBody->GetAngle() + PI;
	if (processContact && impactVelocityDelta > 15.0f) {
		if (soundTriggerTimeLeft == 0) {
			//context->audioManager->playSound("click.ogg", 0, 1.0f, 1.0f, .90f + ((rand() % 20) / 100.0f));
			soundTriggerTimeLeft = SOUND_TRIGGER_INTERVAL;
		}
	}
	clearImpact();
}

F32 Ball::getLinearVelocity() {
	return boxBody->GetLinearVelocity().Length();
}

void Ball::onTouchMove(F32 x, F32 y) {
	b2Transform transform = boxBody->GetTransform();
	// flick (20 is eyeballed multiplier)
	boxBody->SetLinearVelocity(b2Vec2((x - transform.p.x) * 20, (y - transform.p.y) * 20) );
	transform.p.x = x;
	transform.p.y = y;
	boxBody->SetTransform(transform.p, transform.q.GetAngle());
	boxBody->SetAwake(true);
}
