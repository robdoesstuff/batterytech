/*
 * BTIOSystem.h
 *
 *  Created on: Oct 12, 2011
 *      Author: rgreen
 */

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
