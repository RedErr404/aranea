#ifndef INC_CPHYSXMESH_H
#define INC_CPHYSXMESH_H

#include "IPhysxMesh.h"
#include <NxPhysics.h>

namespace IrrPhysx {

class CPhysxMesh : public IPhysxMesh {

	public:
		CPhysxMesh(NxScene* scene, E_MESH_TYPE type, const core::vector3df& scale);
		
	protected:
		NxScene* Scene;

};

} // end IrrPhysx namespace

#endif /* INC_CPHYSXMESH_H */
