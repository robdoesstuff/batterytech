/*
 * Game.cpp
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#include "Game.h"
#include "GameConstants.h"
#include "GameUtil.h"
#include <bt-box2d/Dynamics/b2World.h>
#include <bt-box2d/Dynamics/Contacts/b2Contact.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/network/NetworkManager.h>
#include "GameContext.h"
#include "World.h"
#include "gameobject/GameObject.h"
#include "gameobjects/Ball.h"
#include <batterytech/ui/Menu.h>
#include "input/TouchInputProcessor.h"
#include "menus/TopMenu.h"
#include "menus/MenuButtonMenu.h"
#include "menus/GameOptionsMenu.h"
#include "menus/SettingsMenu.h"
#include "menus/SoundcheckMenu.h"
#include <batterytech/util/PropertiesIO.h>
#include "render/WorldRenderer.h"
#include <string.h>
#include <batterytech/VibrationManager.h>
#include "level/LevelIO.h"
#include <batterytech/render/GLResourceManager.h>

Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig) {
	return new GameContext(graphicsConfig);
}

Game::Game(GameContext *context) {
	this->context = context;
	this->context->world->lastGameState = GAMESTATE_NONE;
	this->context->world->gameState = GAMESTATE_NONE;
	touchInputProcessor = new TouchInputProcessor();
	initialized = FALSE;
}

Game::~Game() {
	logmsg("Releasing Game");
	initialized = FALSE;
	context = NULL;
	// context is freed by batterytech core
	// game objects are freed by world (which is freed by context)
	// menus are freed by uimanager
	if (touchInputProcessor) {
		logmsg("Releasing Touch Input Processor");
		delete touchInputProcessor;
	}
	touchInputProcessor = NULL;
	logmsg("Done Releasing Game");
}

void Game::loadPreferences() {
	PropertiesIO propsIO;
	char path[512];
	LevelIO::getDataDirPath(path);
	strcat(path, _platform_get_path_separator());
	strcat(path, "settings.txt");
	if (_platform_path_exists(path)) {
		// TODO - fix property loading
		/*
		ManagedArray<Property> *props = propsIO.loadPropertiesFromFile(path);
		// parse props
		if (props) {
			for (S32 i = 0; i < props->getSize(); i++) {
				Property *prop = props->array[i];
				if (strcmp(prop->getName(), PREF_SOUND_ENABLED) == 0) {
					if (strcmp(prop->getValue(), "1") == 0) {
						context->audioManager->setEnabled(TRUE);
					} else {
						context->audioManager->setEnabled(FALSE);
					}
				} else if (strcmp(prop->getName(), PREF_VIBES_ENABLED) == 0) {
					if (strcmp(prop->getValue(), "1") == 0) {
						context->vibrationManager->setEnabled(TRUE);
					} else {
						context->vibrationManager->setEnabled(FALSE);
					}
				} else if (strcmp(prop->getName(), PREF_SHOW_FPS) == 0) {
					if (strcmp(prop->getValue(), "1") == 0) {
						context->showFPS = TRUE;
					} else {
						context->showFPS = FALSE;
					}
				}
			}
		}*/
	} else {
		logmsg("No settings file found.  Using defaults.");
	}
}

void Game::update() {
	updateInput();
	updateState();
	updateNetwork();
	if (getWorld()->gameState == GAMESTATE_RUNNING) {
		// this will skip the giant tick delta that follows a suspend
		updatePhysics();
		updateGameObjects();
		updateLevel();
	}
	if (!initialized) {
		// load preferences
		loadPreferences();
		logmsg("Initializing Menus");
		context->uiManager->addMenu(new TopMenu(context));
		context->uiManager->addMenu(new MenuButtonMenu(context));
		context->uiManager->addMenu(new GameOptionsMenu(context));
		context->uiManager->addMenu(new SettingsMenu(context));
		context->uiManager->addMenu(new SoundcheckMenu(context));
		// load sounds
		logmsg("Loading Sounds");
		context->audioManager->loadSound("click.ogg");
		this->context->world->gameState = GAMESTATE_READY;
		//context->audioManager->playStreamingSound("song1.ogg", -1, 1.0f, 1.0f, 1.0f);
	}
	if (!initialized || context->newGraphicsContext) {
		logmsg("Initializing Renderers");
		// init menu renderer first so we can expose the font to world renderer
		// (perhaps GLResourceManager should manage all fonts?)
		context->menuRenderer->init(TRUE);
		context->worldRenderer->init(TRUE);
		context->newGraphicsContext = FALSE;
		initialized = TRUE;
	}
}

void Game::onSuspend() {
	// about to be suspended
}

void Game::updateState() {
	BOOL32 stateChanged = FALSE;
	U8 gameState = getWorld()->gameState;
	if (getWorld()->lastGameState != gameState) {
		stateChanged = TRUE;
	}
	if (gameState == GAMESTATE_READY) {
		getWorld()->gameState = GAMESTATE_LOADING;
	} else if (gameState == GAMESTATE_RUNNING) {
		// running state is level loaded and playing a game.
		if (stateChanged) {
			//context->uiManager->clearMenuStack();
			logmsg("Showing top menu");
			context->uiManager->showMenu(TOP_MENU_NAME);
		}
		// watch for state change conditions
	} else if (gameState == GAMESTATE_LOADING) {
		if (stateChanged) {
			getWorld()->renderersReady = FALSE;
			loadLevel();
		}
		if (getWorld()->renderersReady && getWorld()->levelLoaded) {
			initializeNewGame();
			getWorld()->gameState = GAMESTATE_RUNNING;
		}
	}
	if (stateChanged) {
		getWorld()->lastGameState = gameState;
	}
}

void Game::updatePhysics() {
	World *world = getWorld();
	if (world->gameState == GAMESTATE_RUNNING) {
		world->boxWorld->SetGravity(b2Vec2(-context->accelerometerState.x * 3, -context->accelerometerState.y * 3));
		//world->physicsTimeRemainder += context->tickDelta;
		b2World *boxWorld = getWorld()->boxWorld;
		boxWorld->Step(context->tickDelta, BOX2D_VELOCITY_ITERATIONS, BOX2D_POSITION_ITERATIONS);
		/*
		while (world->physicsTimeRemainder - PHYSICS_TIMESTEP > 0) {
			world->physicsTimeRemainder -= PHYSICS_TIMESTEP;
			boxWorld->Step(PHYSICS_TIMESTEP, BOX2D_VELOCITY_ITERATIONS, BOX2D_POSITION_ITERATIONS);
		}*/
		boxWorld->ClearForces();
	}
}

void Game::updateGameObjects() {
	World *world = context->world;
	updateGameObjectArray(world->gameObjects);
}

void Game::updateGameObjectArray(ManagedArray<GameObject> *objects) {
	// two pass, update then remove
	S32 i;
	for (i = 0; i < objects->getSize(); i++) {
		objects->array[i]->update();
	}
	for (i = 0; i < objects->getSize(); i++) {
		if (!objects->array[i]->isActive) {
			GameObject *obj = objects->remove(i);
			delete obj;
			--i; // move i back one to recheck this slot now that a new element may be in it
		}
	}
}

void Game::updateLevel() {
}

void Game::updateNetwork() {
	context->networkManager->update();
}

void Game::updateInput() {
	if (context->isUIConsumingPointers) {
		return;
	}
	touchInputProcessor->processInput(context);
}


void Game::initializeNewGame() {
	World *world = getWorld();
	logmsg("Initializing New Game");
	// make an assumption before first update
	context->accelerometerState.x = 0;
	context->accelerometerState.y = -WORLD_GRAVITY;
	context->accelerometerState.z = 0;
}

void Game::loadLevel() {
	World *world = getWorld();
	// clean up last game
	//logmsg("Stopping all sounds");
	//context->audioManager->stopAllSounds();
	//logmsg("Stopping all vibes");
	//_platform_stop_all_vibration_effects();
	logmsg("Clearing World");
	world->clear();
	logmsg("Done cleaning up.");
	// done cleaning up
	b2Vec2 gravity(0.0f, WORLD_GRAVITY);
	world->boxWorld = new b2World(gravity);
	world->boxWorld->SetContinuousPhysics(true);
	world->boxWorld->SetContactListener(this);
	world->boxWorld->SetDestructionListener(this);
	for (S32 i = 1; i < WORLD_RIGHT / 3; i++) {
		for (S32 j = 1; j < WORLD_TOP / 3; j++) {
			world->gameObjects->add(new Ball(context, i * 3, j * 3, 0));
		}
	}
	b2BodyDef triDef;
	//triDef.type = b2_dynamicBody;
	b2Body* triBody = world->boxWorld->CreateBody(&triDef);
	b2PolygonShape triShape;
	// left wall
	triShape.SetAsBox(0, WORLD_TOP-WORLD_BOTTOM, b2Vec2(WORLD_LEFT, 0), 0);
	//triShape.SetAsEdge(b2Vec2(WORLD_LEFT, WORLD_TOP), b2Vec2(WORLD_LEFT, WORLD_BOTTOM));
	b2FixtureDef triFixtureDef;
	triFixtureDef.shape = &triShape;
	triFixtureDef.friction = 1.0f;
	triFixtureDef.filter.categoryBits = COLLISION_BITS_STATIC;
	//triFixtureDef.density = 0.1f;
	triBody->CreateFixture(&triFixtureDef);
	// bottom wall
	triShape.SetAsBox(WORLD_RIGHT-WORLD_LEFT, 0, b2Vec2(0, WORLD_BOTTOM), 0);
	// triShape.SetAsEdge(b2Vec2(WORLD_LEFT, WORLD_BOTTOM), b2Vec2(WORLD_RIGHT, WORLD_BOTTOM));
	triBody->CreateFixture(&triFixtureDef);
	// right wall
	triShape.SetAsBox(0, WORLD_TOP-WORLD_BOTTOM, b2Vec2(WORLD_RIGHT, 0), 0);
	// triShape.SetAsEdge(b2Vec2(WORLD_RIGHT, WORLD_BOTTOM), b2Vec2(WORLD_RIGHT, WORLD_TOP));
	triBody->CreateFixture(&triFixtureDef);
	// top wall
	triShape.SetAsBox(WORLD_RIGHT-WORLD_LEFT, 0, b2Vec2(0, WORLD_TOP), 0);
	// triShape.SetAsEdge(b2Vec2(WORLD_RIGHT, WORLD_TOP), b2Vec2(WORLD_LEFT, WORLD_TOP));
	triBody->CreateFixture(&triFixtureDef);
	world->levelLoaded = TRUE;
}

/// Called when two fixtures begin to touch.
void Game::BeginContact(b2Contact* contact) {
	if (contact->GetFixtureA()->GetBody()->GetUserData() && contact->GetFixtureB()->GetBody()->GetUserData()) {
		PhysicsBodyObject *o1 = (PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
		PhysicsBodyObject *o2 = (PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
		if (o1->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureA()->GetBody()->GetUserData())->contactStarted(contact);
		}
		if (o2->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureB()->GetBody()->GetUserData())->contactStarted(contact);
		}
	}
}

/// Called when two fixtures cease to touch.
void Game::EndContact(b2Contact* contact) {
	if (contact->GetFixtureA()->GetBody()->GetUserData() && contact->GetFixtureB()->GetBody()->GetUserData()) {
		PhysicsBodyObject *o1 = (PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
		PhysicsBodyObject *o2 = (PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
		if (o1->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureA()->GetBody()->GetUserData())->contactEnded(contact);
		}
		if (o2->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureB()->GetBody()->GetUserData())->contactEnded(contact);
		}
	}
}

void Game::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	if (contact->GetFixtureA()->GetBody()->GetUserData() && contact->GetFixtureB()->GetBody()->GetUserData()) {
		PhysicsBodyObject *o1 = (PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
		PhysicsBodyObject *o2 = (PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
		if (o1->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureA()->GetBody()->GetUserData())->contactPreSolve(contact, oldManifold);
		}
		if (o2->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureB()->GetBody()->GetUserData())->contactPreSolve(contact, oldManifold);
		}
	}
}

void Game::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
	if (contact->GetFixtureA()->GetBody()->GetUserData() && contact->GetFixtureB()->GetBody()->GetUserData()) {
		PhysicsBodyObject *o1 = (PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
		PhysicsBodyObject *o2 = (PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
		if (o1->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureA()->GetBody()->GetUserData())->contactPostSolve(contact, impulse);
		}
		if (o2->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			((GameObject*) contact->GetFixtureB()->GetBody()->GetUserData())->contactPostSolve(contact, impulse);
		}
	}
}

void Game::SayGoodbye(b2Joint* joint) {
	if (context->world) {
		World *world = context->world;
		for (S32 i = 0; i < world->gameObjects->getSize(); i++) {
			world->gameObjects->array[i]->SayGoodbye(joint);
		}
	}
}

void Game::SayGoodbye(b2Fixture* fixture) {
	if (context->world) {
		World *world = context->world;
		for (S32 i = 0; i < world->gameObjects->getSize(); i++) {
			world->gameObjects->array[i]->SayGoodbye(fixture);
		}
	}
}

// called when a new network message has arrived.
void Game::onNetworkMessage(NetworkMessage* message) {
}

// called when the network connection has disconnected.
void Game::onNetworkDisconnected() {
	getWorld()->gameState = GAMESTATE_READY;
}

F32 Game::screenToWorldX(S32 screenX) {
	return GameUtil::screenToWorldX(screenX, this->context);
}

F32 Game::screenToWorldY(S32 screenY) {
	return GameUtil::screenToWorldY(screenY, this->context);
}

World* Game::getWorld() {
	return context->world;
}
