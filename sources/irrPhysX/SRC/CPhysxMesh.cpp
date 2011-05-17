#include "CPhysxMesh.h"

namespace IrrPhysx {

CPhysxMesh::CPhysxMesh(NxScene* scene, E_MESH_TYPE type, const core::vector3df& scale) : IPhysxMesh(type, scale) {

	Scene = scene;

}

} // end IrrPhysx namespace
