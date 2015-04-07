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
// Name        : BTAssimpImporter.h
// Description : Basic Assimp Model Importing Integration
//============================================================================

#ifndef BTASSIMPIMPORTER_H_
#define BTASSIMPIMPORTER_H_

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "../../../bt-assimp/include/assimp.hpp"
#include "../../../bt-assimp/include/aiScene.h"

namespace BatteryTech {

class BTAssimpImporter {
public:
	BTAssimpImporter();
	virtual ~BTAssimpImporter();
	Assimp::Importer* importAsset(const char* assetName);
	const char* getImportedAssetBasename() { return importedAssetBasename; }
private:
	void processNode(aiNode *node);
	char *importedAssetBasename;
};

}

#endif
#endif /* BTASSIMPIMPORTER_H_ */
