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

//============================================================================
// Name        : BTIOSystem.cpp
// Description : An Assimp IOSystem that can read assets through BatteryTech
//============================================================================

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "BTIOSystem.h"
#include "../../platform/platformgeneral.h"
#include "../../util/strx.h"
#include "BTMemoryIOStream.h"

namespace BatteryTech {

BTIOSystem::BTIOSystem(const char *basePath) {
	this->basePath = strDuplicate(basePath);
}

BTIOSystem::~BTIOSystem() {
	delete [] this->basePath;
}

bool BTIOSystem::Exists(const char* pFile) const {
	return true;
}

char BTIOSystem::getOsSeparator() const {
	return _platform_get_path_separator()[0];
}

IOStream* BTIOSystem::Open(const char* pFile, const char* pMode) {
	S32 size;
	char fullPath[1024];
	strcpy(fullPath, basePath);
	strcat(fullPath, _platform_get_path_separator());
	strcat(fullPath, pFile);
	unsigned char *data = _platform_load_asset(fullPath, &size);
	if (data) {
		return new BTMemoryIOStream(data, size);
	} else {
		return NULL;
	}
}

void BTIOSystem::Close(IOStream* pFile) {
	delete pFile;
}

bool BTIOSystem::ComparePaths(const char* one, const char* second) const {
	return strEquals(one, second);
}

}

#endif
