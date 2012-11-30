#include "AssimpPCH.h"
#ifndef ASSIMP_BUILD_NO_BAI_IMPORTER

#include "DefaultIOSystem.h"
#include "BAIFileImporter.h"

#define BAI_MAGIC 0x1BADD00D

namespace Assimp {

using namespace std;

// ------------------------------------------------------------------------------------------------
//	Default constructor
BAIFileImporter::BAIFileImporter() {
	DefaultIOSystem io;
}

// ------------------------------------------------------------------------------------------------
//	Destructor.
BAIFileImporter::~BAIFileImporter() {
}

// ------------------------------------------------------------------------------------------------
//	Returns true, if file is an obj file.
bool BAIFileImporter::CanRead(const std::string& pFile, IOSystem* pIOHandler,
		bool checkSig) const {
	if (!checkSig) //Check File Extension
	{
		return SimpleExtensionCheck(pFile, "bai");
	} else //Check file Header
	{
		unsigned int magic = BAI_MAGIC;
		return BaseImporter::CheckMagicToken(pIOHandler, pFile, &magic, 1,	0, 4);
	}
}

static bai_u32 readFromData(void *target, bai_u32 offset, const unsigned char *src, size_t size) {
	memcpy(target, src+offset, size);
	return offset+size;
}

static bai_u32 readNode(aiNode *node, bai_u32 offset, unsigned char *src) {
	bai_u32 myOffset = offset;
	bai_nodeheader nodeHeader;
	myOffset=readFromData(&nodeHeader, myOffset, src, sizeof(bai_nodeheader));
	node->mChildren = new aiNode*[nodeHeader.numChildren];
	node->mNumChildren = nodeHeader.numChildren;
	node->mNumMeshes = nodeHeader.numMeshes;
	node->mMeshes = new bai_u32[nodeHeader.numMeshes];
	node->mName.length = nodeHeader.nameLength;
	myOffset=readFromData(node->mMeshes, myOffset, src, nodeHeader.numMeshes*4);
	myOffset=readFromData(&node->mTransformation, myOffset, src, sizeof(aiMatrix4x4));
	myOffset=readFromData(node->mName.data, myOffset, src, nodeHeader.nameLength);
	node->mName.data[nodeHeader.nameLength] = '\0';
	// cout << "Found node " << node->mName.data << endl;
	for (bai_u32 i = 0; i < node->mNumChildren; i++) {
		node->mChildren[i] = new aiNode();
		node->mChildren[i]->mParent = node;
		myOffset=readNode(node->mChildren[i], myOffset, src);
	}
	return myOffset;
}

static void importBAI(unsigned char *data, size_t dataLength, aiScene *scene) {
	cout << "********** Import BAI ****************" << endl;
	bai_header mainHeader;
	bai_u32 offset = 0;
	offset=readFromData(&mainHeader, offset, data, sizeof(bai_header));
	if (mainHeader.token == BAI_MAGIC) {
		cout << "magic number matches" << endl;
	} else {
		cout << "magic number mismatch" << endl;
		return;
	}
	cout << "data length = " << mainHeader.length << endl;
	cout << "version = " << mainHeader.version << endl;
	cout << "numMeshes = " << mainHeader.numMeshes << endl;
	cout << "numMaterials = " << mainHeader.numMaterials << endl;
	cout << "numNodes = " << mainHeader.numNodes << endl;
	cout << "numAnimations = " << mainHeader.numAnimations << endl;
	scene->mNumMeshes = mainHeader.numMeshes;
	scene->mMeshes = new aiMesh*[mainHeader.numMeshes];
	for (bai_u32 i = 0; i < mainHeader.numMeshes; i++) {
		bai_meshheader meshHeader;
		offset=readFromData(&meshHeader, offset, data, sizeof(bai_meshheader));
		aiMesh *mesh = new aiMesh();
		mesh->mNumVertices = meshHeader.numVerts;
		mesh->mNumFaces = meshHeader.numFaces;
		mesh->mNumBones = meshHeader.numBones;
		mesh->mVertices = new aiVector3D[meshHeader.numVerts];
		mesh->mNormals = new aiVector3D[meshHeader.numVerts];
		mesh->mTextureCoords[0] = new aiVector3D[meshHeader.numVerts];
		mesh->mMaterialIndex = meshHeader.materialIndex;
		offset=readFromData(mesh->mVertices, offset, data, meshHeader.numVerts*sizeof(aiVector3D));
		offset=readFromData(mesh->mNormals, offset, data, meshHeader.numVerts*sizeof(aiVector3D));
		for (bai_u32 j = 0; j < mesh->mNumVertices; j++) {
			// read 2 UV values back into 3-value element
			offset=readFromData(&mesh->mTextureCoords[0][j].x, offset, data, sizeof(float));
			offset=readFromData(&mesh->mTextureCoords[0][j].y, offset, data, sizeof(float));
			mesh->mTextureCoords[0][j].z = 0;
		}
		mesh->mFaces = new aiFace[mesh->mNumFaces];
		for (bai_u32 j = 0; j < mesh->mNumFaces; j++) {
			bai_face_triangle ft;
			offset=readFromData(&ft, offset, data, sizeof(ft));
			aiFace *face = &mesh->mFaces[j];
			face->mNumIndices = 3;
			face->mIndices = new unsigned int[3];
			face->mIndices[0] = ft.v1;
			face->mIndices[1] = ft.v2;
			face->mIndices[2] = ft.v3;
		}
		mesh->mBones = new aiBone*[mesh->mNumBones];
		for (bai_u32 j = 0; j < mesh->mNumBones; j++) {
			bai_meshboneheader boneHeader;
			offset=readFromData(&boneHeader, offset, data, sizeof(bai_meshboneheader));
			aiBone *bone = mesh->mBones[j];
			bone->mNumWeights = boneHeader.influences;
			bone->mWeights = new aiVertexWeight[bone->mNumWeights];
			bone->mName.length = boneHeader.nameLength;
			offset=readFromData(bone->mWeights, offset, data, sizeof(aiVertexWeight)*bone->mNumWeights);
			offset=readFromData(bone->mName.data, offset, data, boneHeader.nameLength);
			bone->mName.data[boneHeader.nameLength] = '\0';
			offset=readFromData(&bone->mOffsetMatrix, offset, data, sizeof(aiMatrix4x4));
			mesh->mBones[j] = bone;
		}
		offset=readFromData(mesh->mName.data, offset, data, meshHeader.nameLength);
		mesh->mName.length = meshHeader.nameLength;
		mesh->mName.data[meshHeader.nameLength] = '\0';
		// cout << "Read in mesh name " << mesh->mName.data << endl;
		scene->mMeshes[i] = mesh;
	}
	// aiMaterials
	scene->mNumMaterials = mainHeader.numMaterials;
	scene->mMaterials = new aiMaterial*[mainHeader.numMaterials];
	for (bai_u32 i = 0; i < mainHeader.numMaterials; i++) {
		aiMaterial *mat = new aiMaterial();
		bai_materialheader matHeader;
		offset=readFromData(&matHeader, offset, data, sizeof(matHeader));
		mat->mNumAllocated = matHeader.numProperties;
		mat->mNumProperties = matHeader.numProperties;
		mat->mProperties = new aiMaterialProperty*[matHeader.numProperties];
		for (bai_u32 j = 0; j < mat->mNumProperties; j++) {
			aiMaterialProperty *prop = new aiMaterialProperty();
			bai_materialpropheader propHeader;
			offset=readFromData(&propHeader, offset, data, sizeof(propHeader));
			prop->mDataLength = propHeader.dataLength;
			prop->mIndex = propHeader.index;
			prop->mSemantic = propHeader.symantec;
			prop->mType = propHeader.type;
			prop->mData = new char[prop->mDataLength];
			// put null on end?
			offset=readFromData(prop->mData, offset, data, prop->mDataLength);
			offset=readFromData(prop->mKey.data, offset, data, propHeader.keyLength);
			prop->mKey.length = propHeader.keyLength;
			prop->mKey.data[propHeader.keyLength] = '\0';
			//cout << "read in material " << prop->mKey.data << endl;
			mat->mProperties[j] = prop;
		}
		scene->mMaterials[i] = mat;
	}
	// aiNodes
	if (mainHeader.numNodes > 0) {
		scene->mRootNode = new aiNode();
		offset=readNode(scene->mRootNode, offset, data);
	} else {
		scene->mRootNode = NULL;
	}
	cout << "Read complete expected = " << mainHeader.length << " actual = " << offset << endl;
}

// ------------------------------------------------------------------------------------------------
//	Obj-file import implementation
void BAIFileImporter::InternReadFile(const std::string& pFile, aiScene* pScene,
		IOSystem* pIOHandler) {
	DefaultIOSystem io;

	// Read file into memory
	const std::string mode = "rb";
	boost::scoped_ptr<IOStream> file(pIOHandler->Open(pFile, mode));
	if (NULL == file.get())
		//throw DeadlyImportError( "Failed to open file " + pFile + ".");
		return;

	// Get the filesize and validate it, throwing an exception when failes
	const size_t fileSize = file->FileSize();
	if (fileSize < 16)
		return;
	std::vector<unsigned char> data;
	data.reserve(fileSize + 1);
	data.resize(fileSize);
	if (fileSize != file.get()->Read(&data[0], 1, fileSize)) {
		ASSIMP_THROW_EXCEPTION(DeadlyImportError("File read error"));
	}
	unsigned char *rawData = &(data.front()) ;
	// now read from data into pScene
	importBAI(rawData, fileSize, pScene);
	data.clear();
}

// ------------------------------------------------------------------------------------------------

} // Namespace Assimp

#endif // !! ASSIMP_BUILD_NO_BAI_IMPORTER
