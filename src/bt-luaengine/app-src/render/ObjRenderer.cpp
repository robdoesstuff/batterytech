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

#include "ObjRenderer.h"
#include "../gameobject/GameObject.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/importers/obj/ObjImporter.h>
#include <batterytech/importers/obj/ObjScene.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/render/GLModelBinding.h>
#include <batterytech/render/ShaderProgram.h>
#include <batterytech/Logger.h>
//#include <LinearMath/btVector3.h>
//#include <LinearMath/btMatrix3x3.h>
#include "../World.h"
#include <batterytech/render/GLResourceManager.h>
#include "../gameobject/GameObjectLuaBinder.h"
#include "RenderItem.h"
#include <batterytech/render/GLObjSceneBinding.h>
#include <batterytech/util/strx.h>
#include "../GameContext.h"

static ShaderProgram *shaderProgram;

ObjRenderer::ObjRenderer(GameContext *context) {
	this->context = context;
}

ObjRenderer::~ObjRenderer() {
}

void ObjRenderer::init(BOOL32 newContext) {
	if (shaderProgram) {
		delete shaderProgram;
	}
	shaderProgram = new ShaderProgram("obj", "shaders/static_obj_shader.vert", "shaders/static_obj_shader.frag");
	shaderProgram->load(FALSE);
	checkGLError("ObjRenderer init");
}

void ObjRenderer::render(RenderItem *item) {
	//glBindTexture(GL_TEXTURE_2D, texId);
	glFrontFace(GL_CCW);
	context->renderContext->colorFilter = Vector4f(1,1,1,1);
	shaderProgram->bind();
	glUniform4f(shaderProgram->getUniformLoc("material.ambient_color"), 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("material.diffuse_color"), 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("material.specular_color"), 0.5f, 0.5f, 0.5f, 1.0f);
	glUniform1f(shaderProgram->getUniformLoc("material.specular_exponent"), 0.8f);
	Vector3f lightDir(0,0,1.0f);
	Vector3f ecLightDir = context->world->camera->matrix * lightDir;
	ecLightDir.normalize();
	glUniform3f(shaderProgram->getUniformLoc("light.direction"), ecLightDir.x, ecLightDir.y, ecLightDir.z);
	glUniform4f(shaderProgram->getUniformLoc("light.ambient_color"), 0.7f, 0.7f, 0.7f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("light.diffuse_color"), 0.7f, 0.7f, 0.7f, 1.0f);
	glUniform4f(shaderProgram->getUniformLoc("light.specular_color"), 0.5f, 0.5f, 0.5f, 1.0f);
	glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
	Vector4f colorFilter = context->renderContext->colorFilter;
	glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
	glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
	Matrix4f myMv;
	Matrix3f inv3;
	// item-specific

	GLObjSceneBinding *objBinding = context->glResourceManager->getObjScene(item->resourceName);
	if (item->textureName[0]) {
		Texture *texture = context->glResourceManager->getTexture(item->textureName);
		if (texture) {
			texture->bind();
		}
	}
	myMv = context->renderContext->mvMatrix * item->mat;
	//Vector3f modelScale = ball->getModelScale();
	//myMv.scale(modelScale.x, modelScale.y, modelScale.z);
	glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMv.data);
	myMv.toMatrix3(inv3);
	inv3 = inv3.transpose().inverse();
	glUniformMatrix3fv(shaderProgram->getUniformLoc("inv_matrix"), 1, GL_FALSE, (GLfloat*) inv3.data);
	// halfplane needs to be normalized vec3(light dir + view dir)
	Vector3f objToCam = context->world->camera->pos - item->mat.getTranslation();
	objToCam.normalize();
	Vector3f halfplane = lightDir + objToCam;
	halfplane.normalize();
	halfplane = context->world->camera->matrix * halfplane;
	halfplane.normalize();
	glUniform3f(shaderProgram->getUniformLoc("light.halfplane"), halfplane.x, halfplane.y, halfplane.z);
	if (objBinding) {
		for (S32 i = 0; i < objBinding->modelBindingArray->getSize(); i++) {
			GLModelBinding *modelBinding = objBinding->modelBindingArray->array[i];
			// draw it if it's a mesh of the group or if no group was specified.
			if (!item->attr1[0] || strEquals(item->attr1, modelBinding->name)) {
				modelBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
				modelBinding->bindNormalsToShader(shaderProgram->getVertexAttributeLoc("vNormal"));
				modelBinding->bindUVsToShader(shaderProgram->getVertexAttributeLoc("vUV"));
				glDrawArrays(GL_TRIANGLES, 0, modelBinding->vCount);
				modelBinding->unbind();
			}
		}
	}
	shaderProgram->unbind();
	checkGLError("ObjRenderer render");
}
