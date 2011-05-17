#include "CConvexPhysxMesh.h"

namespace IrrPhysx {

CConvexPhysxMesh::CConvexPhysxMesh(NxScene* scene, NxConvexMesh* mesh, const core::vector3df& scale) : CPhysxMesh(scene, EMT_CONVEX, scale) {

	Mesh = mesh;

}

CConvexPhysxMesh::~CConvexPhysxMesh() {

	if (Mesh && Mesh->getReferenceCount() == 0)
		Scene->getPhysicsSDK().releaseConvexMesh(*Mesh);

}

} // end IrrPhysx namespace
