#ifndef INC_CCONVEXPHYSXMESH_H
#define INC_CCONVEXPHYSXMESH_H

#include "CPhysxMesh.h"

namespace IrrPhysx {

class CConvexPhysxMesh : public CPhysxMesh {

	public:
		CConvexPhysxMesh(NxScene* scene, NxConvexMesh* mesh, const core::vector3df& scale);
		~CConvexPhysxMesh();

		inline u32 getReferenceCount() { return Mesh->getReferenceCount(); }
		inline NxConvexMesh* getConvexMesh() { return Mesh; }

	private:
		NxConvexMesh* Mesh;

};

} // end IrrPhysx namespace

#endif /* INC_CCONVEXPHYSXMESH_H */
