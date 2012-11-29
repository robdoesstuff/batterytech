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
// Name        : GLAssimpMeshBinding.cpp
// Description : Open Asset Importer to OpenGL VBO Bindings
//============================================================================

#include "GLAssimpMeshBinding.h"
#include "../Logger.h"
#include "../util/strx.h"
#include <stdio.h>
#include "../batterytech.h"
#include "../Context.h"

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

static BOOL32 debugAssimp = FALSE;

namespace BatteryTech {

	GLAssimpMeshBinding::GLAssimpMeshBinding() {
		vertAttsVBOId = 0;
		vCount = 0;
		faceCount = 0;
		vertBasicAtts = NULL;
		vertSkinnedAtts = NULL;
		mesh = NULL;
		hasBones = FALSE;
		matDiffuse = Vector4f(1.0, 1.0, 1.0, 1.0);
		matAmbient = Vector4f(0.7, 0.7, 0.7, 1.0);
		matSpecular = Vector4f(0.7, 0.7, 0.7, 1.0);
		matEmissive = Vector4f(0,0,0,1);
		matOpacity = 1.0f;
		matShininess = 1.0f;
		matDiffuseTexture = NULL;
	}

	GLAssimpMeshBinding::~GLAssimpMeshBinding() {
		if (vertBasicAtts) {
			delete [] vertBasicAtts;
		}
		vertBasicAtts = NULL;
		if (vertSkinnedAtts) {
			delete [] vertSkinnedAtts;
		}
		vertSkinnedAtts = NULL;
		mesh = NULL;
		delete [] matDiffuseTexture;
		matDiffuseTexture = NULL;
	}

	void GLAssimpMeshBinding::load(const aiScene *scene, const aiMesh *mesh) {
		// only process if we haven't already loaded this - otherwise is just GL reload
		if (!this->mesh) {
			Property *prop = btGetContext()->appProperties->get("debug_assimp");
			if (prop) {
				debugAssimp = prop->getBoolValue();
			}
			this->mesh = mesh;
			hasBones = (mesh->mNumBones > 0);
			if (hasBones) {
				vertSkinnedAtts = new GLAssimpSkinnedMeshVertex[mesh->mNumVertices];
			} else {
				vertBasicAtts = new GLAssimpMeshVertex[mesh->mNumVertices];
			}
			if (hasBones) {
				for (U32 i = 0; i < mesh->mNumVertices; i++) {
					aiVector3D vert = mesh->mVertices[i];
					aiVector3D normal = mesh->mNormals[i];
					aiVector3D uv = mesh->mTextureCoords[0][i];
					vertSkinnedAtts[i].position = Vector3f(vert.x, vert.y, vert.z);
					vertSkinnedAtts[i].normal = Vector3f(normal.x, normal.y, normal.z);
					vertSkinnedAtts[i].uv = Vector2f(uv.x, uv.y);
					vertSkinnedAtts[i].bones = Vector4b(0, 0, 0, 0);
					vertSkinnedAtts[i].weights = Vector4f(0, 0, 0, 0);
				}
				U32 *weightCounts = new U32[mesh->mNumVertices];
				memset(weightCounts, 0, mesh->mNumVertices * 4);
				BOOL32 discardedWeights = FALSE;
				for (U8 i = 0; i < (U8)mesh->mNumBones; i++) {
					aiBone *bone = mesh->mBones[i];
					for (U32 j = 0; j < bone->mNumWeights; j++) {
						U32 vertIdx = bone->mWeights[j].mVertexId;
						F32 weight = bone->mWeights[j].mWeight;
						U32 weightNum = weightCounts[vertIdx];
						if (weightNum < 4) {
							vertSkinnedAtts[vertIdx].bones[weightNum] = i;
							vertSkinnedAtts[vertIdx].weights[weightNum] = weight;
							weightCounts[vertIdx]++;
						} else {
							discardedWeights = TRUE;
						}
					}
				}
				delete [] weightCounts;
				if (discardedWeights) {
					logmsg("AssimpBind Warning: Too many bone weights - discarding!");
				}
			} else {
				for (U32 i = 0; i < mesh->mNumVertices; i++) {
					aiVector3D vert = mesh->mVertices[i];
					aiVector3D normal = mesh->mNormals[i];
					aiVector3D uv = mesh->mTextureCoords[0][i];
					vertBasicAtts[i].position = Vector3f(vert.x, vert.y, vert.z);
					vertBasicAtts[i].normal = Vector3f(normal.x, normal.y, normal.z);
					vertBasicAtts[i].uv = Vector2f(uv.x, uv.y);
				}
			}
			loadMaterials(scene->mMaterials[mesh->mMaterialIndex]);
		}
		U16 *faceIndices = new U16[this->mesh->mNumFaces * 3];
		for (U32 i = 0; i < this->mesh->mNumFaces; i++) {
			aiFace face = this->mesh->mFaces[i];
			faceIndices[i*3] = face.mIndices[0];
			faceIndices[i*3+1] = face.mIndices[1];
			faceIndices[i*3+2] = face.mIndices[2];
		}
		loadGeometry(this->mesh, faceIndices);
		delete [] faceIndices;
	}

	void GLAssimpMeshBinding::loadMaterials(aiMaterial *material) {
		aiColor4D aiCol;
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&aiCol)) {
			matDiffuse.x = aiCol.r;
			matDiffuse.y = aiCol.g;
			matDiffuse.z = aiCol.b;
			matDiffuse.w = aiCol.a;
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, (aiColor4D*)&aiCol)) {
			matSpecular.x = aiCol.r;
			matSpecular.y = aiCol.g;
			matSpecular.z = aiCol.b;
			matSpecular.w = aiCol.a;
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&aiCol)) {
			matAmbient.x = aiCol.r;
			matAmbient.y = aiCol.g;
			matAmbient.z = aiCol.b;
			matAmbient.w = aiCol.a;
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&aiCol)) {
			matEmissive.x = aiCol.r;
			matEmissive.y = aiCol.g;
			matEmissive.z = aiCol.b;
			matEmissive.w = aiCol.a;
		}
		if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_OPACITY, &matOpacity)) {
			matOpacity = 1.0f;
		}
		if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &matShininess)) {
			matShininess = 1.0f;
		}
		aiString path;
		F32 blend;
		aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path, NULL, NULL, &blend, NULL, NULL, NULL);
		matDiffuseTexture = strDuplicate(path.data);
		if (debugAssimp) {
			char buf[1024];
			sprintf(buf, "Diffuse: %f %f %f %f", matDiffuse.x, matDiffuse.y, matDiffuse.z, matDiffuse.w);
			logmsg(buf);
			sprintf(buf, "Specular: %f %f %f %f", matSpecular.x, matSpecular.y, matSpecular.z, matSpecular.w);
			logmsg(buf);
			sprintf(buf, "Ambient: %f %f %f %f", matAmbient.x, matAmbient.y, matAmbient.z, matAmbient.w);
			logmsg(buf);
			sprintf(buf, "Emissive: %f %f %f %f", matEmissive.x, matEmissive.y, matEmissive.z, matEmissive.w);
			logmsg(buf);
			sprintf(buf, "Opacity: %f Shininess: %f", matOpacity, matShininess);
			logmsg(buf);
		}
	}

	void GLAssimpMeshBinding::unload(Context *context) {
		if (vertAttsVBOId) {
			glDeleteBuffers(1, &vertAttsVBOId);
		}
		vertAttsVBOId = 0;
		if (vertBasicAtts) {
			delete [] vertBasicAtts;
		}
		vertBasicAtts = NULL;
		if (vertSkinnedAtts) {
			delete [] vertSkinnedAtts;
		}
		vertSkinnedAtts = NULL;
	}

	void GLAssimpMeshBinding::loadGeometry(const aiMesh *mesh, U16 *faceIndices) {
		this->vCount = mesh->mNumVertices;
		this->faceCount = mesh->mNumFaces;
		GLuint bufferIds[2];
		glGenBuffers(2, bufferIds);
		vertAttsVBOId = bufferIds[0];
		faceIndicesVBOId = bufferIds[1];
		glBindBuffer(GL_ARRAY_BUFFER, vertAttsVBOId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceIndicesVBOId);
		if (!hasBones) {
			// basic static object
			glBufferData(GL_ARRAY_BUFFER, vCount * sizeof(GLAssimpMeshVertex), vertBasicAtts, GL_STATIC_DRAW);
		} else if (ASSIMP_GPU_ACCELERATED_RENDER) {
			// GPU accel animation
			glBufferData(GL_ARRAY_BUFFER, vCount * sizeof(GLAssimpSkinnedMeshVertex), vertSkinnedAtts, GL_STATIC_DRAW);
		} else {
			// CPU animation - dynamic draw - this may be slow because we are mixing static & dynamic in one interleaved array
			glBufferData(GL_ARRAY_BUFFER, vCount * sizeof(GLAssimpSkinnedMeshVertex), vertSkinnedAtts, GL_DYNAMIC_DRAW);
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * 2, faceIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void GLAssimpMeshBinding::updateDynamicDraw() {
		// Must call bindVertexAttributes first!
		if (hasBones && !ASSIMP_GPU_ACCELERATED_RENDER) {
			// inefficient - we should only be replacing the positions and normals... need a separate buffer for that.
			glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(GLAssimpSkinnedMeshVertex), vertSkinnedAtts);
		}
	}

	void GLAssimpMeshBinding::bindPositionsToShader(GLint attributeLoc) {
		if (hasBones) {
			glVertexAttribPointer(attributeLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLAssimpSkinnedMeshVertex), 0);
		} else {
			glVertexAttribPointer(attributeLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLAssimpMeshVertex), 0);
		}
	}

	void GLAssimpMeshBinding::bindNormalsToShader(GLint attributeLoc) {
		if (hasBones) {
			glVertexAttribPointer(attributeLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLAssimpSkinnedMeshVertex), BUFFER_OFFSET(sizeof(Vector3f)));
		} else {
			glVertexAttribPointer(attributeLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLAssimpMeshVertex), BUFFER_OFFSET(sizeof(Vector3f)));
		}
	}

	void GLAssimpMeshBinding::bindUVsToShader(GLint attributeLoc) {
		if (hasBones) {
			glVertexAttribPointer(attributeLoc, 2, GL_FLOAT, GL_FALSE, sizeof(GLAssimpSkinnedMeshVertex), BUFFER_OFFSET(sizeof(Vector3f) + sizeof(Vector3f)));
		} else {
			glVertexAttribPointer(attributeLoc, 2, GL_FLOAT, GL_FALSE, sizeof(GLAssimpMeshVertex), BUFFER_OFFSET(sizeof(Vector3f) + sizeof(Vector3f)));
		}
	}

	void GLAssimpMeshBinding::bindBonesToShader(GLint attributeLoc) {
		if (hasBones) {
			glVertexAttribPointer(attributeLoc, 4, GL_BYTE, GL_FALSE, sizeof(GLAssimpSkinnedMeshVertex), BUFFER_OFFSET(sizeof(Vector3f) + sizeof(Vector3f) + sizeof(Vector2f)));
		}
	}

	void GLAssimpMeshBinding::bindWeightsToShader(GLint attributeLoc) {
		if (hasBones) {
			glVertexAttribPointer(attributeLoc, 4, GL_FLOAT, GL_FALSE, sizeof(GLAssimpSkinnedMeshVertex), BUFFER_OFFSET(sizeof(Vector3f) + sizeof(Vector3f) + sizeof(Vector2f) + sizeof(Vector4b)));
		}
	}

	void GLAssimpMeshBinding::bindFaceIndices() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceIndicesVBOId);
	}

	void GLAssimpMeshBinding::bindVertexAttributes() {
		glBindBuffer(GL_ARRAY_BUFFER, vertAttsVBOId);
	}

	void GLAssimpMeshBinding::unbind() {
		// This hurts performance and is not necessary.
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void GLAssimpMeshBinding::invalidateGL() {
		faceIndicesVBOId = 0;
		vertAttsVBOId = 0;
	}
}

#endif
