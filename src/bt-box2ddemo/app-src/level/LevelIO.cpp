/*
 * LevelIO.cpp
 *
 *  Created on: Dec 1, 2010
 *      Author: rgreen
 */

#include "LevelIO.h"
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Logger.h>
#include "../gameobject/GameObjectFactory.h"
#include <batterytech/util/TextFileUtil.h>
#include <string.h>

LevelIO::LevelIO(GameContext *context) {
	this->context = context;
}

LevelIO::~LevelIO() {
	context = NULL;
}

void LevelIO::getDataDirPath(char* path) {
	_platform_get_external_storage_dir_name(path, 255);
	strcat(path, _platform_get_path_separator());
	strcat(path, "demo-app-data");
}

void LevelIO::checkDataDir() {
	logmsg("External Storage Directory:");
	char dir[255];
	getDataDirPath(dir);
	logmsg(dir);
	if (!_platform_path_exists(dir)) {
		logmsg("Path does not exist.  Attempting to create");
		if (!_platform_path_create(dir)) {
			logmsg("Unable to create dir!");
		}
	} else {
		logmsg("Path exists.");
	}
	if (_platform_path_can_read(dir)) {
		logmsg("Can read from dir.");
	}
	if (_platform_path_can_write(dir)) {
		logmsg("Can write to dir.");
	}
	logmsg(dir);
}



