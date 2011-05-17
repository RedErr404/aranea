#include "CClothPhysxMesh.h"

namespace IrrPhysx {

CClothPhysxMesh::CClothPhysxMesh(NxScene* scene, NxClothMesh* mesh, bool tearable, const core::vector3df& scale) : CPhysxMesh(scene, EMT_CLOTH, scale) {

	Mesh = mesh;
	Tearable = tearable;

	MaxVertices = 0;
	MaxIndices = 0;
	NumTexCoords = 0;
	TexCoords = NULL;
	IndexRenderBuffer = NULL;
	InitNumVertices = 0;
	InitNumTriangles = 0;

}

CClothPhysxMesh::~CClothPhysxMesh() {

	if (Mesh && Mesh->getReferenceCount() == 0) 
		Scene->getPhysicsSDK().releaseClothMesh(*Mesh);

	delete [] TexCoords;
	TexCoords = NULL;

	//delete [] IndexRenderBuffer;
	//IndexRenderBuffer = NULL;

}

} // end IrrPhysx namespace
