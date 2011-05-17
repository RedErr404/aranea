
#ifndef INC_CCLOTH_H
#define INC_CCLOTH_H

#include "CClothPhysxMesh.h"
#include <NxPhysics.h>
#include <irrlicht.h>

using namespace irr;

namespace IrrPhysx {

class CCloth {

	public:
		CCloth(NxScene* scene, NxClothDesc& desc, CClothPhysxMesh* mesh, const core::vector3df& scale);
		~CCloth();

		void update();

		inline NxCloth* getNxCloth() { return Cloth; }
		inline NxMeshData* getMeshData() { return &ReceiveBuffers; }

	private:
		void allocateReceiveBuffers(u32 numVertices, u32 numTriangles);
		void updateTextureCoordinates();

		NxMeshData ReceiveBuffers;
		NxScene* Scene;
		NxCloth* Cloth;

		// The rendering buffers
		video::S3DVertex* VertexRenderBuffer;
		u16* IndexRenderBuffer;

		NxU32 MaxVertices;
		NxU32 MaxIndices;
		NxU32 NumIndices;
		NxU32 NumParentIndices;
		NxU32 NumVertices;
		NxU32 LastNumVertices;

		NxU32 MeshDirtyFlags;
		bool Torn;

		// Temporary storage for tex coords
		core::vector2df* TempTexCoords;
		NxU32 NumTempTexCoords;

};

} // end IrrPhysx namespace

#endif /* INC_CCLOTH_H */
