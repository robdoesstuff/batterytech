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

#ifndef LEVELIO_H_
#define LEVELIO_H_

#include <batterytech/Context.h>
#include "../World.h"
#include <stdio.h>

using namespace BatteryTech;

class Level;
class World;

class LevelIO {
public:
	LevelIO(Context *context);
	virtual ~LevelIO();
	static void getDataDirPath(char* path);
	static void checkDataDir();
	static BOOL32 createFileRecursive(const char *filename, char *path);
private:
	Context *context;
};

#endif /* LEVELIO_H_ */
