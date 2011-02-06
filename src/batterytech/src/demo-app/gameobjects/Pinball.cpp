/*
 * Pinball.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#include "Pinball.h"
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <string.h>

// static field initialization
GameObjectMeta* Pinball::meta = NULL;

Pinball::Pinball(Context *context) : GameObject(context) {
}

Pinball::Pinball(Context *context, F32 x, F32 y, F32 angle) : GameObject(context) {
	init(x, y, angle);
}

Pinball::~Pinball() {
	destroyBody();
}

void Pinball::init(F32 x, F32 y, F32 angle) {
	type = GAMEOBJECT_TYPE_PINBALL;
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
	dCircle.m_radius = PINBALL_RADIUS;
	b2FixtureDef cFixtureDef;
	cFixtureDef.shape = &dCircle;
	cFixtureDef.density = 2.0f;
	cFixtureDef.friction = 0.1f;
	cFixtureDef.restitution = 0.21f;
	cFixtureDef.filter.categoryBits = COLLISION_BITS_PINBALL;
	boxBody->CreateFixture(&cFixtureDef);
	boxBody->SetUserData(this);
	wasInPlay = FALSE;
}

GameObjectMeta* Pinball::getMeta() {
	if (!meta) {
		meta = new GameObjectMeta(1);
	}
	return meta;
}

void Pinball::update() {
	b2Vec2 pos = boxBody->GetPosition();
	x = pos.x;
	y = pos.y;
	angle = boxBody->GetAngle();
	clearImpact();
	if (x > WORLD_LEFT && x < WORLD_RIGHT && y > WORLD_BOTTOM && y < WORLD_TOP) {
		// flag used for MP ball passing off-screen
		wasInPlay = TRUE;
	}
}

F32 Pinball::getLinearVelocity() {
	return boxBody->GetLinearVelocity().Length();
}

void Pinball::onTouchMove(F32 x, F32 y) {
	b2Transform transform = boxBody->GetTransform();
	transform.position.x = x;
	transform.position.y = y;
	boxBody->SetTransform(transform.position, transform.GetAngle());
	boxBody->SetAwake(true);
}
