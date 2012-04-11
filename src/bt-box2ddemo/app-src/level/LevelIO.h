/*
 * LevelIO.h
 *
 *  Created on: Dec 1, 2010
 *      Author: rgreen
 */

#ifndef LEVELIO_H_
#define LEVELIO_H_

#include "../GameContext.h"
#include "../World.h"
#include <stdio.h>

using namespace BatteryTech;

class Level;
class World;

class LevelIO {
public:
	LevelIO(GameContext *context);
	virtual ~LevelIO();
	static void getDataDirPath(char* path);
	static void checkDataDir();
private:
	GameContext *context;
};

#endif /* LEVELIO_H_ */
