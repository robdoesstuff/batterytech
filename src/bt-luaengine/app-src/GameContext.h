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
