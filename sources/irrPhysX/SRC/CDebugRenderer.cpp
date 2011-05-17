
#include "CDebugRenderer.h"
#include "NxDebugRenderable.h"

namespace IrrPhysx {

CDebugRenderer::CDebugRenderer(video::IVideoDriver* driver) {
	
	this->driver = driver;

	material.Wireframe = true;
	material.Lighting = false;
	
}

void CDebugRenderer::renderData(const NxDebugRenderable& data, const video::SColor& colour) const {

	driver->setMaterial(material);
	driver->setTransform(video::ETS_WORLD, core::matrix4());

	u32 NbPoints = data.getNbPoints();
	if(NbPoints) {
		printf("want to draw points\n");
	}

	/*glLineWidth(1.0f);
	glColor4f(0.7f,0.7f,0.7f,1.0f);
	glDisable(GL_LIGHTING);

	unsigned int NbPoints = data.getNbPoints();
	if(NbPoints)
	{
		float* pVertList = new float[NbPoints*3];
    	float* pColorList = new float[NbPoints*4];
    	int vertIndex = 0;
    	int colorIndex = 0;
		const NxDebugPoint* Points = data.getPoints();
		while(NbPoints--)
		{
        	pVertList[vertIndex++] = Points->p.x;
        	pVertList[vertIndex++] = Points->p.y;
        	pVertList[vertIndex++] = Points->p.z;
        	pColorList[colorIndex++] = (float)((Points->color>>16)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)((Points->color>>8)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)(Points->color&0xff)/255.0f;
        	pColorList[colorIndex++] = 1.0f;
	      	Points++;
		}
		
		renderBuffer(pVertList, pColorList, GL_POINTS, data.getNbPoints());
    	
    	delete[] pVertList;
    	delete[] pColorList;
	}*/

	u32 numLines = data.getNbLines();
	if (numLines) {
		video::S3DVertex* pVertList = new video::S3DVertex[numLines*2];
		u16* pIndexList = new u16[numLines*2];
    	u32 vertIndex = 0;
		u32 indexIndex = 0;
		const NxDebugLine* lines = data.getLines();
		while(numLines--) {
			pIndexList[indexIndex++] = vertIndex;
        	pVertList[vertIndex].Pos.X = lines->p0.x;
        	pVertList[vertIndex].Pos.Y = lines->p0.y;
        	pVertList[vertIndex].Pos.Z = lines->p0.z;
			pVertList[vertIndex].Color = colour;
			vertIndex++;

			pIndexList[indexIndex++] = vertIndex;
        	pVertList[vertIndex].Pos.X = lines->p1.x;
        	pVertList[vertIndex].Pos.Y = lines->p1.y;
        	pVertList[vertIndex].Pos.Z = lines->p1.z;
			pVertList[vertIndex].Color = colour;
			vertIndex++;

			if (indexIndex >= 65530) { // 66536 index limit so before we reach it fire off a render and reset to zero
				driver->drawVertexPrimitiveList(pVertList, vertIndex, pIndexList, indexIndex / 2, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);
				indexIndex = 0;
				vertIndex = 0;
			}

			lines++;
		}

		driver->drawVertexPrimitiveList(pVertList, vertIndex, pIndexList, indexIndex / 2, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);

    	delete [] pVertList;
		delete [] pIndexList;
	}

	u32 numTris = data.getNbTriangles();
	if (numTris) {
		video::S3DVertex* pVertList = new video::S3DVertex[numTris * 3];
		u16* pIndexList = new u16[numTris * 3];
    	u32 vertIndex = 0;
		u32 indexIndex = 0;
		const NxDebugTriangle* triangles = data.getTriangles();
		while(numTris--) {
			pIndexList[indexIndex++] = vertIndex;
        	pVertList[vertIndex].Pos.X = triangles->p0.x;
        	pVertList[vertIndex].Pos.Y = triangles->p0.y;
        	pVertList[vertIndex].Pos.Z = triangles->p0.z;
			pVertList[vertIndex].Color = colour;
			vertIndex++;

			pIndexList[indexIndex++] = vertIndex;
        	pVertList[vertIndex].Pos.X = triangles->p1.x;
        	pVertList[vertIndex].Pos.Y = triangles->p1.y;
        	pVertList[vertIndex].Pos.Z = triangles->p1.z;
			pVertList[vertIndex].Color = colour;
			vertIndex++;

			pIndexList[indexIndex++] = vertIndex;
        	pVertList[vertIndex].Pos.X = triangles->p2.x;
        	pVertList[vertIndex].Pos.Y = triangles->p2.y;
        	pVertList[vertIndex].Pos.Z = triangles->p2.z;
			pVertList[vertIndex].Color = colour;
			vertIndex++;

	      	triangles++;
		}
		numTris = data.getNbTriangles();
		
		driver->drawIndexedTriangleList(pVertList, numTris * 3, pIndexList, numTris);
		  	
    	delete [] pVertList;
		delete [] pIndexList;
	}

}

} // end IrrPhysx namespace
