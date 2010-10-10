/*
 * Game.h
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#ifndef GAME_H_
#define GAME_H_

#include "../Context.h"
#include "../primitives.h"

// cross-references
class Context;

class Game {
public:
	Game(Context *context);
	virtual ~Game();
	virtual void update();
private:
	Context *context;
	BOOL32 isFocused;
};

#endif /* GAME_H_ */
