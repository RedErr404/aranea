#ifndef INC_CBUFFERSCENENODE_H
#define INC_CBUFFERSCENENODE_H

#include <irrlicht.h>

using namespace irr;

namespace IrrPhysx {

class CBufferSceneNode : public scene::ISceneNode {

	public:
		CBufferSceneNode(video::S3DVertex* vertices, u32* numVertices, u32 maxVertices, u16* indices, u32* numIndices, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id);
		~CBufferSceneNode();
		void OnRegisterSceneNode();
		void render();

		inline const core::aabbox3d<f32>& getBoundingBox() const { return Box; }
		inline u32 getMaterialCount() const { return 1; }
		inline video::SMaterial& getMaterial(u32 i) { return Material; }	

	private:
		core::aabbox3d<f32> Box;
		video::S3DVertex* Vertices;
		u32* NumVertices;
		u32 MaxVertices;
		u16* Indices;
		u32* NumIndices;
		video::SMaterial Material;

};

} // end IrrPhysx namespace

#endif /* INC_CBUFFERSCENENODE_H */
