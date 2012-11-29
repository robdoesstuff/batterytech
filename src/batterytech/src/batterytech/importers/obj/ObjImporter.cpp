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

#include "ObjImporter.h"

#include "ObjScene.h"
#include "../../util/TextFileUtil.h"
#include "../../util/strx.h"
#include <string.h>
#include <stdlib.h>

namespace BatteryTech {

#define LINE_SIZE 512
#define WHITESPACE " \t\n\r"

ObjImporter::ObjImporter() {
	metaScene = NULL;
	errorMessage = NULL;
}

ObjImporter::~ObjImporter() {
	if (metaScene) {
		delete metaScene;
	}
	metaScene = NULL;
}

void ObjImporter::freeMetaScene() {
	if (metaScene) {
		for (S32 i = 0; i < metaScene->groupCount; i++) {
			ObjMetaGroup *metaGroup = &metaScene->groups[i];
			for (S32 j = 0; j < metaGroup->meshCount; j++) {
				ObjMetaMesh *metaMesh = &metaGroup->meshes[j];
				delete [] metaMesh->faces;
				delete [] metaMesh->material;
			}
			delete [] metaGroup->name;
			delete [] metaGroup->meshes;
		}
		delete [] metaScene->groups;
		delete [] metaScene->verts;
		delete [] metaScene->normals;
		delete [] metaScene->tex;
		delete metaScene;
	}
	metaScene = NULL;
}

ObjScene* ObjImporter::import(const char *objText) {
	if (metaScene) {
		delete metaScene;
	}
	metaScene = new ObjMetaScene;
	// first passes - count and allocate
	S32 groups = countGroups(objText);
	metaScene->groups = new ObjMetaGroup[groups];
	metaScene->groupCount = groups;
	allocateMetaMeshes(objText);
	preParse(objText);
	// second pass - import primitives
	parse(objText);
	logMetaScene();
	// TODO - import any materials
	ObjScene *scene = convertMetaToScene();
	// process - unwind totalFaceCount into GL-compatible arrays
	//         - combine material and other data into end-structures
	// clean up meta allocations
	freeMetaScene();
	scene->log();
	return scene;
}

S32 ObjImporter::countGroups(const char *objText) {
	char line[LINE_SIZE];
	S32 filePos = 0;
	char *current_token = NULL;
	S32 line_number = 0;
	BOOL32 success = TextFileUtil::readLine(line, objText, &filePos);
	S32 groups = 0;
	//parser loop
	while (success) {
		current_token = strtok(line, " \t\n\r");
		line_number++;
		if (current_token != NULL && strEquals(current_token, "g")) {
			groups++;
		} else if (current_token != NULL && strEquals(current_token, "f")) {
			if (groups == 0) {
				// add default group
				groups++;
			}
		}
		success = TextFileUtil::readLine(line, objText, &filePos);
	}
	return groups;
}

// parse the obj reading in all primitives and setting primitive indices in faces of meshes
// allocate meshes by group
void ObjImporter::allocateMetaMeshes(const char *objText) {
	// allocate metaMeshes for each group
	char line[LINE_SIZE];
	S32 filePos = 0;
	char *current_token = NULL;
	S32 line_number = 0;
	BOOL32 success = TextFileUtil::readLine(line, objText, &filePos);
	S32 groupIdx = -1;
	S32 meshes = 0;
	BOOL32 materialChanged = FALSE;
	BOOL32 facesStarted = FALSE;
	//parser loop
	while (success) {
		current_token = strtok(line, " \t\n\r");
		line_number++;
		if (current_token == NULL || current_token[0] == '#') {
			// comment - do nothing.
		} else if (strEquals(current_token, "g")) {
			if (groupIdx != -1) {
				metaScene->groups[groupIdx].meshes = new ObjMetaMesh[meshes];
				metaScene->groups[groupIdx].meshCount = meshes;
			}
			groupIdx++;
			meshes = 1;
			materialChanged = FALSE;
			facesStarted = FALSE;
		} else if (strEquals(current_token, "f")) {
			if (materialChanged && facesStarted) {
				meshes++;
			}
			if (groupIdx == -1) {
				// use default group
				groupIdx++;
				meshes = 1;
			}
			facesStarted = TRUE;
			materialChanged = FALSE;
		} else if (strEquals(current_token, "usemtl")) {
			materialChanged = TRUE;
		}
		success = TextFileUtil::readLine(line, objText, &filePos);
	}
	// do last one.
	if (groupIdx != -1 && meshes > 0) {
		metaScene->groups[groupIdx].meshes = new ObjMetaMesh[meshes];
		metaScene->groups[groupIdx].meshCount = meshes;
	}
}

// Count everything and allocate face arrays by mesh
S32 ObjImporter::preParse(const char *objText) {
	char line[LINE_SIZE];
	S32 filePos = 0;
	char *current_token = NULL;
	S32 line_number = 0;
	BOOL32 success = TextFileUtil::readLine(line, objText, &filePos);
	S32 vertCount = 0;
	S32 normalCount = 0;
	S32 texCount = 0;
	S32 totalFaceCount = 0;
	S32 meshFaces = 0;
	// the current mesh group
	S32 groupIdx = -1;
	// the current mesh in the group
	S32 meshIdx = 0;
	// if the material has changed mid-group
	BOOL32 materialChanged = FALSE;
	// if we've seen totalFaceCount yet for the group
	BOOL32 facesStarted = FALSE;
	//parser loop
	while (success) {
		current_token = strtok(line, " \t\n\r");
		line_number++;
		if (current_token == NULL || current_token[0] == '#') {
		} else if (strEquals(current_token, "v")) {
			vertCount++;
		} else if (strEquals(current_token, "vn")) {
			normalCount++;
		} else if (strEquals(current_token, "vt")) {
			texCount++;
		} else if (strEquals(current_token, "f")) {
			if (materialChanged && facesStarted) {
				metaScene->groups[groupIdx].meshes[meshIdx].faces = new ObjMetaFace[meshFaces];
				metaScene->groups[groupIdx].meshes[meshIdx].faceCount = meshFaces;
				meshIdx++;
			}
			if (groupIdx == -1) {
				// use default group
				groupIdx++;
				meshIdx = 0;
			}
			materialChanged = FALSE;
			facesStarted = TRUE;
			totalFaceCount++;
			meshFaces++;
		} else if (strEquals(current_token, "sp")) {
			//process sphere
		} else if (strEquals(current_token, "pl")) {
			//process plane
		} else if (strEquals(current_token, "p")) {
			//process point
		} else if (strEquals(current_token, "lp")) {
			//light point source
		} else if (strEquals(current_token, "ld")) {
			//process light disc
		} else if (strEquals(current_token, "lq")) {
			//process light quad
		} else if (strEquals(current_token, "c")) {
			//camera
		} else if (strEquals(current_token, "usemtl")) {
			materialChanged = TRUE;
		} else if (strEquals(current_token, "mtllib")) {
			// mtllib
		} else if (strEquals(current_token, "o")) {
			// object name
		} else if (strEquals(current_token, "s")) {
			// smoothing
		} else if (strEquals(current_token, "g")) {
			if (groupIdx > -1) {
				metaScene->groups[groupIdx].meshes[meshIdx].faces = meshFaces ? new ObjMetaFace[meshFaces] : NULL;
				metaScene->groups[groupIdx].meshes[meshIdx].faceCount = meshFaces;
			}
			groupIdx++;
			meshIdx = 0;
			materialChanged = FALSE;
			facesStarted = FALSE;
			meshFaces = 0;
		} else {
			printf("Unknown command '%s' in scene code at line %i: \"%s\".\n",
					current_token, line_number, line);
		}
		success = TextFileUtil::readLine(line, objText, &filePos);
	}
	// do last one.
	if (groupIdx != -1 && meshIdx > -1) {
		metaScene->groups[groupIdx].meshes[meshIdx].faces = meshFaces ? new ObjMetaFace[meshFaces] : NULL;
		metaScene->groups[groupIdx].meshes[meshIdx].faceCount = meshFaces;
	}
	metaScene->vertCount = vertCount;
	metaScene->normalCount = normalCount;
	metaScene->texCount = texCount;
	metaScene->totalFaceCount = totalFaceCount;
	metaScene->verts = new ObjVector3[vertCount];
	metaScene->normals = new ObjVector3[normalCount];
	metaScene->tex = new ObjUV[texCount];
	return 1;
}

// parse the obj reading in all primitives and setting primitive indices in faces of meshes
S32 ObjImporter::parse(const char *objText) {
	char line[LINE_SIZE];
	S32 filePos = 0;
	char *current_token = NULL;
	S32 line_number = 0;
	BOOL32 success = TextFileUtil::readLine(line, objText, &filePos);
	S32 vertIdx = 0;
	S32 normalIdx = 0;
	S32 texIdx = 0;
	S32 meshFaceIdx = 0;
	ObjVector3 *verts = metaScene->verts;
	ObjVector3 *normals = metaScene->normals;
	ObjUV *tex = metaScene->tex;
	// the current mesh group
	S32 groupIdx = -1;
	// the current mesh in the group
	S32 meshIdx = 0;
	// if the material has changed mid-group
	BOOL32 materialChanged = FALSE;
	// if we've seen totalFaceCount yet for the group
	BOOL32 facesStarted = FALSE;
	char currentMaterial[255];
	strcpy(currentMaterial, "default");
	//parser loop
	while (success) {
		current_token = strtok(line, WHITESPACE);
		line_number++;
		if (current_token == NULL || current_token[0] == '#') {
		} else if (strEquals(current_token, "v")) {
			verts[vertIdx] = parseVector();
			vertIdx++;
		} else if (strEquals(current_token, "vn")) {
			normals[normalIdx] = parseVector();
			normalIdx++;
		} else if (strEquals(current_token, "vt")) {
			tex[texIdx] = parseUV();
			texIdx++;
		} else if (strEquals(current_token, "f")) {
			if (materialChanged && facesStarted) {
				meshIdx++;
				meshFaceIdx = 0;
				metaScene->groups[groupIdx].meshes[meshIdx].material = strdup(currentMaterial);
			}
			if (groupIdx == -1) {
				// use default group
				groupIdx++;
				meshIdx = 0;
				meshFaceIdx = 0;
				metaScene->groups[groupIdx].name = strdup("default");
				metaScene->groups[groupIdx].meshes[meshIdx].material = strdup(currentMaterial);
			}
			ObjMetaFace *face = &metaScene->groups[groupIdx].meshes[meshIdx].faces[meshFaceIdx];
			face->vertCount = parseFaceIndices(face->vertIndices, face->texIndices, face->normalIndices);
			facesStarted = TRUE;
			materialChanged = FALSE;
			meshFaceIdx++;
		} else if (strEquals(current_token, "sp")) {
			//process sphere
		} else if (strEquals(current_token, "pl")) {
			//process plane
		} else if (strEquals(current_token, "p")) {
			//process point
		} else if (strEquals(current_token, "lp")) {
			//light point source
		} else if (strEquals(current_token, "ld")) {
			//process light disc
		} else if (strEquals(current_token, "lq")) {
			//process light quad
		} else if (strEquals(current_token, "c")) {
			//camera
		} else if (strEquals(current_token, "usemtl")) {
			materialChanged = TRUE;
			// this is temp string so don't dupe
			strcpy(currentMaterial, strtok(NULL, WHITESPACE));
			if (!facesStarted && groupIdx > -1) {
				// this is when it's g then usemtl immediately after before faces (max does this)
				free(metaScene->groups[groupIdx].meshes[meshIdx].material);
				metaScene->groups[groupIdx].meshes[meshIdx].material = strdup(currentMaterial);
			}
		} else if (strEquals(current_token, "mtllib")) {
			// mtllib
		} else if (strEquals(current_token, "o")) {
			// object name
		} else if (strEquals(current_token, "s")) {
			// smoothing
		} else if (strEquals(current_token, "g")) {
			groupIdx++;
			meshIdx = 0;
			materialChanged = FALSE;
			facesStarted = FALSE;
			meshFaceIdx = 0;
			metaScene->groups[groupIdx].name = strdup(strtok(NULL, WHITESPACE));
			metaScene->groups[groupIdx].meshes[meshIdx].material = strdup(currentMaterial);
		} else {
			printf("Unknown command '%s' in scene code at line %i: \"%s\".\n",
					current_token, line_number, line);
		}
		success = TextFileUtil::readLine(line, objText, &filePos);
	}
	return 1;
}

void ObjImporter::logMetaScene() {
}

ObjScene* ObjImporter::convertMetaToScene() {
	ObjMetaScene *metaScene = this->metaScene;
	ObjVector3 *verts = metaScene->verts;
	ObjVector3 *normals = metaScene->normals;
	ObjUV *tex = metaScene->tex;
	ObjScene *scene = new ObjScene();
	S32 sceneTotalVertAttributes = metaScene->totalFaceCount * 3;
	scene->vertCount = sceneTotalVertAttributes;
	scene->vertPositions = new F32[sceneTotalVertAttributes * 3];
	scene->normals = new F32[sceneTotalVertAttributes * 3];
	scene->uvs = new F32[sceneTotalVertAttributes * 2];
	memset(scene->vertPositions, 0, sceneTotalVertAttributes * 3 * 4);
	memset(scene->normals, 0, sceneTotalVertAttributes * 3 * 4);
	memset(scene->uvs, 0, sceneTotalVertAttributes * 2 * 4);
	S32 vIndex = 0;
	scene->groups = new ObjGroup[metaScene->groupCount];
	scene->groupCount = metaScene->groupCount;
	for (S32 i = 0; i < metaScene->groupCount; i++) {
		ObjMetaGroup *metaGroup = &metaScene->groups[i];
		ObjGroup *group = &scene->groups[i];
		group->name = strdup(metaGroup->name);
		group->meshCount = metaGroup->meshCount;
		group->meshes = new ObjMesh[group->meshCount];
		for (S32 j = 0; j < metaGroup->meshCount; j++) {
			ObjMetaMesh *metaMesh = &metaGroup->meshes[j];
			ObjMesh *mesh = &group->meshes[j];
			S32 faceCount = metaMesh->faceCount;
			mesh->materialName = strdup(metaMesh->material);
			// ONLY SUPPORTS TRIANGLES FOR NOW
			mesh->vertCount = faceCount * 3;
			// point to memory of main vert attribs
			mesh->vertPositions = &scene->vertPositions[vIndex * 3];
			mesh->normals = &scene->normals[vIndex * 3];
			mesh->uvs = &scene->uvs[vIndex * 2];
			// TODO - optimize vtable access to speed up
			for (S32 k = 0; k < faceCount; k++) {
				ObjMetaFace *face = &metaMesh->faces[k];
				if (face->vertCount < 3) {
					// TODO - ignore this face without screwing up indices
					errorMessage = "Invalid face with less than 3 verts found!";
				} else if (face->vertCount > 3) {
					errorMessage = "Face has more than 3 vertices - not supported, ignoring extras";
				}
				// TODO - convert quads to triangles
				for (S32 l = 0; l < 3; l++) {
					S32 vertIdx = face->vertIndices[l];
					S32 normalIdx = face->normalIndices[l];
					S32 uvIdx = face->texIndices[l];
					if (vertIdx < 0 || normalIdx < 0 || uvIdx < 0) {
						errorMessage = "Relative (negative) indexing is not supported";
					} else {
						scene->vertPositions[vIndex * 3] = verts[vertIdx - 1].x;
						scene->vertPositions[vIndex * 3 + 1] = verts[vertIdx - 1].y;
						scene->vertPositions[vIndex * 3 + 2] = verts[vertIdx - 1].z;
						if (normalIdx) {
							scene->normals[vIndex * 3] = normals[normalIdx - 1].x;
							scene->normals[vIndex * 3 + 1] = normals[normalIdx - 1].y;
							scene->normals[vIndex * 3 + 2] = normals[normalIdx - 1].z;
						}
						if (uvIdx) {
							scene->uvs[vIndex * 2] = tex[uvIdx - 1].u;
							scene->uvs[vIndex * 2 + 1] = tex[uvIdx - 1].v;
						}
					}
					vIndex++;
				}
			}
		}
	}
	if (vIndex > sceneTotalVertAttributes) {
		errorMessage = "Error importing - memory corruption has occured, brace for impact!";
	}
	return scene;
}

const char* ObjImporter::getError() {
	return errorMessage;
}

ObjVector3 ObjImporter::parseVector() {
	// looking to parse 3 float, eg 1.0 1.0 1.0
	ObjVector3 v;
	v.x = atof( strtok(NULL, WHITESPACE));
	v.y = atof( strtok(NULL, WHITESPACE));
	v.z = atof( strtok(NULL, WHITESPACE));
	return v;
}

ObjUV ObjImporter::parseUV() {
	// looking to parse 2 floats, eg 1.0 1.0
	ObjUV ObjUV;
	ObjUV.u = atof( strtok(NULL, WHITESPACE));
	ObjUV.v = 1.0f - atof( strtok(NULL, WHITESPACE)); // Obj V is flipped to GL
	return ObjUV;
}

S32 ObjImporter::parseFaceIndices(S32 *vertex_index, S32 *texture_index, S32 *normal_index) {
	// looking to parse the complex face line which has 4 forms:  1, 1/2, 1//2, 1/2/3
	// each of which can have multiple groups like 1/2/3 4/5/6 7/8/9 to make a triangle.
	char *temp_str;
	char *token;
	int vertex_count = 0;
	while( (token = strtok(NULL, WHITESPACE)) != NULL) {
		if (texture_index) {
			texture_index[vertex_count] = 0;
		}
		if (normal_index) {
			normal_index[vertex_count] = 0;
		}
		vertex_index[vertex_count] = atoi( token );
		if(strContains(token, "//")) {
			//normal only
			temp_str = strchr(token, '/');
			temp_str++;
			normal_index[vertex_count] = atoi( ++temp_str );
		} else if(strContains(token, "/")) {
			temp_str = strchr(token, '/');
			texture_index[vertex_count] = atoi( ++temp_str );
			if(strContains(temp_str, "/")) {
				temp_str = strchr(temp_str, '/');
				normal_index[vertex_count] = atoi( ++temp_str );
			}
		}
		vertex_count++;
	}
	return vertex_count;
}
}
