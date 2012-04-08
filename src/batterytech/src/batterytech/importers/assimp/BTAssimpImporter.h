/*
 * BTAssimpImporter.h
 *
 *  Created on: Oct 12, 2011
 *      Author: rgreen
 */

#ifndef BTASSIMPIMPORTER_H_
#define BTASSIMPIMPORTER_H_

#include "../../../assimp/include/assimp.hpp"
#include "../../../assimp/include/aiScene.h"

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
#endif /* BTASSIMPIMPORTER_H_ */
