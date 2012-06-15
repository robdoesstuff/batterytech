/*
 * Game.cpp
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#include "Game.h"
#include "GameContext.h"
#include "GameConstants.h"
#include "GameUtil.h"
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

void Game::loadPreferences() {
	PropertiesIO propsIO;
	char path[1024];
	LevelIO::getDataDirPath(path);
	strcat(path, _platform_get_path_separator());
	strcat(path, "settings.txt");
	if (_platform_path_exists(path)) {
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
		}
		*/
	} else {
		//logmsg("No settings file found.  Using defaults.");
	}
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
			// load preferences
			loadPreferences();
			initializeLua();
		}
		if (!initialized || context->newGraphicsContext) {
			logmsg("Initializing Renderers");
			context->worldRenderer->init(TRUE);
			context->menuRenderer->init(TRUE);
			context->newGraphicsContext = FALSE;
			initialized = TRUE;
		} else {
			context->worldRenderer->init(FALSE);
		}
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
	if (context->callbackDataReady) {
		//context->callbackData;
		if (strStartsWith(context->callbackData, "purchaseSucceeded")) {
			char *tok = strtok(context->callbackData, " ");
			tok = strtok(NULL, " ");
			if (tok) {
				luaBinder->callGlobalVA("purchaseResult", "bs>", TRUE, tok);
			}
		} else if (strStartsWith(context->callbackData, "purchaseFailed")) {
			char *tok = strtok(context->callbackData, " ");
			tok = strtok(NULL, " ");
			if (tok) {
				luaBinder->callGlobalVA("purchaseResult", "bs>", FALSE, tok);
			}
		} else {
			luaBinder->callGlobalVA("callback", "s>", context->callbackData);
		}
		context->callbackDataReady = FALSE;
		context->callbackData[0] = '\0';
	}
	if (luaBinder && context->world->gameState == GAMESTATE_RUNNING) {
		luaBinder->update();
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
	}
	if (gameState == GAMESTATE_READY && stateChanged) {
		World *world = getWorld();
		logmsg("Clearing World");
		world->clear();
		logmsg("Done cleaning up.");
		context->world->gameState = GAMESTATE_RUNNING;
		if (!isInError) {
			luaBinder->reset(TRUE);
			context->uiManager->clearMenuStack();
			context->game->luaBinder->setMode(GAMEMODE_MAINMENU);
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
			// reset the tickDelta to 0 to avoid bad timings in first update after load
			context->tickDelta = 0;
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
