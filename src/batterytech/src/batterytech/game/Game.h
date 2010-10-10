/*
 * Game.h
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#ifndef GAME_H_
#define GAME_H_

#include "../Context.h"

// cross-references
class Context;

class Game {
public:
	Game(Context *context);
	virtual ~Game();
	virtual void update();
private:
	Context *context;
};

#endif /* GAME_H_ */
