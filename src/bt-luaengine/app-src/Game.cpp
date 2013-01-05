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

#include "Game.h"
#include "GameContext.h"
#include "GameConstants.h"
#include "GameUtil.h"
#include <batterytech/batterytech.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/network/NetworkManager.h>
#include <batterytech/Context.h>
#include "World.h"
#include "gameobject/GameObject.h"
#include <batterytech/ui/Menu.h>
#include "menus/ErrorMenu.h"
#include <batterytech/util/PropertiesIO.h>
#include "render/WorldRenderer.h"
#include <string.h>
#include <batterytech/VibrationManager.h>
#include "level/LevelIO.h"
#include "level/Level.h"
#include "gameobject/GameObjectLuaBinder.h"
#include "input/TouchInputProcessor.h"
#include "ScreenControl.h"
#include <batterytech/util/strx.h>
#include <string.h>
#include <batterytech/render/QuadRenderer.h>
#include <batterytech/video/VideoManager.h>
#include "gameobject/ParticleEmitter.h"

#ifdef BATTERYTECH_INCLUDE_BOX2D
#include <bt-box2d/Dynamics/b2World.h>
#include <bt-box2d/Dynamics/Contacts/b2Contact.h>
#include <bt-box2d/Dynamics/Joints/b2Joint.h>
#endif

Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig) {
	return new GameContext(graphicsConfig);
}

Game::Game(GameContext *context) {
	this->context = context;
	this->context->world->lastGameState = GAMESTATE_NONE;
	this->context->world->gameState = GAMESTATE_LOADING;
	this->context->world->nextGameState = GAMESTATE_READY;
	initialized = FALSE;
	luaBinder = NULL;
	touchInputProcessor = new TouchInputProcessor();
	isInError = FALSE;
	needsReset = FALSE;
}

Game::~Game() {
	logmsg("Releasing Game");
	delete touchInputProcessor;
	touchInputProcessor = NULL;
	// the world will already be gone at this point.
	if (context && context->world && context->world->luaState) {
		LuaBinder::closeState(context->world->luaState);
		context->world->luaState = NULL;
	}
	initialized = FALSE;
	context = NULL;
	// context is freed by batterytech core
	// game objects are freed by world (which is freed by context)
	// menus are freed by uimanager
	delete luaBinder;
	luaBinder = NULL;
	needsReset = FALSE;
	logmsg("Done Releasing Game");
}

void Game::showError(const char* errorTitle, const char* errorMessage) {
	char buf[1024];
	sprintf(buf, "Error - %s, %s", errorTitle, errorMessage);
	logmsg(buf);
	if (!isInError) {
		// menu will delete this when its shown
		ErrorMessage *m = new ErrorMessage(errorTitle, errorMessage);
		context->uiManager->showMenu(ERROR_MENU_NAME, m);
	}
	isInError = TRUE;
}

void Game::clearError() {
	isInError = FALSE;
}

void Game::update() {
	if (!initialized || context->newGraphicsContext) {
		if (context->world->gameState != GAMESTATE_LOADING) {
			// if we're resuming, we want to go back to whatever state we were just in after we load.
			context->world->nextGameState = context->world->gameState;
			context->world->gameState = GAMESTATE_LOADING;
		}
	}
	if (context->world->gameState == GAMESTATE_LOADING && context->worldRenderer->isLoadingScreenDisplayed()) {
		if (!initialized) {
			// init error menu first in case there are problems running scripts
			context->uiManager->addMenu(new ErrorMenu(context));
		}
		if (!initialized || context->newGraphicsContext) {
			logmsg("Initializing Renderers");
			// init menu renderer first so we can expose the font to world renderer
			// (perhaps GLResourceManager should manage all fonts?)
			context->menuRenderer->init(TRUE);
			if (!initialized) {
				// always init lua after menu so that it can display error messages
				initializeLua();
			}
            // lua must be before world so that resources can be marked for loading
			context->worldRenderer->init(TRUE);
			context->newGraphicsContext = FALSE;
		} else {
			context->worldRenderer->init(FALSE);
		}
		// load any pending sounds
		S32 soundsPending = context->world->soundsPendingLoad->getSize();
		for (S32 i = 0; i < soundsPending; i++) {
			char* assetName = context->world->soundsPendingLoad->array[i];
			context->audioManager->loadSound(assetName);
		}
		if (soundsPending) {
			context->world->soundsPendingLoad->deleteElements();
		}
        initialized = TRUE;
 	}
	updateInput();
	updateState();
	// updateNetwork();
	if (needsReset) {
		reset();
		needsReset = FALSE;
	}
	if (isInError) {
		lua_settop(luaBinder->L, 0);
		// Don't do anything with lua but keep the stack clear
		return;
	}
	if (luaBinder && context->world->gameState == GAMESTATE_RUNNING) {
        if (context->callbackDataReady) {
            //context->callbackData;
            luaBinder->callGlobalVA("callback", "s>", context->callbackData);
            if (strStartsWith(context->callbackData, "purchaseSucceeded")) {
                // deprecated!
                char *tok = strtok(context->callbackData, " ");
                tok = strtok(NULL, " ");
                if (tok) {
                    luaBinder->callGlobalVA("purchaseResult", "bs>", TRUE, tok);
                }
            } else if (strStartsWith(context->callbackData, "purchaseFailed")) {
                // deprecated!
                char *tok = strtok(context->callbackData, " ");
                tok = strtok(NULL, " ");
                if (tok) {
                    luaBinder->callGlobalVA("purchaseResult", "bs>", FALSE, tok);
                }
            }
            context->callbackDataReady = FALSE;
            context->callbackData[0] = '\0';
        }
		luaBinder->update();
        // safety sweep
        removeInactiveGameObjects();
        HashTable<S32, ParticleEmitter*> *emitters = context->world->emitters;
		for (HashTable<S32,ParticleEmitter*>::Iterator i = emitters->getIterator(); i.hasNext;) {
			emitters->getNext(i)->update(context->tickDelta);
		}
	}
	context->videoManager->update(context->tickDelta);
}

void Game::onSuspend() {
	if (luaBinder) {
		luaBinder->callGlobalVA("onSuspend", "");
	}
}

void Game::initializeLua() {
	logmsg("Initializing Lua");
	context->world->luaState = LuaBinder::newState(context);
	luaBinder = new GameLuaBinder(context->world->luaState, this, context);
	GameLuaBinder::luaRegister(context->world->luaState, context);
	GameObjectLuaBinder::luaRegister(context->world->luaState, context);
	// run main script (it should run all subsequent scripts)
	luaBinder->execScript("scripts/main.lua");
	luaBinder->init();
}

void Game::reset() {
	logmsg("Resetting Game");
	context->uiManager->clearMenuStack();
	getWorld()->clear();
	getWorld()->screenControls->deleteElements();
	if (context && context->world && context->world->luaState) {
		LuaBinder::closeState(context->world->luaState);
		context->world->luaState = NULL;
	}
	clearError();
	delete luaBinder;
	luaBinder = NULL;
	context->quadRenderer->init(FALSE);
	context->worldRenderer->init(FALSE);
	context->menuRenderer->init(FALSE);
	initializeLua();
	this->context->world->gameState = GAMESTATE_READY;
	logmsg("Done Resetting Game");
}

void Game::updateState() {
	BOOL32 stateChanged = FALSE;
	U8 gameState = getWorld()->gameState;
	if (getWorld()->lastGameState != gameState) {
		stateChanged = TRUE;
		// reset the tickDelta to 0 to avoid bad timings in first update after load
		btClearTickDeltas();
	}
	if (gameState == GAMESTATE_READY && stateChanged) {
		World *world = getWorld();
		logmsg("Clearing World");
		world->clear();
		logmsg("Done cleaning up.");
		context->world->gameState = GAMESTATE_RUNNING;
		if (!isInError) {
			context->uiManager->clearMenuStack();
			luaBinder->reset(TRUE);
		}
		// normally we'd show the top menu here
		// context->uiManager->showMenu(TOP_MENU_NAME);
		//getWorld()->gameState = GAMESTATE_LOADING;
	} else if (gameState == GAMESTATE_RUNNING) {
		// lua will take it from here.
	} else if (gameState == GAMESTATE_LOADING) {
		if (stateChanged) {
			getWorld()->renderersReady = FALSE;
		}
		if (getWorld()->renderersReady) {
			getWorld()->gameState = getWorld()->nextGameState;
		}
	}
	if (stateChanged) {
		getWorld()->lastGameState = gameState;
	}
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

void Game::updatePhysics(F32 updateDelta, S32 substeps1, S32 substeps2) {
	World *world = getWorld();
	if (world->gameState == GAMESTATE_RUNNING && getWorld()->simEnabled) {
		// process any removals from lua before updating physics model and running collisions
		removeInactiveGameObjects();
#ifdef BATTERYTECH_INCLUDE_BOX2D
		b2World *boxWorld = getWorld()->boxWorld;
        if (boxWorld) {
            boxWorld->Step(updateDelta, substeps1, substeps2);
         }
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
		world->btProfiler->reset();
		world->btWorld->stepSimulation(updateDelta, substeps1);
		world->physicsUpdateTime = world->btProfiler->getTimeMilliseconds();
		for (S32 i = 0; i < world->gameObjects->getSize(); i++) {
			world->gameObjects->array[i]->startCollisionPhase();
		}
		btDispatcher *dispatcher = world->btWorld->getDispatcher();
		S32 manifolds = dispatcher->getNumManifolds();
		for (S32 i = 0; i < manifolds; i++) {
			btPersistentManifold *contactManifold = dispatcher->getManifoldByIndexInternal(i);
			if (contactManifold->getNumContacts() == 0) {
				continue;
			}
			btCollisionObject *objA = static_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject *objB = static_cast<btCollisionObject*>(contactManifold->getBody1());
			PhysicsBodyObject *physicsObjA = (PhysicsBodyObject*)objA->getUserPointer();
			PhysicsBodyObject *physicsObjB = (PhysicsBodyObject*)objB->getUserPointer();
			if (physicsObjA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT && physicsObjB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
				// logmsg("GameObject Collision");
				// both must be active!
				if (((GameObject*)physicsObjA)->isActive && ((GameObject*)physicsObjB)->isActive) {
					((GameObject*)physicsObjA)->onCollision((GameObject*)physicsObjB);
					((GameObject*)physicsObjB)->onCollision((GameObject*)physicsObjA);
				}
			} else if (physicsObjA->bodyType == PHYSICS_BODY_TYPE_LEVEL && physicsObjB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
				if (((GameObject*)physicsObjB)->isActive) {
					((GameObject*)physicsObjB)->onLevelCollision();
				}
			} else if (physicsObjB->bodyType == PHYSICS_BODY_TYPE_LEVEL && physicsObjA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
				if (((GameObject*)physicsObjA)->isActive) {
					((GameObject*)physicsObjA)->onLevelCollision();
				}
			}
			/*
             int numContacts = contactManifold->getNumContacts();
             for (int j=0;j<numContacts;j++) {
             btManifoldPoint& pt = contactManifold->getContactPoint(j);
             if (pt.getDistance() < 0.0f) {
             const btVector3& ptA = pt.getPositionWorldOnA();
             const btVector3& ptB = pt.getPositionWorldOnB();
             const btVector3& normalOnB = pt.m_normalWorldOnB;
             }
             }
             */
		}
		for (S32 i = 0; i < world->gameObjects->getSize(); i++) {
			world->gameObjects->array[i]->endCollisionPhase();
		}
#endif
#if defined(BATTERYTECH_INCLUDE_BOX2D) || defined(BATTERYTECH_INCLUDE_BULLET)
		// physics callbacks could have invalidated objects.
        removeInactiveGameObjects();
#endif
		updateGameObjects(); // now that things have moved, update gameobjects so render is correct (also check for collision deactivations before render)
	} else {
		getWorld()->physicsUpdateTime = 0;
	}
}

void Game::updateGameObjects() {
	ManagedArray<GameObject> *objects = context->world->gameObjects;
	for (S32 i = 0; i < objects->getSize(); i++) {
		if (objects->array[i]->isActive) {
			objects->array[i]->update();
		}
	}
}

void Game::removeInactiveGameObjects() {
	ManagedArray<GameObject> *objects = context->world->gameObjects;
	for (S32 i = 0; i < objects->getSize(); i++) {
		if (!objects->array[i]->isActive) {
			GameObject *obj = objects->remove(i);
            for (S32 j = 0; j < objects->getSize(); j++) {
                if (objects->array[j]->isActive) {
                    objects->array[j]->onOtherGameObjectRemoved(obj);
                }
            }
			delete obj;
			--i; // move i back one to recheck this slot now that a new element may be in it
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

World* Game::getWorld() {
	return context->world;
}

#ifdef BATTERYTECH_INCLUDE_BOX2D
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
        // actually points to the value of the key held in the joints table
        S32 jointId = (S32)(size_t)joint->GetUserData();
        world->boxJoints->remove(jointId);
	}
}

void Game::SayGoodbye(b2Fixture* fixture) {
	/*
	if (context->world) {
		World *world = context->world;
		for (S32 i = 0; i < world->gameObjects->getSize(); i++) {
			world->gameObjects->array[i]->SayGoodbye(fixture);
		}
	}
	*/
}
#endif
