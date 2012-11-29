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
// Name        : BTIOSystem.h
// Description : An Assimp IOSystem that can read assets through BatteryTech
//============================================================================

#ifndef BTIOSYSTEM_H_
#define BTIOSYSTEM_H_

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "../../../bt-assimp/include/IOSystem.h"

using namespace Assimp;

namespace BatteryTech {

class BTIOSystem : public IOSystem {
public:
	BTIOSystem(const char *basePath);
	virtual ~BTIOSystem();
	virtual bool Exists(const char* pFile) const;
	virtual char getOsSeparator() const;
	virtual IOStream* Open(const char* pFile, const char* pMode = "rb");
	virtual void Close(IOStream* pFile);
	virtual bool ComparePaths(const char* one, const char* second) const;
private:
	char *basePath;
};

}
#endif /* BTIOSYSTEM_H_ */

#endif
