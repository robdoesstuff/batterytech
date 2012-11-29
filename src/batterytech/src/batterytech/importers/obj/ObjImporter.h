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
// Name        : ObjImporter.cpp
// Description : Imports Wavefront .OBJ files into OpenGL-compatible formats
//============================================================================

#ifndef OBJIMPORTER_H_
#define OBJIMPORTER_H_
#include "../../primitives.h"
#include <string.h>

namespace BatteryTech {

struct ObjVector3 {
	F32 x,y,z;
};

struct ObjUV {
	F32 u,v;
};

struct ObjMetaFace {
	// all indices of master lists, length of 4 to accomodate quads which will be broken into triangles later
	S32 vertIndices[4];
	S32 normalIndices[4];
	S32 texIndices[4];
	S32 vertCount;
	ObjMetaFace() {
		// its a struct so we can zero out the entire length of it starting at the first variable
		memset(vertIndices, 0, sizeof(ObjMetaFace));
	}
};

struct ObjMetaMesh {
	char *material;
	ObjMetaFace *faces;
	S32 faceCount;
};

struct ObjMetaGroup {
	char *name;
	ObjMetaMesh *meshes;
	S32 meshCount;
};

struct ObjMetaScene {
	S32 vertCount;
	S32 normalCount;
	S32 texCount;
	S32 totalFaceCount;
	ObjMetaGroup *groups;
	S32 groupCount;
	ObjVector3 *verts;
	ObjVector3 *normals;
	ObjUV *tex;
};

class ObjScene;

class ObjImporter {
public:
	ObjImporter();
	virtual ~ObjImporter();
	ObjScene* import(const char *objText);
	const char* getError();
private:
	S32 countGroups(const char *objText);
	void allocateMetaMeshes(const char *objText);
	S32 preParse(const char *objText);
	S32 parse(const char *objText);
	ObjScene* convertMetaToScene();
	ObjVector3 parseVector();
	ObjUV parseUV();
	S32 parseFaceIndices(S32 *vertex_index, S32 *texture_index, S32 *normal_index);
	void logMetaScene();
	void freeMetaScene();
	ObjMetaScene *metaScene;
	const char *errorMessage;
};

}

#endif /* OBJIMPORTER_H_ */
