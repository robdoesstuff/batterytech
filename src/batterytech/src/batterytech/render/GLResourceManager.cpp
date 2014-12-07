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
// Name        : GLResourceManager.cpp
// Description : An OpenGL resource manager, manages textures and VBO-bound objects
//============================================================================

#include "GLResourceManager.h"
#include "GLObjSceneBinding.h"
#include "GLAssimpBinding.h"
#include "../Logger.h"
#include <stdio.h>
#include "AssetTexture.h"
#include "ShaderProgram.h"
#include "../Context.h"
#include "GraphicsConfiguration.h"

#define MAX_TEXTURES 400
#define MAX_OBJSCENES 100
#define MAX_ASSIMPS 100
#define MAX_SHADERS 100

static BOOL32 debugTextures = FALSE;
static BOOL32 debugAssimp = FALSE;

namespace BatteryTech {

	GLResourceManager::GLResourceManager(Context *context) {
		this->context = context;
		texArray = new ManagedArray<Texture>(MAX_TEXTURES);
		texTable = new StrHashTable<Texture*>((int)(MAX_TEXTURES * 1.3f));
		objSceneBindingArray = new ManagedArray<GLObjSceneBinding>(MAX_OBJSCENES);
		objSceneBindingTable = new StrHashTable<GLObjSceneBinding*>((int)(MAX_OBJSCENES * 1.3f));
		shaderPrograms = new StrHashTable<ShaderProgram*>((int)(MAX_SHADERS * 1.3f));
#ifdef BATTERYTECH_INCLUDE_ASSIMP
		assimpBindings = new StrHashTable<GLAssimpBinding*>(MAX_ASSIMPS * 1.3f);
#endif
		Property *prop = context->appProperties->get("debug_textures");
		if (prop) {
			debugTextures = prop->getBoolValue();
		}
		prop = context->appProperties->get("debug_assimp");
		if (prop) {
			debugAssimp = prop->getBoolValue();
		}
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
		shaderPrograms->deleteElements();
		delete shaderPrograms;
		shaderPrograms = NULL;
#ifdef BATTERYTECH_INCLUDE_ASSIMP
		assimpBindings->deleteElements();
		delete assimpBindings;
		assimpBindings = NULL;
#endif
		context = NULL;
	}

	void GLResourceManager::invalidateGL() {
		//is called from renderer when context was killed.
		for (StrHashTable<Texture*>::Iterator iter = texTable->getIterator(); iter.hasNext;) {
			Texture *tex = texTable->getNext(iter);
			if (tex) {
				tex->invalidateGL();
			}
		}
		for (StrHashTable<ShaderProgram*>::Iterator i = shaderPrograms->getIterator(); i.hasNext;) {
			ShaderProgram *shader = shaderPrograms->getNext(i);
			shader->invalidateGL();
		}
#ifdef BATTERYTECH_INCLUDE_ASSIMP
		for (StrHashTable<GLAssimpBinding*>::Iterator i = assimpBindings->getIterator(); i.hasNext;) {
			GLAssimpBinding *assimpBinding = assimpBindings->getNext(i);
			assimpBinding->invalidateGL();
		}
#endif
	}

	void GLResourceManager::addTexture(const char *assetName, BOOL32 loadOnDemand) {
		if (!texTable->contains(assetName)) {
			Texture *texture = new AssetTexture(context, assetName, loadOnDemand);
			texture->repeatX = context->gConfig->textureRepeat;
			texture->repeatY = context->gConfig->textureRepeat;
			// AssetTexture = asset-backed texture
			// AtlasMappedTexture = virtualized texture coordinate system using assetName pointing to a real texture
			texArray->add(texture);
			// use the texture-allocated string as key, texture itself will clean it up (removal should be synchronized)
			texTable->put(texture->assetName, texture);
			if (debugTextures) {
				char buf[255];
				sprintf(buf, "GLResourceManager: %d textures (%s) %s", texArray->getSize(), assetName, (loadOnDemand ? "OnDemand" : ""));
				logmsg(buf);
			}
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
		Texture *texture = texTable->remove(assetName);
		if (texture) {
			texArray->remove(texture);
			if (texture->isLoaded()) {
				texture->unload();
			}
			// this will call back to this function to remove the atlased textures
			texture->clearAliases();
			delete texture;
		}
	}

	void GLResourceManager::clearTextures() {
		for (S32 i = 0; i < texArray->getSize(); i++) {
			if (texArray->array[i]->isLoaded()) {
				texArray->array[i]->unload();
			}
		}
		// no need to clear aliases here - all is already cleared.
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

	void GLResourceManager::addShaderProgram(const char *tag, ShaderProgram *program) {
		if (!context->gConfig->useShaders) {
			return;
		}
		if (!shaderPrograms->contains(tag)) {
			shaderPrograms->put(tag, program);
		}
	}

	ShaderProgram* GLResourceManager::getShaderProgram(const char *tag) {
		return shaderPrograms->get(tag);
	}

	void GLResourceManager::removeShaderProgram(const char *tag) {
		shaderPrograms->remove(tag);
	}

	void GLResourceManager::clearShaderPrograms() {
		shaderPrograms->deleteElements();
	}

	void GLResourceManager::loadShaderPrograms() {
		if (!context->gConfig->useShaders) {
			return;
		}
		for (StrHashTable<ShaderProgram*>::Iterator i = shaderPrograms->getIterator(); i.hasNext;) {
			ShaderProgram *shader = shaderPrograms->getNext(i);
			shader->load(FALSE);
		}
	}

	void GLResourceManager::unloadShaderPrograms() {
		for (StrHashTable<ShaderProgram*>::Iterator i = shaderPrograms->getIterator(); i.hasNext;) {
			ShaderProgram *shader = shaderPrograms->getNext(i);
			shader->unload();
		}
	}

	void GLResourceManager::addObjScene(const char *assetName) {
		if (!objSceneBindingTable->contains(assetName)) {
			GLObjSceneBinding *binding = new GLObjSceneBinding(assetName);
			objSceneBindingArray->add(binding);
			objSceneBindingTable->put(assetName, binding);
			if (debugAssimp) {
				char buf[255];
				sprintf(buf, "GLResourceManager: %d objscenes (%s)", objSceneBindingArray->getSize(), assetName);
				logmsg(buf);
			}
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

#ifdef BATTERYTECH_INCLUDE_ASSIMP
	void GLResourceManager::addAssimp(const char *assetName) {
		if (!assimpBindings->contains(assetName)) {
			GLAssimpBinding *binding = new GLAssimpBinding(assetName);
			assimpBindings->put(assetName, binding);
			if (debugAssimp) {
				char buf[255];
				sprintf(buf, "GLResourceManager: %d assimps (%s)", assimpBindings->size(), assetName);
				logmsg(buf);
			}
		}
	}

	GLAssimpBinding* GLResourceManager::getAssimp(const char *assetName) {
		return assimpBindings->get(assetName);
	}

	void GLResourceManager::removeAssimp(const char *assetName) {
		GLAssimpBinding *binding = assimpBindings->remove(assetName);
		binding->unload(context);
		if (binding) {
			assimpBindings->remove(assetName);
		}
	}

	void GLResourceManager::clearAssimps() {
		unloadAssimps();
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
#endif

}
