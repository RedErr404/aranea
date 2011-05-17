#include "CBufferSceneNode.h"

namespace IrrPhysx {

CBufferSceneNode::CBufferSceneNode(video::S3DVertex* vertices, u32* numVertices, u32 maxVertices, u16* indices, u32* numIndices, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
		: scene::ISceneNode(parent, mgr, id), Vertices(vertices), NumVertices(numVertices), MaxVertices(maxVertices), Indices(indices), NumIndices(numIndices) {

	Material.Wireframe = false;
		
	// Can't do frustum culling due to the bounding box changing all the time
	setAutomaticCulling(scene::EAC_OFF);

	/*if (*NumVertices > 0) {
		Box.reset(Vertices[0].Pos);
		if (*NumVertices > 1) {
			for (u32 i = 1 ; i < *NumVertices ; ++i)
				Box.addInternalPoint(Vertices[i].Pos);
		}
	}*/
	
}

CBufferSceneNode::~CBufferSceneNode() {

	// We don't own the render buffers so shouldn't delete them here

}

void CBufferSceneNode::OnRegisterSceneNode() {

	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();

}

void CBufferSceneNode::render() {
	
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	driver->setTransform(video::ETS_WORLD, core::matrix4());
	driver->setMaterial(Material);
	driver->drawIndexedTriangleList(Vertices, *NumVertices, Indices, *NumIndices / 3);

}

} // end IrrPhysx namespace
