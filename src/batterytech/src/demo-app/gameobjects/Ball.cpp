/*
 * Ball.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#include "Ball.h"
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <string.h>

// static field initialization
GameObjectMeta* Ball::meta = NULL;

Ball::Ball(Context *context) : GameObject(context) {
}

Ball::Ball(Context *context, F32 x, F32 y, F32 angle) : GameObject(context) {
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
}

GameObjectMeta* Ball::getMeta() {
	if (!meta) {
		meta = new GameObjectMeta(1);
	}
	return meta;
}

void Ball::update() {
	b2Vec2 pos = boxBody->GetPosition();
	x = pos.x;
	y = pos.y;
	angle = boxBody->GetAngle();
	clearImpact();
}

F32 Ball::getLinearVelocity() {
	return boxBody->GetLinearVelocity().Length();
}

void Ball::onTouchMove(F32 x, F32 y) {
	b2Transform transform = boxBody->GetTransform();
	// flick (20 is eyeballed multiplier)
	boxBody->SetLinearVelocity(b2Vec2((x - transform.position.x) * 20, (y - transform.position.y) * 20) );
	transform.position.x = x;
	transform.position.y = y;
	boxBody->SetTransform(transform.position, transform.GetAngle());
	boxBody->SetAwake(true);
}
