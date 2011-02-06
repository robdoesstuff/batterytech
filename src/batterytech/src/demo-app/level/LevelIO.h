/*
 * LevelIO.h
 *
 *  Created on: Dec 1, 2010
 *      Author: rgreen
 */

#ifndef LEVELIO_H_
#define LEVELIO_H_

#include <batterytech/Context.h>
#include "../World.h"
#include <stdio.h>

class Level;
class World;

class LevelIO {
public:
	LevelIO(Context *context);
	virtual ~LevelIO();
	static void getDataDirPath(char* path);
	static void checkDataDir();
private:
	Context *context;
};

#endif /* LEVELIO_H_ */
