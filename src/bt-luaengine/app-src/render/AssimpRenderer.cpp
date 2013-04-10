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

#include "AssimpRenderer.h"
#include "../gameobject/GameObject.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/importers/assimp/BTAssimpImporter.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/render/GLModelBinding.h>
#include <batterytech/Logger.h>
#include "../World.h"
#include <batterytech/render/GLResourceManager.h>
#include "../gameobject/GameObjectLuaBinder.h"
#include "RenderItem.h"
#include <batterytech/render/GLAssimpBinding.h>
#include <batterytech/render/GLAssimpMeshBinding.h>
#include <batterytech/util/strx.h>
#include <batterytech/render/AssimpAnimator.h>
#include "GlobalLight.h"
#include "LocalLight.h"
#include "WorldRenderer.h"
#include "../GameContext.h"
#include <stdio.h>

#define RENDER_SKELETON FALSE

// #define COUNT_RENDER_NODES

#ifdef COUNT_RENDER_NODES
static U32 renderCount;
#endif

AssimpRenderer::AssimpRenderer(GameContext *context) {
	this->context = context;
	skelShaderProgram = NULL;
	currentShaderProgram = NULL;
	lineCount = 0;
	lightsBound = FALSE;
	context->glResourceManager->addShaderProgram("skeleton", new ShaderProgram("skeleton", "shaders/lineshader.vert", "shaders/lineshader.frag"));
}

AssimpRenderer::~AssimpRenderer() {
    if(currentMeshBinding)
        currentMeshBinding->unbind();
	// current is just a pointer to one of the others.  no deletion.
	currentShaderProgram = NULL;
}

void AssimpRenderer::init(BOOL32 newContext) {
	checkGLError("AssimpRenderer init");
}

void AssimpRenderer::bindShader(ShaderProgram *shaderProgram, const Vector3f &ecLightDir, const AssimpShaderConfig &config) {
	// switch to the shader for this node
	shaderProgram->bind();
	glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
	glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
	if (config.withDirectionalLight) {
		glUniform3f(shaderProgram->getUniformLoc("dirLight.direction"), ecLightDir.x, ecLightDir.y, ecLightDir.z);
		// dir light's colors will be calculated with the material in bindMaterial()
	}
	if (config.withRGBAShadowmap) {
	    glUniform1i(shaderProgram->getUniformLoc("shadowTexture"), 2);
	    glUniformMatrix4fv(shaderProgram->getUniformLoc("shadow_matrix"), 1, GL_FALSE, (GLfloat*) (*(Matrix4f*)context->renderContext->userValues->get("shadowLookupMatrix")).data);
		const Vector3f &shadColor = context->world->globalLight->shadowColor;
		F32 shadEpsilon = context->world->globalLight->shadowEpsilon;
		glUniform4f(shaderProgram->getUniformLoc("shadowColorEpsilon"), shadColor.x, shadColor.y, shadColor.z, shadEpsilon);
	}
}

void AssimpRenderer::bindMaterial(RenderItem *item, GLAssimpMeshBinding *meshBinding, GLAssimpBinding *binding, const AssimpShaderConfig &config) {
	// only override the texture if the texture attribute is null
	if (item->textureName[0] == '\0') {
		// bind the diffuse texture specified in the material using the relative path to the imported asset
		char texturePath[1024];
		strcpy(texturePath, binding->importedAssetBasename);
		strcat(texturePath, "/");
		strcat(texturePath, meshBinding->matDiffuseTexture);
		Texture *tex = context->glResourceManager->getTexture(texturePath);
		if (tex) {
			tex->bind();
		}
	}
	ShaderProgram *shaderProgram = ShaderProgram::currentProgram;
	if (config.withDirectionalLight || config.pointLightCount) {
		glUniform1f(shaderProgram->getUniformLoc("material.specular_exponent"), meshBinding->matShininess);
	}
	// we precalculate the global light's ambient with the current material to save on shader cost
	if (config.withDirectionalLight) {
		Vector4f gAmbient = context->world->globalLight->ambient * meshBinding->matAmbient;
		Vector4f gDiffuse = context->world->globalLight->diffuse * meshBinding->matDiffuse;
		Vector4f gSpecular = context->world->globalLight->specular * meshBinding->matSpecular;
		glUniform4f(shaderProgram->getUniformLoc("dirLight.ambient_color"), gAmbient.x, gAmbient.y, gAmbient.z, gAmbient.w);
		glUniform4f(shaderProgram->getUniformLoc("dirLight.diffuse_color"), gDiffuse.x, gDiffuse.y, gDiffuse.z, gDiffuse.w);
		glUniform4f(shaderProgram->getUniformLoc("dirLight.specular_color"), gSpecular.x, gSpecular.y, gSpecular.z, gSpecular.w);
	}
	Vector4f colorFilter = context->renderContext->colorFilter;
	glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
    RenderDefaults *defaults = ((RenderDefaults*)context->renderContext->userValues->get("renderDefaults"));
	F32 fogNear = defaults->fogNear;
	F32 fogFar = defaults->fogFar;
	glUniform4f(shaderProgram->getUniformLoc("fog_and_uv_offset"), fogNear, fogFar, item->uvs.x, item->uvs.y);
	if (config.withFog) {
        Vector4f fogColor = defaults->fogColor;
		glUniform4f(shaderProgram->getUniformLoc("fogColor"), fogColor.x, fogColor.y, fogColor.z, fogColor.w);
	}
}


void AssimpRenderer::render(RenderItem *item, BOOL32 transparent) {
	// we have not bound the lights for this item yet.
	lightsBound = FALSE;
	// precalculate object lighting and modelview
	context->renderContext->colorFilter = item->colorFilter;
	Vector3f ecLightDir = context->world->camera->matrix * context->world->globalLight->direction;
	ecLightDir.normalize();
	// halfplane needs to be normalized vec3(light dir + view dir)
	Vector3f objToCam = context->world->camera->pos - item->mat.getTranslation();
	objToCam.normalize();
	Vector3f halfplane = context->world->globalLight->direction + objToCam;
	halfplane.normalize();
	halfplane = context->world->camera->matrix * halfplane;
	halfplane.normalize();
	Matrix4f myMv;
	myMv = context->renderContext->mvMatrix * item->mat;

	if (item->textureName[0]) {
		Texture *texture = context->glResourceManager->getTexture(item->textureName);
		if (texture) {
			texture->bind();
		}
	}
	// TODO - mark if binding has transparent textures in it!
	GLAssimpBinding *binding = context->glResourceManager->getAssimp(item->resourceName);
	const char *meshName = item->attr1;
#ifdef COUNT_RENDER_NODES
	renderCount = 0;
#endif
	if (binding) {
		AssimpAnimator *animator = item->animator ? item->animator : binding->defaultAnimator;
        RenderNode* node = animator->rootNode;
        if( meshName[0] != '\0' ) {
            node = animator->nodeTable->get(meshName);
        }
        if (node) {
        	renderNode(node, binding, myMv, item, ecLightDir, halfplane, transparent);
        }
	}
#ifdef COUNT_RENDER_NODES
	char buf[255];
	sprintf(buf, "AssimpRenderer: rendered %d nodes", renderCount);
	logmsg(buf);
#endif
//	if (currentShaderProgram) {
//		currentShaderProgram->unbind();
//	}
//	currentShaderProgram = NULL;
	checkGLError("AssimpRenderer render");
	if (RENDER_SKELETON) {
		glFrontFace(GL_CW);
		renderSkeleton(item);
		glFrontFace(GL_CCW);
	}
}

void AssimpRenderer::setupPointLights(ShaderProgram *shaderProgram, RenderItem *item, Matrix4f mv, GLAssimpMeshBinding *meshBinding) {
	if (!item->maxPointLights) {
		return;
	}
	// find closest (numObjectLights) and display them, or do closest to camera if terrain object
	F32 distances[MAX_LOCALLIGHTS];
	// lightIdxs is the lights we've determined to be the closest to the RenderItem
	U16 lightIdxs[MAX_LOCALLIGHTS];
	U16 lightsInDistance = 0;
	if (item->flags & RENDERITEM_FLAG_USE_CAMERA_CLOSEST_POINT_LIGHTS) {
		const Vector3f &camPos = context->world->camera->pos;
		for (S32 i = 0; i < context->world->localLightsUsed; i++) {
			distances[i] = (context->world->localLights[i].position - camPos).lengthSq();
		}
	} else {
		for (S32 i = 0; i < context->world->localLightsUsed; i++) {
			distances[i] = (context->world->localLights[i].position - item->getTranslation()).lengthSq();
		}
	}
	F32 furthestAccepted = 0.0f;
	if (item->maxPointLights > 0) {
		for (S32 i = 0; i < context->world->localLightsUsed; i++) {
			F32 distance = distances[i];
			if (lightsInDistance < item->maxPointLights) {
				lightIdxs[lightsInDistance++] = i;
				if (distance > furthestAccepted) {
					furthestAccepted = distance;
				}
			} else {
				if (distance < furthestAccepted) {
					// this light is closer than any in our working set.
					// replace the furthest one away
					// find furthest
					F32 furthestDistance = 0;
					S32 furthestLightIdx = -1;
					for (S32 j = 0; j < lightsInDistance; j++) {
						if (distances[lightIdxs[j]] > furthestDistance) {
							furthestDistance = distances[lightIdxs[j]];
							furthestLightIdx = j;
						}
					}
					// now replace and recheck to find the new furthest distance
					lightIdxs[furthestLightIdx] = i;
					furthestAccepted = 0;
					for (S32 j = 0; j < lightsInDistance; j++) {
						if (distances[lightIdxs[j]] > furthestAccepted) {
							furthestAccepted = distances[lightIdxs[j]];
						}
					}
				}
			}
		}
	}
	if (lightsInDistance > item->maxPointLights) {
		lightsInDistance = item->maxPointLights;
	}
	// glUniform1i(shaderProgram->getUniformLoc("pointLightCount"), lightsInDistance);
	// The number of lights in distance will be the maximum of the render item's max point lights or the number of point lights in the scene.
	// A preprocessed shader will be created with exactly that number of point lights, up to 5 in the current implementation (that's a lot - don't use more.)
	for (S32 i = 0; i < lightsInDistance; i++) {
		char buf[256];
		// point light
		LocalLight *light = &context->world->localLights[lightIdxs[i]];
		Vector4f lightPosition = Vector4f(light->position.x, light->position.y, light->position.z, 1.0f);
		Vector4f pECPosition = context->world->camera->matrix * lightPosition;
		//char logBuf[512];
		//sprintf(logBuf, "lightpos = %f, %f, %f ecPos = %f, %f, %f", light->position.x, light->position.y, light->position.z, pECPosition.x, pECPosition.y, pECPosition.z);
		//logmsg(logBuf);
		getUniformStructName(buf, "pointLight", i, "position");
		glUniform3f(shaderProgram->getUniformLoc(buf), pECPosition.x, pECPosition.y, pECPosition.z);
		getUniformStructName(buf, "pointLight", i, "attenuations");
		glUniform3f(shaderProgram->getUniformLoc(buf), light->constantAttenuation, light->linearAttenuation, light->quadraticAttenuation);
		Vector4f pAmbient = light->ambient * meshBinding->matAmbient;
		Vector4f pDiffuse = light->diffuse * meshBinding->matDiffuse;
		Vector4f pSpecular = light->specular * meshBinding->matSpecular;
		getUniformStructName(buf, "pointLight", i, "ambient_color");
		glUniform4f(shaderProgram->getUniformLoc(buf), pAmbient.x, pAmbient.y, pAmbient.z, pAmbient.w);
		getUniformStructName(buf, "pointLight", i, "diffuse_color");
		glUniform4f(shaderProgram->getUniformLoc(buf), pDiffuse.x, pDiffuse.y, pDiffuse.z, pDiffuse.w);
		getUniformStructName(buf, "pointLight", i, "specular_color");
		glUniform4f(shaderProgram->getUniformLoc(buf), pSpecular.x, pSpecular.y, pSpecular.z, pSpecular.w);
	}
	const Vector3f &camPos = context->world->camera->pos;
	Vector4f camPos4(camPos.x, camPos.y, camPos.z, 1.0);
	Vector4f ecCamera = mv * camPos4;
	glUniform3f(shaderProgram->getUniformLoc("ecCamera"), ecCamera.x, ecCamera.y, ecCamera.z);
}

void AssimpRenderer::unbind() {
    if(currentMeshBinding != NULL) currentMeshBinding->unbind();
}

void AssimpRenderer::renderNode(RenderNode *node, GLAssimpBinding *binding, Matrix4f mv, RenderItem *item, const Vector3f &ecLightDir, const Vector3f &halfplane, BOOL32 transparent) {
	Matrix4f myMv = mv * node->globalTransform;
	BOOL32 renderThis = node->meshCount && !node->isBone && !strStartsWith(node->name, "Bulb");
	if (renderThis && item->flags & RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST) {
		AABB3f transformedAABB = item->mat * node->globalAABB;
		transformedAABB.reCalc();
		renderThis = context->world->camera->isInView(transformedAABB.pMin, transformedAABB.pMax);
	}
	if (renderThis) {
#ifdef COUNT_RENDER_NODES
		renderCount++;
#endif
		// only render nodes with meshes that aren't bone nodes
		if (node->meshCount && !node->isBone) {
			for (U32 i = 0; i < node->meshCount; i++) {
				U32 meshIdx = node->meshIndices[i];
				GLAssimpMeshBinding *meshBinding = binding->meshBindingPtrs[meshIdx];
				// is transparent if this texture name ends in .png
				const char *textureName = item->textureName;
				if (!textureName || strlen(textureName) == 0) {
					textureName = meshBinding->matDiffuseTexture;
				}
				BOOL32 isTransparent = strEquals(textureName+strlen(textureName)-4, ".png") || item->colorFilter.a < 1.0f;
				if (isTransparent != transparent) {
					continue;
				}
				checkGLError("AssimpRenderer render0");
				// choose and bind to shader program for this mesh
				AssimpShaderConfig config;
				RenderDefaults *dflts = (RenderDefaults*)context->renderContext->userValues->get("renderDefaults");
				config.hwAccel = ASSIMP_GPU_ACCELERATED_RENDER && meshBinding->hasBones;
				config.pointLightCount = item->maxPointLights;
				if (context->world->localLightsUsed < item->maxPointLights) {
					// we can only have as many point lights as actually exist in the world.
					config.pointLightCount = context->world->localLightsUsed;
				}
				config.withDirectionalLight = dflts->dirLightEnabled && !(item->flags & RENDERITEM_FLAG_NO_DIR_LIGHT);
				config.withFog = dflts->fogEnabled && !(item->flags & RENDERITEM_FLAG_NO_FOG);
				config.withRGBAShadowmap = context->gConfig->shadowType != GraphicsConfiguration::SHADOWTYPE_NONE && !(item->flags & RENDERITEM_FLAG_NO_SHADOW_RECV);
				config.vertexLighting = !(item->flags & RENDERITEM_FLAG_LIGHT_PER_PIXEL);
				char tagBuf[255];
				getShaderTag(tagBuf, config);
				ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(tagBuf);
				if (!shaderProgram) {
					shaderProgram = addShaderProgram(tagBuf, config);
				}
                bool shaderChanged = false;
                
                if(shaderProgram != ShaderProgram::currentProgram) {
                    bindShader(shaderProgram, ecLightDir, config);
                    lightsBound = FALSE;
                    shaderChanged = true;
                }
				// we used to cache the point lights but that we can't because we're premultiplying the material now to optimize the shader's work
				if (config.pointLightCount) {
					setupPointLights(shaderProgram, item, myMv, meshBinding);
				}
				bindMaterial(item, meshBinding, binding, config);
				if (config.withDirectionalLight) {
					// half-plane is object translation dependent so don't let it be cached with the global light
					glUniform3f(shaderProgram->getUniformLoc("dirLight.halfplane"), halfplane.x, halfplane.y, halfplane.z);
				}
				Matrix3f inv3;
				checkGLError("AssimpRenderer render2");
				glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMv.data);
				if (config.withDirectionalLight || config.pointLightCount) {
					myMv.toMatrix3(inv3);
					inv3 = inv3.transpose().inverse();
					glUniformMatrix3fv(shaderProgram->getUniformLoc("inv_matrix"), 1, GL_FALSE, (GLfloat*) inv3.data);
				}
                if( shaderChanged || (meshBinding != currentMeshBinding) || meshBinding->hasBones ) {
                    if(currentMeshBinding)
                        currentMeshBinding->unbind();
                    currentMeshBinding = meshBinding;
                    meshBinding->bindVertexAttributes();
                    if (meshBinding->hasBones) {
                        AssimpAnimator *animator = item->animator ? item->animator : binding->defaultAnimator;
                        // render animated
                        if (!ASSIMP_GPU_ACCELERATED_RENDER) {
                            // CPU skinning
                            // very heavy operation here, applying skeletal animation to skin
                            animator->applyTransforms(node, meshBinding->mesh, meshBinding->vertSkinnedAtts);
                            // now uploading CPU computed skin to GPU
                            meshBinding->updateDynamicDraw();
                        }
                        meshBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
                        if (config.withDirectionalLight || config.pointLightCount) {
                        	meshBinding->bindNormalsToShader(shaderProgram->getVertexAttributeLoc("vNormal"));
                        }
                        // GPU skinning should needs Matrix4f *boneMats and each vert needs a Vector4i idx to boneMats and a Vector4f weights.
                        // Total vert data will be:  bind Pos, bind Norm, UV, boneMatsIdx, boneWeights
                        if (ASSIMP_GPU_ACCELERATED_RENDER) {
                            // GPU skinning
                            meshBinding->bindBonesToShader(shaderProgram->getVertexAttributeLoc("vBones"));
                            meshBinding->bindWeightsToShader(shaderProgram->getVertexAttributeLoc("vWeights"));
                            Matrix4f *boneMatsWithGlobalInv = animator->updateGlobalInverseBoneMatrices(node, meshBinding->mesh);
                            MeshBoneMatrices *mbMats = animator->meshBoneMatrices->get(meshBinding->mesh);
                            glUniformMatrix4fv(shaderProgram->getUniformLoc("bone_matrices"), mbMats->boneMatrixCount, GL_FALSE, (GLfloat*) boneMatsWithGlobalInv);
                       }
                    } else {
                        // Static model, no skinning
                        meshBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
                        if (config.withDirectionalLight || config.pointLightCount) {
                        	meshBinding->bindNormalsToShader(shaderProgram->getVertexAttributeLoc("vNormal"));
                        }
                    }
                    meshBinding->bindUVsToShader(shaderProgram->getVertexAttributeLoc("vUV"));
                    meshBinding->bindFaceIndices();
                 }
				glDrawElements(GL_TRIANGLES, meshBinding->faceCount * 3, GL_UNSIGNED_SHORT, 0);
                checkGLError("AssimpRenderer render");
               //meshBinding->unbind();
			}
		}
	}
	for (S32 i = 0; i < node->childNodes->getSize(); i++) {
		renderNode(node->childNodes->array[i], binding, mv, item, ecLightDir, halfplane, transparent);
	}
}


/* ------------------------ ShadowMap Creation -------------------------- */

void AssimpRenderer::renderShadow(RenderItem* item) {
	if (item->flags & RENDERITEM_FLAG_NO_SHADOW_GEN) {
		return;
	}
	//if(!strcmp(item->resourceName,"BeachWorld/BeachLevel.obj")) return;
	GLAssimpBinding *binding = context->glResourceManager->getAssimp(item->resourceName);
	Matrix4f shadowMVP = *(Matrix4f*)context->renderContext->userValues->get("shadowMVP");
	Matrix4f myMv = shadowMVP * item->mat;
    // shadowShader->bind();
    
	// checkGLError("AssimpRenderer renderShadow shadowShader->bind()");
	if (binding) {
		AssimpAnimator *animator = item->animator ? item->animator : binding->defaultAnimator;
		renderNodeShadow(animator->rootNode, binding, item, myMv);
		checkGLError("AssimpRenderer renderShadow renderNodeShadow()");
	}
	//context->worldRenderer->currentShaderProgram->unbind();
	checkGLError("AssimpRenderer renderShadow");
}

void AssimpRenderer::renderNodeShadow(RenderNode *node, GLAssimpBinding *binding, RenderItem *item, Matrix4f mv) {
	Matrix4f myMv = mv * node->globalTransform;
    if (node->meshCount && !node->isBone && !strStartsWith(node->name, "noshadow")) {
		for (U32 i = 0; i < node->meshCount; i++) {
			U32 meshIdx = node->meshIndices[i];
			GLAssimpMeshBinding *meshBinding = binding->meshBindingPtrs[meshIdx];
            bool shaderChanged = false;
            char ssTag[255];
            getShadowShaderTag(ssTag, ASSIMP_GPU_ACCELERATED_RENDER && meshBinding->hasBones);
            ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(ssTag);
            if (!shaderProgram) {
            	shaderProgram = createShadowShaderProgram(TRUE, ASSIMP_GPU_ACCELERATED_RENDER && meshBinding->hasBones);
            	ShadowMap::ShadowTextureType *textureType = (ShadowMap::ShadowTextureType*) context->renderContext->userValues->get("shadowmap_texture_type");
            	if (textureType) {
            		if (*textureType != ShadowMap::SHADOWTEXTURE_DEPTH) {
                		shaderProgram->addDefine("SHADOWMAP_WITH_FRAGMENT", "1");
            		}
            		if (*textureType == ShadowMap::SHADOWTEXTURE_RGB) {
                		shaderProgram->addDefine("SHADOWMAP_PACK_RGB", "1");
            		} else if (*textureType == ShadowMap::SHADOWTEXTURE_ALPHA) {
            			shaderProgram->addDefine("SHADOWMAP_ALPHA_CHANNEL", "1");
            		}
            	}
            	// do it now!
            	shaderProgram->load(TRUE);
            	context->glResourceManager->addShaderProgram(ssTag, shaderProgram);
            }
            if (shaderProgram != ShaderProgram::currentProgram) {
                shaderProgram->bind();
                shaderChanged = true;
            }
			glUniformMatrix4fv(shaderProgram->getUniformLoc("mvp_matrix"), 1, GL_FALSE, (GLfloat*) myMv.data);
            if( shaderChanged || (meshBinding != currentMeshBinding) || meshBinding->hasBones )
            {
                currentMeshBinding = meshBinding;
                meshBinding->bindVertexAttributes();
                if (meshBinding->hasBones) {
                    AssimpAnimator *animator = item->animator ? item->animator : binding->defaultAnimator;
                    // render animated
                    if (!ASSIMP_GPU_ACCELERATED_RENDER) {
                        // CPU skinning
                        // very heavy operation here, applying skeletal animation to skin
                        // TODO - don't double-apply from regular render!!
                        animator->applyTransforms(node, meshBinding->mesh, meshBinding->vertSkinnedAtts);
                        // now uploading CPU computed skin to GPU
                        meshBinding->updateDynamicDraw();
                    }
                    meshBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
                     // GPU skinning should needs Matrix4f *boneMats and each vert needs a Vector4i idx to boneMats and a Vector4f weights.
                    // Total vert data will be:  bind Pos, bind Norm, UV, boneMatsIdx, boneWeights
                    if (ASSIMP_GPU_ACCELERATED_RENDER) {
                        // GPU skinning
                        meshBinding->bindBonesToShader(shaderProgram->getVertexAttributeLoc("vBones"));
                        meshBinding->bindWeightsToShader(shaderProgram->getVertexAttributeLoc("vWeights"));
                        Matrix4f *boneMatsWithGlobalInv = animator->updateGlobalInverseBoneMatrices(node, meshBinding->mesh);
                        MeshBoneMatrices *mbMats = animator->meshBoneMatrices->get(meshBinding->mesh);
                        //boneMatsWithGlobalInv[0].identity();
                       //Matrix4f mat = boneMatsWithGlobalInv[0];
                        //glUniformMatrix4fv(shadowHwSkinShader->getUniformLoc("bone_matrices"), animator->boneMatrixCount, GL_FALSE, (GLfloat*) boneMatsWithGlobalInv[0]);
						glUniformMatrix4fv(shaderProgram->getUniformLoc("bone_matrices"), mbMats->boneMatrixCount, GL_FALSE, (GLfloat*) boneMatsWithGlobalInv);
                        checkGLError("AssimpRenderer renderNodeShadow bone matrices");
                   }
                } else {
                    // Static model, no skinning
                    meshBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
                }
                meshBinding->bindFaceIndices();
                checkGLError("AssimpRenderer renderNodeShadow before draw");
            }
            glDrawElements(GL_TRIANGLES, meshBinding->faceCount * 3, GL_UNSIGNED_SHORT, 0);
       		checkGLError("AssimpRenderer renderNodeShadow after draw");
            
			//meshBinding->unbind();
		}
    }
	for (S32 i = 0; i < node->childNodes->getSize(); i++) {
		renderNodeShadow(node->childNodes->array[i], binding, item, mv);
	}

}

/* ------------------------ End ShadowMap Creation -------------------------- */



/* ------------------------ Skeleton Debugging ------------------------------ */

void AssimpRenderer::renderSkeleton(RenderItem *item) {
	checkGLError("renderSkeleton Start");
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	skelShaderProgram->bind();
	GLAssimpBinding *binding = context->glResourceManager->getAssimp(item->resourceName);
	Matrix4f m;
	Matrix4f myMv = context->renderContext->mvMatrix * item->mat;
	lineCount = 0;
	AssimpAnimator *animator = item->animator ? item->animator : binding->defaultAnimator;
	RenderSkeletonNode(animator->rootNode, m);
	glVertexAttribPointer(skelShaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, vPos);
	glVertexAttribPointer(skelShaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, 0, vColor);
	glUniformMatrix4fv(skelShaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, context->renderContext->projMatrix.data);
	glUniformMatrix4fv(skelShaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, myMv.data);
	glDrawArrays(GL_LINES, 0, lineCount * 2);
	skelShaderProgram->unbind();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void AssimpRenderer::RenderSkeletonNode(RenderNode* node, const Matrix4f parent) {
	Matrix4f me = node->globalTransform;
	if (node->parentNode) {
		drawLine(Vector3f(parent.data[12],parent.data[13],parent.data[14]), Vector3f(me.data[12], me.data[13], me.data[14]), Vector3f(1.0,0,0));
	}
	// render all child nodes
	for (S32 i = 0; i < node->childNodes->getSize();++i) {
		RenderSkeletonNode(node->childNodes->array[i], me );
	}
}

void AssimpRenderer::drawLine(const Vector3f& from, const Vector3f& to,	const Vector3f& color) {
	if (lineCount < SKEL_MAX_LINES) {
		vPos[lineCount * 6] = from.x;
		vPos[lineCount * 6 + 1] = from.y;
		vPos[lineCount * 6 + 2] = from.z;
		vPos[lineCount * 6 + 3] = to.x;
		vPos[lineCount * 6 + 4] = to.y;;
		vPos[lineCount * 6 + 5] = to.z;
		vColor[lineCount * 8] = color.x;
		vColor[lineCount * 8 + 1] = color.y;
		vColor[lineCount * 8 + 2] = color.z;
		vColor[lineCount * 8 + 3] = 1.0;
		vColor[lineCount * 8 + 4] = color.x;
		vColor[lineCount * 8 + 5] = color.y;
		vColor[lineCount * 8 + 6] = color.z;
		vColor[lineCount * 8 + 7] = 1.0;
		lineCount++;
	}
}

void AssimpRenderer::getShadowShaderTag(char *buf, BOOL32 hwAccel) {
	buf[0] = '\0';
	strcat(buf, "assimpshadow");
	if (hwAccel) {
		strcat(buf, "-hwskin");
	}
}

void AssimpRenderer::getShaderTag(char *buf, AssimpShaderConfig config) {
	buf[0] = '\0';
	strcat(buf, "assimp");
	if (config.hwAccel) {
		strcat(buf, "-hwskin");
	}
	if (config.withDirectionalLight) {
		strcat(buf, "-dir");
	}
	if (config.withFog) {
		strcat(buf, "-fog");
	}
	if (config.withRGBAShadowmap) {
		strcat(buf, "-shmap");
	}
	if (config.pointLightCount) {
		strcat(buf, "-pl");
		char count[3];
		sprintf(count, "%d", config.pointLightCount);
		strcat(buf, count);
	}
	if (!config.vertexLighting) {
		strcat(buf, "-frlt");
	}
}

ShaderProgram* AssimpRenderer::addShaderProgram(const char *tag, AssimpShaderConfig config) {
	ShaderProgram *shader = new ShaderProgram(tag, "shaders/assimp.vert", "shaders/assimp.frag");
	if (config.hwAccel) {
		shader->addDefine("HW_SKIN", "1");
	}
	if (config.withDirectionalLight) {
		shader->addDefine("DIR_LIGHT", "1");
	}
	if (config.pointLightCount) {
		char count[3];
		sprintf(count, "%d", config.pointLightCount);
		shader->addDefine("POINT_LIGHT_COUNT", count);
	}
	if (config.withFog) {
		shader->addDefine("FOG", "1");
	}
	if (config.withRGBAShadowmap) {
		shader->addDefine("SHADOWMAP", "1");
	}
	if (config.vertexLighting) {
		shader->addDefine("VERTEX_LIGHTING", "1");
	}
	ShadowMap::ShadowTextureType *textureType = (ShadowMap::ShadowTextureType*) context->renderContext->userValues->get("shadowmap_texture_type");
	if (textureType && *textureType == ShadowMap::SHADOWTEXTURE_RGB) {
		shader->addDefine("SHADOWMAP_PACK_RGB", "1");
	} else if (textureType && *textureType == ShadowMap::SHADOWTEXTURE_ALPHA) {
		shader->addDefine("SHADOWMAP_ALPHA_CHANNEL", "1");
	}

	shader->load(TRUE);
	context->glResourceManager->addShaderProgram(tag, shader);
	return shader;
}

ShaderProgram* AssimpRenderer::createShadowShaderProgram(BOOL32 newContext, BOOL32 hwSkinned) {
	ShaderProgram *shaderProgram;
	char buf[255];
	getShadowShaderTag(buf, hwSkinned);
	if (hwSkinned) {
		shaderProgram = new ShaderProgram(buf, "shaders/assimp_accel_shadowdepth.vert", "shaders/assimp_shadowdepth.frag");
	} else {
		shaderProgram = new ShaderProgram(buf, "shaders/assimp_shadowdepth.vert", "shaders/assimp_shadowdepth.frag");
	}
	return shaderProgram;
}
