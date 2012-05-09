/*
 * Game.h
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#ifndef GAME_H_
#define GAME_H_

#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/network/NetworkMessageListener.h>
#include <batterytech/math/Vector3.h>
#include "GameLuaBinder.h"
#include "GameConstants.h"
#include <batterytech/batterytech_entrypoints.h>

class LevelEditor;
class GameObject;
class World;
class LevelIO;
class TouchInputProcessor;
class GameContext;

namespace BatteryTech { class NetworkMessageFactory; }

using namespace BatteryTech;

class Game : public BTApplicationUpdater, NetworkMessageListener {
public:
	Game(GameContext *context);
	virtual ~Game();
	void update();
	void reset();
	void showError(const char* errorTitle, const char* errorMessage);
	void clearError();
	// called when a new network message has arrived.
	virtual void onNetworkMessage(NetworkMessage* message);
	// called when the network connection has disconnected.
	virtual void onNetworkDisconnected();
	GameLuaBinder *luaBinder;
	TouchInputProcessor *touchInputProcessor;
	BOOL32 needsReset;
	BOOL32 isInError;
private:
	GameContext *context;
	BOOL32 isFocused;
	BOOL32 initialized;
	GameObject* makeBox(Vector3f pos);
	void loadPreferences();
	void initializeLua();
	void updateState();
	void updateInput();
	void updateNetwork();
	World* getWorld();
};

#endif /* GAME_H_ */
