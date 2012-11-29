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

#include "LevelRenderer.h"
#include "../level/Level.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/importers/obj/ObjImporter.h>
#include <batterytech/importers/obj/ObjScene.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Logger.h>
#include <batterytech/util/strx.h>
#include <batterytech/render/Texture.h>
#include <batterytech/render/GLResourceManager.h>
#include <stdio.h>
//#include <LinearMath/btVector3.h>
//#include <LinearMath/btMatrix3x3.h>
#include "../World.h"
#include "../Game.h"

LevelRenderer::LevelRenderer(GameContext *context) {
	this->context = context;
	modelBindings = new ManagedArray<GLModelBinding>(200);
	shaderProgram = NULL;
}

LevelRenderer::~LevelRenderer() {
	modelBindings->deleteElements();
	delete modelBindings;
	modelBindings = NULL;
}

void LevelRenderer::init(BOOL32 newContext) {
	if (!newContext) {
		for (S32 i = 0; i < modelBindings->getSize(); i++) {
			GLModelBinding *binding = modelBindings->array[i];
			binding->clearGL();
		}
	}
	modelBindings->deleteElements();
	if (shaderProgram) {
		delete shaderProgram;
	}
	// load texture, obj geometry and vbos
	char *collisionGroupName = context->game->luaBinder->getString("levelCollisionGroup");
	ObjScene *scene = context->world->level->scene;
	if (scene) {
		// we'll have multiple bindings with the same name - each representing a different mesh for the group
		for (S32 i = 0; i < scene->groupCount; i++) {
			ObjGroup *group = &scene->groups[i];
			// don't add collision mesh
			if (!strEquals(group->name, collisionGroupName)) {
				for (S32 j = 0; j < group->meshCount; j++) {
					ObjMesh *mesh = &group->meshes[j];
					GLModelBinding *binding = new GLModelBinding();
					binding->name = strDuplicate(group->name);
					binding->init(mesh->vertPositions, mesh->normals, mesh->uvs, mesh->vertCount);
					modelBindings->add(binding);
				}
			}
		}
	}
	delete [] collisionGroupName;
	shaderProgram = new ShaderProgram("level", "shaders/terrain_shader.vert", "shaders/terrain_shader.frag");
	shaderProgram->load(FALSE);
	checkGLError("ObjRenderer init");
}

void LevelRenderer::render(World *world) {
	char *textureName = context->game->luaBinder->getString("levelTexture");
	Texture *tex = context->glResourceManager->getTexture(textureName);
	// TODO - bad spot to alloc & delete
	delete [] textureName;
	if (tex) {
		tex->bind();
	}
	glFrontFace(GL_CCW);
	F32 modelScale = 1.0f;
	context->renderContext->colorFilter = Vector4f(1,1,1,1);
	shaderProgram->bind();
	glUniform4f(shaderProgram->getUniformLoc("material.ambient_color"), 0,0,0, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("material.diffuse_color"), 0.5f, 0.5f, 0.5f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("material.specular_color"), 0.7f, 0.7f, 0.7f, 1.0f);
	glUniform1f(shaderProgram->getUniformLoc("material.specular_exponent"), 0.8f);
	Vector3f lightDir(0,0,1.0f);
	Vector3f ecLightDir = context->world->camera->matrix * lightDir;
	ecLightDir.normalize();
	// halfplane needs to be normalized vec3(light dir + view dir)
	Vector3f objToCam = world->camera->pos - Vector3f(0, 0, 0);
	objToCam.normalize();
	Vector3f halfplane = lightDir + objToCam;
	halfplane.normalize();
	halfplane = context->world->camera->matrix * halfplane;
	halfplane.normalize();
	glUniform3f(shaderProgram->getUniformLoc("light.direction"), ecLightDir.x, ecLightDir.y, ecLightDir.z);
	glUniform3f(shaderProgram->getUniformLoc("light.halfplane"), halfplane.x, halfplane.y, halfplane.z);
	glUniform4f(shaderProgram->getUniformLoc("light.ambient_color"), 0.7f, 0.7f, 0.7f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("light.diffuse_color"), 0.7f, 0.7f, 0.7f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("light.specular_color"), 0.5f, 0.5f, 0.5f, 1.0f);
	glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
	Vector4f colorFilter = context->renderContext->colorFilter;
	glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
	glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
	Matrix4f myMv = context->renderContext->mvMatrix;
	myMv.scale(modelScale, modelScale, modelScale);
	glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMv.data);
	Matrix3f inv3;
	myMv.toMatrix3(inv3);
	inv3 = inv3.transpose().inverse();
	glUniformMatrix3fv(shaderProgram->getUniformLoc("inv_matrix"), 1, GL_FALSE, (GLfloat*) inv3.data);
	// render all meshes for level
	for (S32 i = 0; i < modelBindings->getSize(); i++) {
		GLModelBinding *modelBinding = modelBindings->array[i];
		modelBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
		modelBinding->bindNormalsToShader(shaderProgram->getVertexAttributeLoc("vNormal"));
		modelBinding->bindUVsToShader(shaderProgram->getVertexAttributeLoc("vUV"));
		glDrawArrays(GL_TRIANGLES, 0, modelBinding->vCount);
		if (i == modelBindings->getSize() - 1) {
			// unbind on last
			modelBinding->unbind();
		}
	}
	shaderProgram->unbind();
	checkGLError("ObjRenderer render");
}

