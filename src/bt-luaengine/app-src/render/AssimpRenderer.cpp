/*
 * AssimpRenderer
 *
 *  Created on: Oct 16, 2011
 *      Author: rgreen
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

#define RENDER_SKELETON FALSE
// maximum number of point lights defined in shaders, must match!
#define MAX_POINT_LIGHTS 0
#define USE_FOG FALSE

// #define COUNT_RENDER_NODES

#ifdef COUNT_RENDER_NODES
static U32 renderCount;
#endif

AssimpRenderer::AssimpRenderer(GameContext *context) {
	this->context = context;
	defaultShaderProgram = NULL;
	hwSkinShaderProgram = NULL;
	skelShaderProgram = NULL;
	currentShaderProgram = NULL;
	shadowShader = NULL;
	shadowHwSkinShader = NULL;
	lineCount = 0;
	lightsBound = FALSE;
}

AssimpRenderer::~AssimpRenderer() {
    if(currentMeshBinding)
        currentMeshBinding->unbind();

	delete defaultShaderProgram;
	delete hwSkinShaderProgram;
	delete skelShaderProgram;
	delete shadowShader;
	delete shadowHwSkinShader;
	defaultShaderProgram = NULL;
	hwSkinShaderProgram = NULL;
	skelShaderProgram = NULL;
	shadowShader = NULL;
	shadowHwSkinShader = NULL;
	// current is just a pointer to one of the others.  no deletion.
	currentShaderProgram = NULL;
}

void AssimpRenderer::init(BOOL32 newContext) {
	if (defaultShaderProgram) {
		delete defaultShaderProgram;
	}
	if (hwSkinShaderProgram) {
		delete hwSkinShaderProgram;
	}
	if (skelShaderProgram) {
		delete skelShaderProgram;
	}
	if (shadowShader) {
		delete shadowShader;
	}
	if (shadowHwSkinShader) {
		delete shadowHwSkinShader;
	}
	defaultShaderProgram = createShaderProgram(newContext, FALSE);
	shadowShader = createShadowShaderProgram(newContext, FALSE);
	if (ASSIMP_GPU_ACCELERATED_RENDER) {
		hwSkinShaderProgram = createShaderProgram(newContext, TRUE);
		shadowHwSkinShader = createShadowShaderProgram(newContext, TRUE);
	}
	skelShaderProgram = new ShaderProgram("skeleton", "shaders/lineshader.vert", "shaders/lineshader.frag");
	skelShaderProgram->load(FALSE);
	checkGLError("AssimpRenderer init");
}

ShaderProgram* AssimpRenderer::createShaderProgram(BOOL32 newContext, BOOL32 hwSkinned) {
	ShaderProgram *shaderProgram;
	if (hwSkinned) {
		shaderProgram = new ShaderProgram("assimpaccel", "shaders/assimp_accel_shader.vert", "shaders/assimp_shader.frag");
	} else {
		shaderProgram = new ShaderProgram("assimp", "shaders/assimp_shader.vert", "shaders/assimp_shader.frag");
	}
	shaderProgram->load(FALSE);
	return shaderProgram;
}

ShaderProgram* AssimpRenderer::createShadowShaderProgram(BOOL32 newContext, BOOL32 hwSkinned) {
	ShaderProgram *shaderProgram;
	if (hwSkinned) {
		shaderProgram = new ShaderProgram("assimpaccelshadow", "shaders/assimp_accel_shadowdepth.vert", "shaders/assimp_shadowdepth.frag");
		shaderProgram->addDefine("TEST_DEFINE", "5");
	} else {
		shaderProgram = new ShaderProgram("assimpshadow", "shaders/assimp_shadowdepth.vert", "shaders/assimp_shadowdepth.frag");
	}
	shaderProgram->load(FALSE);
	return shaderProgram;
}

void AssimpRenderer::bindShader(ShaderProgram *shaderProgram, const Vector3f &ecLightDir, const Vector3f &halfplane) {
	// switch to the shader for this node
	shaderProgram->bind();
	glUniform3f(shaderProgram->getUniformLoc("dirLight.direction"), ecLightDir.x, ecLightDir.y, ecLightDir.z);
	const Vector4f &gAmbient = context->world->globalLight->ambient;
	const Vector4f &gDiffuse = context->world->globalLight->diffuse;
	const Vector4f &gSpecular = context->world->globalLight->specular;
	glUniform4f(shaderProgram->getUniformLoc("dirLight.ambient_color"), gAmbient.x, gAmbient.y, gAmbient.z, gAmbient.w);
	glUniform4f(shaderProgram->getUniformLoc("dirLight.diffuse_color"), gDiffuse.x, gDiffuse.y, gDiffuse.z, gDiffuse.w);
	glUniform4f(shaderProgram->getUniformLoc("dirLight.specular_color"), gSpecular.x, gSpecular.y, gSpecular.z, gSpecular.w);
	glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
    glUniform1i(shaderProgram->getUniformLoc("shadowTexture"), 2);
	glUniform3f(shaderProgram->getUniformLoc("dirLight.halfplane"), halfplane.x, halfplane.y, halfplane.z); 
	Vector4f colorFilter = context->renderContext->colorFilter;
	glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
	glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
    glUniformMatrix4fv(shaderProgram->getUniformLoc("shadow_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->shadowLookupMatrix.data);
	const Vector3f &camPos = context->world->camera->pos;
	glUniform3f(shaderProgram->getUniformLoc("cameraPos"), camPos.x, camPos.y, camPos.z);
	const Vector3f &shadColor = context->world->globalLight->shadowColor;
	F32 shadEpsilon = context->world->globalLight->shadowEpsilon;
	glUniform4f(shaderProgram->getUniformLoc("shadowColorEpsilon"), shadColor.x, shadColor.y, shadColor.z, shadEpsilon);
	//char buf[255];
	//sprintf(buf, "cp =  %f %f %f", camPos.x, camPos.y, camPos.z);
	//logmsg(buf);
}

void AssimpRenderer::render(RenderItem *item, BOOL32 transparent) {
	// we have not bound the lights for this item yet.
	lightsBound = FALSE;
	// precalculate object lighting and modelview
	context->renderContext->colorFilter = Vector4f(1,1,1,1);
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
		renderNode(animator->rootNode, binding, myMv, item, ecLightDir, halfplane, transparent);
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

void AssimpRenderer::setupPointLights(ShaderProgram *shaderProgram, RenderItem *item, Matrix4f mv) {
	// TODO - init default uniform values for point lights
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
	if (lightsInDistance > MAX_LOCALLIGHTS) {
		lightsInDistance = MAX_LOCALLIGHTS;
	}
	glUniform1i(shaderProgram->getUniformLoc("pointLightCount"), lightsInDistance);
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
		const Vector4f &pAmbient = light->ambient;
		const Vector4f &pDiffuse = light->diffuse;
		const Vector4f &pSpecular = light->specular;
		getUniformStructName(buf, "pointLight", i, "ambient_color");
		glUniform4f(shaderProgram->getUniformLoc(buf), pAmbient.x, pAmbient.y, pAmbient.z, pAmbient.w);
		getUniformStructName(buf, "pointLight", i, "diffuse_color");
		glUniform4f(shaderProgram->getUniformLoc(buf), pDiffuse.x, pDiffuse.y, pDiffuse.z, pDiffuse.w);
		getUniformStructName(buf, "pointLight", i, "specular_color");
		glUniform4f(shaderProgram->getUniformLoc(buf), pSpecular.x, pSpecular.y, pSpecular.z, pSpecular.w);
	}
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
				BOOL32 isTransparent = strEquals(meshBinding->matDiffuseTexture+strlen(meshBinding->matDiffuseTexture)-4, ".png");
				if (isTransparent != transparent) {
					continue;
				}
				checkGLError("AssimpRenderer render0");
				// choose and bind to shader program for this mesh
				ShaderProgram *shaderProgram;                
				if (ASSIMP_GPU_ACCELERATED_RENDER && meshBinding->hasBones) {
					shaderProgram = hwSkinShaderProgram;
				} else {
					shaderProgram = defaultShaderProgram;
				}
                
                bool shaderChanged = false;
                
                if(shaderProgram != ShaderProgram::currentProgram) {
                    bindShader(shaderProgram, ecLightDir, halfplane);                
                    lightsBound = FALSE;
                    shaderChanged = true;
                }
				if (!lightsBound) {
					if (MAX_POINT_LIGHTS) {
						setupPointLights(shaderProgram, item, myMv);
					}
					lightsBound = TRUE;
				}
				bindMaterial(item, meshBinding, binding);
				Matrix3f inv3;
				checkGLError("AssimpRenderer render2");
				glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMv.data);
				myMv.toMatrix3(inv3);
				inv3 = inv3.transpose().inverse();
				glUniformMatrix3fv(shaderProgram->getUniformLoc("inv_matrix"), 1, GL_FALSE, (GLfloat*) inv3.data);
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
                        meshBinding->bindNormalsToShader(shaderProgram->getVertexAttributeLoc("vNormal"));
                        // GPU skinning should needs Matrix4f *boneMats and each vert needs a Vector4i idx to boneMats and a Vector4f weights.
                        // Total vert data will be:  bind Pos, bind Norm, UV, boneMatsIdx, boneWeights
                        if (ASSIMP_GPU_ACCELERATED_RENDER) {
                            // GPU skinning
                            meshBinding->bindBonesToShader(shaderProgram->getVertexAttributeLoc("vBones"));
                            meshBinding->bindWeightsToShader(shaderProgram->getVertexAttributeLoc("vWeights"));
                            Matrix4f *boneMatsWithGlobalInv = animator->updateGlobalInverseBoneMatrices(node, meshBinding->mesh);
                            glUniformMatrix4fv(shaderProgram->getUniformLoc("bone_matrices"), animator->boneMatrixCount, GL_FALSE, (GLfloat*) boneMatsWithGlobalInv);
                       }
                    } else {
                        // Static model, no skinning
                        meshBinding->bindPositionsToShader(shaderProgram->getVertexAttributeLoc("vPosition"));
                        meshBinding->bindNormalsToShader(shaderProgram->getVertexAttributeLoc("vNormal"));
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

void AssimpRenderer::bindMaterial(RenderItem *item, GLAssimpMeshBinding *meshBinding, GLAssimpBinding *binding) {
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
	Vector4f amb = meshBinding->matAmbient;
	Vector4f dif = meshBinding->matDiffuse;
	Vector4f spec = meshBinding->matSpecular;
	glUniform4f(shaderProgram->getUniformLoc("material.ambient_color"), amb.x, amb.y, amb.z, amb.w);
	glUniform4f(shaderProgram->getUniformLoc("material.diffuse_color"), dif.x, dif.y, dif.z, dif.w);
	glUniform4f(shaderProgram->getUniformLoc("material.specular_color"), spec.x, spec.y, spec.z, spec.w);
	glUniform1f(shaderProgram->getUniformLoc("material.specular_exponent"), 0.8f);
	F32 fogNear = 0;
	F32 fogFar = 500.0f;
	if (item->flags & RENDERITEM_FLAG_NO_FOG) {
		fogFar = 0;
	}
	glUniform4f(shaderProgram->getUniformLoc("fog_and_uv_offset"), fogNear, fogFar, item->uvs.x, item->uvs.y);
	if (USE_FOG) {
		// glUniform4f(shaderProgram->getUniformLoc("fogColor"), 0.05f, 0.05f, 0.2f, 1.0f);
	}
}


/* ------------------------ ShadowMap Creation -------------------------- */

void AssimpRenderer::renderShadow(RenderItem* item) {
	//if(!strcmp(item->resourceName,"BeachWorld/BeachLevel.obj")) return;
	GLAssimpBinding *binding = context->glResourceManager->getAssimp(item->resourceName);
	Matrix4f myMv = context->renderContext->shadowMVP * item->mat;
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
			ShaderProgram *shaderProgram;
            bool shaderChanged = false;
			if (ASSIMP_GPU_ACCELERATED_RENDER && meshBinding->hasBones) {
				shaderProgram = shadowHwSkinShader;
			} else {
				shaderProgram = shadowShader;
			}
            if( shaderProgram != ShaderProgram::currentProgram ) {
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
                        //boneMatsWithGlobalInv[0].identity();
                       //Matrix4f mat = boneMatsWithGlobalInv[0];
                        //glUniformMatrix4fv(shadowHwSkinShader->getUniformLoc("bone_matrices"), animator->boneMatrixCount, GL_FALSE, (GLfloat*) boneMatsWithGlobalInv[0]);
						glUniformMatrix4fv(shaderProgram->getUniformLoc("bone_matrices"), animator->boneMatrixCount, GL_FALSE, (GLfloat*) boneMatsWithGlobalInv);
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
		itoa(config.pointLightCount, count, 10);
		strcat(buf, count);
	}
}

ShaderProgram* AssimpRenderer::addShaderProgram(const char *tag, AssimpShaderConfig config) {
	ShaderProgram *shader = new ShaderProgram(tag, "assimp.vert", "assimp.frag");
	if (config.hwAccel) {
		shader->addDefine("HW_SKIN", "1");
	}
	if (config.withDirectionalLight) {
		shader->addDefine("DIR_LIGHT", "1");
	}
	if (config.pointLightCount) {
		char count[3];
		itoa(config.pointLightCount, count, 10);
		shader->addDefine("POINT_LIGHT_COUNT", count);
	}
	if (config.withFog) {
		shader->addDefine("FOG", "1");
	}
	if (config.withRGBAShadowmap) {
		shader->addDefine("SHADOWMAP", "1");
	}
	shader->load(TRUE);
	context->glResourceManager->addShaderProgram(tag, shader);
	return shader;
}
