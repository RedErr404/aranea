#ifndef INC_CTRIANGLEPHYSXMESH_H
#define INC_CTRIANGLEPHYSXMESH_H

#include "CPhysxMesh.h"

namespace IrrPhysx {

class CTrianglePhysxMesh : public CPhysxMesh {

	public:
		CTrianglePhysxMesh(NxScene* scene, NxTriangleMesh* mesh, const core::vector3df& scale);
		~CTrianglePhysxMesh();

		inline u32 getReferenceCount() { return Mesh->getReferenceCount(); }
		inline NxTriangleMesh* getTriangleMesh() { return Mesh; }

	private:
		NxTriangleMesh* Mesh;

};

} // end IrrPhysx namespace

#endif /* INC_CTRIANGLEPHYSXMESH_H */
