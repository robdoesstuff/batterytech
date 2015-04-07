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
// Name        : GLAssimpMeshBinding.h
// Description : Open Asset Importer to OpenGL VBO Bindings
//============================================================================

#ifndef GLASSIMPMESHBINDING_H_
#define GLASSIMPMESHBINDING_H_

#include "../batterytech_globals.h"
#include "../platform/platformgl.h"

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "../../bt-assimp/include/aiScene.h"
#include "../../bt-assimp/include/assimp.hpp"

namespace BatteryTech {

class Context;

// make sure interleaved structures are multiples of 32 bytes for optimal performance

struct GLAssimpMeshVertex {
	Vector3f position; // 12 bytes
	Vector3f normal; // 12 bytes
	Vector2f uv; // 8 bytes
	// exactly 32 bytes - no padding needed
};

struct GLAssimpSkinnedMeshVertex {
	Vector3f position; // 12 bytes
	Vector3f normal; // 12 bytes
	Vector2f uv; // 8 bytes
	Vector4b bones; // 4 bytes
	Vector4f weights; // 16 bytes
	// 52 bytes - pad to 64
	unsigned char padding[12];
};

class GLAssimpMeshBinding {
public:
	GLAssimpMeshBinding();
	virtual ~GLAssimpMeshBinding();
	void load(const aiScene *scene, const aiMesh *mesh);
	void unload(Context *context);
	void bindVertexAttributes();
	void bindPositionsToShader(GLint attributeLoc);
	void bindNormalsToShader(GLint attributeLoc);
	void bindUVsToShader(GLint attributeLoc);
	void bindBonesToShader(GLint attributeLoc);
	void bindWeightsToShader(GLint attributeLoc);
	void bindFaceIndices();
	void unbind();
	void loadGeometry(const aiMesh *mesh, U16 *faceIndices);
	void updateDynamicDraw();
	void invalidateGL();
	GLAssimpMeshVertex *vertBasicAtts;
	GLAssimpSkinnedMeshVertex *vertSkinnedAtts;
	Vector4f matDiffuse;
	Vector4f matSpecular;
	Vector4f matAmbient;
	Vector4f matEmissive;
	char *matDiffuseTexture;
	F32 matOpacity;
	F32 matShininess;
	GLuint vertAttsVBOId;
	GLuint faceIndicesVBOId;
	S32 vCount;
	S32 faceCount;
	const aiMesh *mesh;
	BOOL32 hasBones;
private:
	void loadMaterials(aiMaterial *material);
};

}

#endif

#endif /* GLASSIMPMESHBINDING_H_ */
