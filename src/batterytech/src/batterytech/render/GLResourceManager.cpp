/*
 * GLResourceManager.cpp
 *
 *  Created on: May 16, 2011
 *      Author: rgreen
 */

#include "GLResourceManager.h"
#include "GLObjSceneBinding.h"
#include "GLAssimpBinding.h"
#include "../Logger.h"
#include <stdio.h>

#define MAX_TEXTURES 300
#define MAX_OBJSCENES 100
#define MAX_ASSIMPS 100

namespace BatteryTech {

	GLResourceManager::GLResourceManager(Context *context) {
		this->context = context;
		texArray = new ManagedArray<Texture>(MAX_TEXTURES);
		texTable = new StrHashTable<Texture*>((int)(MAX_TEXTURES * 1.3f));
		objSceneBindingArray = new ManagedArray<GLObjSceneBinding>(MAX_OBJSCENES);
		objSceneBindingTable = new StrHashTable<GLObjSceneBinding*>((int)(MAX_OBJSCENES * 1.3f));
		assimpBindings = new StrHashTable<GLAssimpBinding*>(MAX_ASSIMPS * 1.3f);
	}

	GLResourceManager::~GLResourceManager() {
		texArray->deleteElements();
		texTable->clear();
		delete texArray;
		delete texTable;
		texArray = NULL;
		texTable = NULL;
		objSceneBindingArray->deleteElements();
		objSceneBindingTable->clear();
		delete objSceneBindingArray;
		delete objSceneBindingTable;
		objSceneBindingArray = NULL;
		objSceneBindingTable = NULL;
		assimpBindings->deleteElements();
		delete assimpBindings;
		assimpBindings = NULL;
		context = NULL;
	}

	void GLResourceManager::invalidateGL() {
		//is called from renderer when context was killed.
		for (StrHashTable<Texture*>::Iterator iter = texTable->getIterator(); iter.hasNext;) {
			Texture *tex = texTable->getNext(iter);
			tex->invalidateGL();
		}
	}

	void GLResourceManager::addTexture(const char *assetName, BOOL32 loadOnDemand) {
		if (!texTable->contains(assetName)) {
			Texture *texture = new Texture(context, assetName, loadOnDemand);
			texArray->add(texture);
			// use the texture-allocated string as key, texture itself will clean it up (removal should be synchronized)
			texTable->put(texture->assetName, texture);
			char buf[255];
			sprintf(buf, "GLResourceManager: %d textures (%s) %s", texArray->getSize(), assetName, (loadOnDemand ? "OnDemand" : ""));
			logmsg(buf);
		}
	}

	void GLResourceManager::addTexture(Texture *texture) {
		if (!texTable->contains(texture->assetName)) {
			texArray->add(texture);
			// use the texture-allocated string as key, texture itself will clean it up (removal should be synchronized)
			texTable->put(texture->assetName, texture);
		}
	}

	Texture* GLResourceManager::getTexture(const char *assetName) {
		return texTable->get(assetName);
	}

	void GLResourceManager::removeTexture(const char *assetName) {
		// TODO - unload if active
		Texture *texture = texTable->remove(assetName);
		if (texture) {
			texArray->remove(texture);
		}
	}

	void GLResourceManager::clearTextures() {
		// TODO - if any textures are loaded into current context, we need to unload them first!
		texArray->deleteElements();
		texArray->clear();
		texTable->clear();
	}

	void GLResourceManager::loadTextures() {
		for (S32 i = 0; i < texArray->getSize(); i++) {
			texArray->array[i]->load();
		}
	}

	void GLResourceManager::unloadTextures() {
		for (S32 i = 0; i < texArray->getSize(); i++) {
			texArray->array[i]->unload();
		}
	}

	void GLResourceManager::addObjScene(const char *assetName) {
		if (!objSceneBindingTable->contains(assetName)) {
			GLObjSceneBinding *binding = new GLObjSceneBinding(assetName);
			objSceneBindingArray->add(binding);
			objSceneBindingTable->put(assetName, binding);
			char buf[255];
			sprintf(buf, "GLResourceManager: %d objscenes (%s)", objSceneBindingArray->getSize(), assetName);
			logmsg(buf);
		}
	}

	GLObjSceneBinding* GLResourceManager::getObjScene(const char *assetName) {
		return objSceneBindingTable->get(assetName);
	}

	void GLResourceManager::removeObjScene(const char *assetName) {
		// TODO - unload if active
		GLObjSceneBinding *binding = objSceneBindingTable->remove(assetName);
		if (binding) {
			objSceneBindingArray->remove(binding);
		}
	}

	void GLResourceManager::clearObjScenes() {
		// TODO - if any objs are loaded into current context, we need to unload them first!
		objSceneBindingArray->deleteElements();
		objSceneBindingArray->clear();
		objSceneBindingTable->clear();
	}

	void GLResourceManager::loadObjScenes() {
		for (S32 i = 0; i < objSceneBindingArray->getSize(); i++) {
			objSceneBindingArray->array[i]->load(context);
		}
	}

	void GLResourceManager::unloadObjScenes() {
		for (S32 i = 0; i < objSceneBindingArray->getSize(); i++) {
			objSceneBindingArray->array[i]->unload(context);
		}
	}

	void GLResourceManager::addAssimp(const char *assetName) {
		if (!assimpBindings->contains(assetName)) {
			GLAssimpBinding *binding = new GLAssimpBinding(assetName);
			assimpBindings->put(assetName, binding);
			//char buf[255];
			//sprintf(buf, "GLResourceManager: %d assimps (%s)", assimpBindings->getSize(), assetName);
			//logmsg(buf);
		}
	}

	GLAssimpBinding* GLResourceManager::getAssimp(const char *assetName) {
		return assimpBindings->get(assetName);
	}

	void GLResourceManager::removeAssimp(const char *assetName) {
		// TODO - unload if active
		GLAssimpBinding *binding = assimpBindings->remove(assetName);
		if (binding) {
			assimpBindings->remove(assetName);
		}
	}

	void GLResourceManager::clearAssimps() {
		// TODO - if any objs are loaded into current context, we need to unload them first!
		assimpBindings->deleteElements();
	}

	void GLResourceManager::loadAssimps() {
		for (StrHashTable<GLAssimpBinding*>::Iterator i = assimpBindings->getIterator(); i.hasNext;) {
			GLAssimpBinding *binding = assimpBindings->getNext(i);
			binding->load(context);
		}
	}

	void GLResourceManager::unloadAssimps() {
		for (StrHashTable<GLAssimpBinding*>::Iterator i = assimpBindings->getIterator(); i.hasNext;) {
			GLAssimpBinding *binding = assimpBindings->getNext(i);
			binding->unload(context);
		}
	}

}
