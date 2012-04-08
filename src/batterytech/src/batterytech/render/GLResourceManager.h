/*
 * GLResourceManager.h
 *
 *  Created on: May 16, 2011
 *      Author: rgreen
 */

#ifndef GLRESOURCEMANAGER_H_
#define GLRESOURCEMANAGER_H_

#include "../primitives.h"
#include "Texture.h"
#include "../util/HashTable.h"
#include "../util/ManagedArray.h"

namespace BatteryTech {

	class Context;
	class GLObjSceneBinding;
	class GLAssimpBinding;

	class GLResourceManager {
	public:
		GLResourceManager(Context *context);
		virtual ~GLResourceManager();
		void invalidateGL();
		void addTexture(const char *assetName, BOOL32 loadOnDemand=FALSE);
		void addTexture(Texture *texture);
		Texture* getTexture(const char *assetName);
		void removeTexture(const char *assetName);
		void clearTextures();
		void loadTextures();
		void unloadTextures();
		void addObjScene(const char *assetName);
		GLObjSceneBinding* getObjScene(const char *assetName);
		void removeObjScene(const char *assetName);
		void clearObjScenes();
		void loadObjScenes();
		void unloadObjScenes();
		// Open Asset Import Library support
		void addAssimp(const char *assetName);
		GLAssimpBinding* getAssimp(const char *assetName);
		void removeAssimp(const char *assetName);
		void clearAssimps();
		void loadAssimps();
		void unloadAssimps();
	private:
		Context *context;
		// array is used for serial-indexing, table is used for random access
		ManagedArray<Texture> *texArray;
		StrHashTable<Texture*> *texTable;
		// we need a GLModelBinding for each group of each obj
		ManagedArray<GLObjSceneBinding> *objSceneBindingArray;
		StrHashTable<GLObjSceneBinding*> *objSceneBindingTable;
		StrHashTable<GLAssimpBinding*> *assimpBindings;

	};

}
#endif /* GLRESOURCEMANAGER_H_ */
