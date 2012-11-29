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
// Name        : ObjScene.cpp
// Description : An OpenGL-compatible representation of an OBJ file
//============================================================================

#ifndef OBJSCENE_H_
#define OBJSCENE_H_

#include "../../primitives.h"

namespace BatteryTech {

// A renderable mesh.  Contains pointers to vertex attributes for just this mesh and also the offset (multiply by vector length)
// of the same vertex attributes in the scene's master list which allows for a single binding/vbo for the entire scene.
// Multiply index by 3 to get the memory address for positions and normals.  Multiply by 2 for uvs.
struct ObjMesh {
	char *materialName;
	// pointer to where the vertex positions begin in the scene's array
	F32 *vertPositions;
	// pointer to where the vertex normals begin in the scene's array
	F32 *normals;
	// pointer to where the vertex uvs begin in the scene's array
	F32 *uvs;
	// how many vertices this mesh has
	S32 vertCount;
	// which vertex in the scene is the first for this mesh
	S32 sceneVOffset;
};

// A group of geometry - used by most exporters for each individual geometric shape.
struct ObjGroup {
	char *name;
	ObjMesh *meshes;
	S32 meshCount;
};

// Represents everything imported from a wavefront OBJ file.
// All vertex attributes are available at the top level or can be bound to per-mesh if desired.
class ObjScene {
public:
	ObjScene();
	virtual ~ObjScene();
	ObjGroup* getGroup(const char *name);
	void log();
	S32 getMemoryConsumed();
	ObjGroup *groups;
	S32 groupCount;
	// Positions - an array of 3 times the length of vertCount - x1, y1, z1, x2, y2, z2, etc...
	F32 *vertPositions;
	// Normals - an array of 3 times the length of vertCount - x1, y1, z1, x2, y2, z2, etc...
	F32 *normals;
	// UVs - an array of 2 times the length of vertCount - u1, v1, u2, v2, etc...
	F32 *uvs;
	// How many vertices are here in total.
	S32 vertCount;
};

}

#endif /* OBJSCENE_H_ */
