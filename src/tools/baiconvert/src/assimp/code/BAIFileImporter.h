

#ifndef BAI_FILE_IMPORTER_H_INC
#define BAI_FILE_IMPORTER_H_INC

#include "BaseImporter.h"
#include <vector>

struct aiMesh;
struct aiNode;

typedef unsigned short bai_u16;
typedef signed int bai_s32;
typedef unsigned int bai_u32;

// floats (real numbers)
typedef float bai_f32;

#define BAI_MAGIC 0x1BADD00D

/** .bai format:
 * bai_header
 * - token = 4 byte magic
 * - length = length of entire data including this header
 * - version = .bai version number
 * - numMeshes = number of meshes
 * - firstMeshOffset = data offset of first mesh from start of this header
 *
 * n meshes
 * - bai_meshheader
 * -- dataLength - data length of mesh data not including this header
 * -- numVerts - number of vertices
 * -- numFaces - number of faces, all faces are triangles
 * -- numBones - number of bones
 * -- nameLength - number of characters in name not including null
 * - vertex positions, 3 floats per vertex
 * - vertex normals, 3 floats per vertex
 * - vertex UVs, 3 floats per vertex
 * - faces, 3 short indices per face
 * - n bones
 * -- bai_meshboneheader
 * --- influences - number of verts this bone influences
 * --- nameLength - length of the bone name
 * -- weights, aiVertexWeight (4 byte vertex index plus float weight) of influences length
 * -- boneName, char of nameLength length
 * - meshName, char of nameLength length
 *
 * n materials
 * - bai_materialheader
 * -- numProperties - number of material properties
 * - n properties
 * - bai_materialpropheader
 * -- dataLength - number of bytes of data
 * -- keyLength - number of bytes of key not including null
 * -- index
 * -- symantec
 * -- type
 *
 * n nodes (written starting at root, through children, depth-first)
 * - bai_nodeheader
 * -- numChildren
 * -- numMeshes
 * -- nameLength
 * - mesh indices
 * - name
 *
 *
 * n animations
 * - bai_animationheader
 * -- duration - duration of animation
 * -- ticksPerSecond - rate of animation
 * -- nameLength - number of bytes in animation name
 * -- numChannels - number of animation channels
 * -- numMeshChannels - number of animation mesh channels
 * - name
 * - n animation channels
 * -- bai_animationchannelheader
 * --- nodeNameLength - numbers of bytes in node name
 * --- numPositionKeys - number of position keys
 * --- numRotationKeys - number of rotation keys
 * --- numScalingKeys - number of scaling keys
 * --- preState - state before playing animation
 * --- postState - state after animation ends
 * -- n positions
 * -- n rotations
 * -- n scales
 * -- node name
 * - n animation mesh channels
 * -- bai_animationmeshchannelheader
 * --- numKeys - number of keys
 * --- nameLength - number of bytes in name
 * -- n keys
 */

struct bai_header {
	bai_u32 token;
	bai_u32 length;
	bai_u32 version;
	bai_u32 numMeshes;
	bai_u32 firstMeshOffset;
	bai_u32 numMaterials;
	bai_u32 firstMaterialOffset;
	bai_u32 numNodes;
	bai_u32 firstNodeOffset;
	bai_u32 numAnimations;
	bai_u32 firstAnimationOffset;
};

struct bai_meshheader {
	bai_u32 dataLength;
	bai_u32 numVerts;
	bai_u32 numFaces;
	bai_u32 numBones;
	bai_u32 nameLength;
	bai_u32 materialIndex;
};

struct bai_meshboneheader {
	bai_u32 influences;
	bai_u32 nameLength;
};

struct bai_materialheader {
	bai_u32 numProperties;
};

struct bai_materialpropheader {
	// this data length is the length of the property data field - not the length of this record like the other structs!
	bai_u32 dataLength;
	bai_u32 keyLength;
	bai_u32 index;
	bai_u32 symantec;
	aiPropertyTypeInfo type;
};

struct bai_nodeheader {
	bai_u32 numChildren;
	bai_u32 numMeshes;
	bai_u32 nameLength;
};

struct bai_face_triangle {
	bai_u16 v1,v2,v3;
};

struct bai_animationheader {
	bai_f32 duration;
	bai_f32 ticksPerSecond;
	bai_u32 nameLength;
	bai_u32 numChannels;
	bai_u32 numMeshChannels;
};

struct bai_animationchannelheader {
	bai_u32 nodeNameLength;
	bai_u32 numPositionKeys;
	bai_u32 numRotationKeys;
	bai_u32 numScalingKeys;
	aiAnimBehaviour preState;
	aiAnimBehaviour postState;
};

struct bai_animationmeshchannelheader {
	bai_u32 nameLength;
	bai_u32 numKeys;
};

namespace Assimp
{

class BAIFileImporter :
	BaseImporter
{	
	friend class Importer;

protected:
	///	\brief	Default constructor
	BAIFileImporter();

	///	\brief	Destructor
	~BAIFileImporter();

public:
	/// \brief	Returns whether the class can handle the format of the given file. 
	/// \remark	See BaseImporter::CanRead() for details.
	bool CanRead( const std::string& pFile, IOSystem* pIOHandler, bool checkSig) const;

private:

	//! \brief	Appends the supported extention.
	void GetExtensionList(std::set<std::string>& extensions);

	//!	\brief	File import implementation.
	void InternReadFile(const std::string& pFile, aiScene* pScene, IOSystem* pIOHandler);
};

// ------------------------------------------------------------------------------------------------
//	
inline void BAIFileImporter::GetExtensionList(std::set<std::string>& extensions)
{
	extensions.insert("bai");
}

// ------------------------------------------------------------------------------------------------

} // Namespace Assimp

#endif
