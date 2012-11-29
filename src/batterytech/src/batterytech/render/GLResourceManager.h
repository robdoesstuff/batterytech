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
// Name        : GLResourceManager.h
// Description : An OpenGL resource manager, manages textures and VBO-bound objects
//============================================================================

#ifndef GLRESOURCEMANAGER_H_
#define GLRESOURCEMANAGER_H_

#include "../primitives.h"
#include "Texture.h"
#include "../util/HashTable.h"
#include "../util/ManagedArray.h"

namespace BatteryTech {

	class Context;
	class GLObjSceneBinding;
	class ShaderProgram;

#ifdef BATTERYTECH_INCLUDE_ASSIMP
	class GLAssimpBinding;
#endif

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
		void addShaderProgram(const char *tag, ShaderProgram *program);
		ShaderProgram* getShaderProgram(const char *tag);
		void removeShaderProgram(const char *tag);
		void clearShaderPrograms();
		void loadShaderPrograms();
		void unloadShaderPrograms();
#ifdef BATTERYTECH_INCLUDE_ASSIMP
		// Open Asset Import Library support
		void addAssimp(const char *assetName);
		GLAssimpBinding* getAssimp(const char *assetName);
		void removeAssimp(const char *assetName);
		void clearAssimps();
		void loadAssimps();
		void unloadAssimps();
#endif
	private:
		Context *context;
		// array is used for serial-indexing, table is used for random access
		ManagedArray<Texture> *texArray;
		StrHashTable<Texture*> *texTable;
		// we need a GLModelBinding for each group of each obj
		ManagedArray<GLObjSceneBinding> *objSceneBindingArray;
		StrHashTable<GLObjSceneBinding*> *objSceneBindingTable;
		StrHashTable<ShaderProgram*> *shaderPrograms;
#ifdef BATTERYTECH_INCLUDE_ASSIMP
		StrHashTable<GLAssimpBinding*> *assimpBindings;
#endif
	};

}
#endif /* GLRESOURCEMANAGER_H_ */
