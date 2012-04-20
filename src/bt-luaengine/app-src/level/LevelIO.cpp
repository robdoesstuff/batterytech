/*
 * LevelIO.cpp
 *
 *  Created on: Dec 1, 2010
 *      Author: rgreen
 */

#include "LevelIO.h"
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Logger.h>
#include <batterytech/util/TextFileUtil.h>
#include <string.h>

LevelIO::LevelIO(Context *context) {
	this->context = context;
}

LevelIO::~LevelIO() {
	context = NULL;
}

void LevelIO::getDataDirPath(char* path) {
	_platform_get_application_storage_dir_name(path, 512);
}

void LevelIO::checkDataDir() {
	logmsg("Application Storage Directory:");
	char dir[1024];
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

BOOL32 LevelIO::createFileRecursive(const char *filename, char *path) {
	checkDataDir();
	getDataDirPath(path);
	strcat(path, _platform_get_path_separator());
	strcat(path, filename);
	if (!_platform_path_exists(path)) {
		char basepath[1024];
		_platform_get_basename(path, basepath);
		_platform_path_create_recursive(basepath);
	}
	return TRUE;
}


