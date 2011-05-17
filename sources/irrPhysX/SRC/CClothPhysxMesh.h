#ifndef INC_CCLOTHPHYSXMESH_H
#define INC_CCLOTHPHYSXMESH_H

#include "CPhysxMesh.h"

namespace IrrPhysx {

class CClothPhysxMesh : public CPhysxMesh {

	public:
		CClothPhysxMesh(NxScene* scene, NxClothMesh* mesh, bool tearable = false, const core::vector3df& scale = core::vector3df(1,1,1));
		~CClothPhysxMesh();

		inline u32 getReferenceCount() { return Mesh->getReferenceCount(); }
		inline NxClothMesh* getClothMesh() { return Mesh; }
		inline bool isTearable() { return Tearable; }

		u32 MaxVertices;
		u32 MaxIndices;
		u32 NumTexCoords;
		core::vector2df* TexCoords;
		u16* IndexRenderBuffer;
		u32 InitNumVertices;
		u32 InitNumTriangles;
		
	private:
		NxClothMesh* Mesh;
		bool Tearable;
		
};

} // end IrrPhysx namespace

#endif /* INC_CCLOTHPHYSXMESH_H */
