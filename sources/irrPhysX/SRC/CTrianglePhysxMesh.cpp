#include "CTrianglePhysxMesh.h"

namespace IrrPhysx {

CTrianglePhysxMesh::CTrianglePhysxMesh(NxScene* scene, NxTriangleMesh* mesh, const core::vector3df& scale) : CPhysxMesh(scene, EMT_TRIANGLE, scale) {

	Mesh = mesh;

}

CTrianglePhysxMesh::~CTrianglePhysxMesh() {

	if (Mesh && Mesh->getReferenceCount() == 0)
		Scene->getPhysicsSDK().releaseTriangleMesh(*Mesh);

}

} // end IrrPhysx namespace
