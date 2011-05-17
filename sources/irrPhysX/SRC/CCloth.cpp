
#include <stdio.h>
#include "CCloth.h"
#include "Streams.h"
#include "Cooking.h"

namespace IrrPhysx {

// -----------------------------------------------------------------------
CCloth::CCloth(NxScene* scene, NxClothDesc& desc, CClothPhysxMesh* mesh, const core::vector3df& scale)
  : VertexRenderBuffer(NULL), Scene(scene), Torn(false) {

	// Grab the pre-defined infos from the mesh
	IndexRenderBuffer = mesh->IndexRenderBuffer;
	MaxIndices = mesh->MaxIndices;
	MaxVertices = mesh->MaxVertices;
	NumTempTexCoords = mesh->NumTexCoords;
	TempTexCoords = mesh->TexCoords;

	// Store correct number to detect tearing mesh in time
	LastNumVertices = mesh->InitNumVertices;

	allocateReceiveBuffers(mesh->InitNumVertices, mesh->InitNumTriangles);

	desc.clothMesh = mesh->getClothMesh();
	desc.meshData = ReceiveBuffers;

	Cloth = Scene->createCloth(desc);

}

// -----------------------------------------------------------------------
CCloth::~CCloth() {

	if (Cloth) Scene->releaseCloth(*Cloth);
		
	delete [] ReceiveBuffers.parentIndicesBegin;
	ReceiveBuffers.setToDefault();

	// delete the rendering buffer
	delete [] VertexRenderBuffer;
	//delete [] IndexRenderBuffer;
	
}

// -----------------------------------------------------------------------
void CCloth::allocateReceiveBuffers(u32 numVertices, u32 numTriangles) {

	// here we setup the buffers through which the SDK returns the dynamic cloth data
	// we reserve more memory for vertices than the initial mesh takes
	// because tearing creates new vertices
	// the SDK only tears cloth as long as there is room in these buffers

	// Allocate Render Buffer for Vertices
	VertexRenderBuffer = new video::S3DVertex[MaxVertices];
	memset(VertexRenderBuffer, 0, sizeof(video::S3DVertex) * MaxVertices);
	
	ReceiveBuffers.verticesPosBegin         = &(VertexRenderBuffer[0].Pos.X);
	ReceiveBuffers.verticesNormalBegin      = &(VertexRenderBuffer[0].Normal.X);
	ReceiveBuffers.verticesPosByteStride    = sizeof(video::S3DVertex);
	ReceiveBuffers.verticesNormalByteStride = sizeof(video::S3DVertex);
	ReceiveBuffers.maxVertices              = MaxVertices;
	ReceiveBuffers.numVerticesPtr           = &NumVertices;

	// the number of triangles is constant, even if the cloth is torn
	NxU32 maxIndices = 3*numTriangles;
	ReceiveBuffers.indicesBegin             = IndexRenderBuffer;
	ReceiveBuffers.indicesByteStride        = sizeof(u16);
	ReceiveBuffers.maxIndices               = maxIndices;
	ReceiveBuffers.numIndicesPtr            = &NumIndices;

	// Copy Tex Coords from temp buffers to graphics buffer
	for (NxU32 i = 0; i < NumTempTexCoords; ++i)
		VertexRenderBuffer[i].TCoords = TempTexCoords[i];
		
	// remove pointer to temp buffer
	NumTempTexCoords = 0;
	TempTexCoords = NULL;	

	// the parent index information would be needed if we used textured cloth
	ReceiveBuffers.parentIndicesBegin       = new NxU32[MaxVertices];
	ReceiveBuffers.parentIndicesByteStride  = sizeof(NxU32);
	ReceiveBuffers.maxParentIndices         = MaxVertices;
	ReceiveBuffers.numParentIndicesPtr      = &NumParentIndices;

	ReceiveBuffers.dirtyBufferFlagsPtr = &MeshDirtyFlags;

	// init the buffers in case we want to draw the mesh 
	// before the SDK as filled in the correct values
	MeshDirtyFlags = 0;
	NumVertices = 0;
	NumIndices = 0;
	NumParentIndices = 0;

}

// -----------------------------------------------------------------------
void CCloth::updateTextureCoordinates() {

	NxU32 numVertices = *ReceiveBuffers.numVerticesPtr;
	NxU32 *parent = (NxU32 *)ReceiveBuffers.parentIndicesBegin + LastNumVertices;
	for (NxU32 i = LastNumVertices; i < numVertices; ++i, ++parent) {
		VertexRenderBuffer[i].TCoords.X = VertexRenderBuffer[*parent].TCoords.X;
		VertexRenderBuffer[i].TCoords.Y = VertexRenderBuffer[*parent].TCoords.Y;
	}

	Torn |= LastNumVertices != numVertices;
	LastNumVertices = numVertices;

}

void CCloth::update() {

	if (!(Cloth->getFlags() & NX_CLF_TEARABLE)) return;

	// Disable pressure if tearing occurs
	if (Torn && (Cloth->getFlags() & NX_CLF_PRESSURE)) {
		// Disable Pressure
		Cloth->setFlags(Cloth->getFlags() & ~NX_CLF_PRESSURE);
		Cloth->setPressure(0);

		// Reduce tearing factor
		NxReal oldTearing = Cloth->getTearFactor();
		oldTearing = (oldTearing - 1) / 3 + 1;
		Cloth->setTearFactor(oldTearing);

		// Reduce bending stiffness
		if (Cloth->getBendingStiffness() > 0.9f)
			Cloth->setBendingStiffness(0.2f);

		// Apply explosion in the middle of the cloth
		NxBounds3 bounds;
		Cloth->getWorldBounds(bounds);
		NxVec3 center;
		bounds.getCenter(center);
		NxReal radius = bounds.min.distance(bounds.max);
		Cloth->addForceAtPos(center, 7 * NxMath::pow(radius,3), radius, NX_IMPULSE);
	}

	updateTextureCoordinates();
	
}

} // end IrrPhysx namespace
