/*
 * GameContext.h
 *
 *  Created on: Apr 9, 2012
 *      Author: rgreen
 */

#ifndef GAMECONTEXT_H_
#define GAMECONTEXT_H_

#include <batterytech/Context.h>

using namespace BatteryTech;

class Game;
class World;
class WorldRenderer;

class GameContext : public Context {
public:
	GameContext(GraphicsConfiguration *gConfig);
	virtual ~GameContext();
	Game *game;
	World *world;
	WorldRenderer *worldRenderer;
};

#endif /* GAMECONTEXT_H_ */
