/*
Copyright (c) 2008 Nils Daumann
 
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:
 
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
 
#include "PostProcessing.h"
 
void PostProcessing_SetShaderConstants::OnSetConstants(video::IMaterialRendererServices *services, s32 userdata)
{
	// set the materialparameters
	services->setPixelShaderConstant("vecValues",shaderparameters, 8);
	
	if(userdata == 1)
	{
		//set Textures for openGL Shaders
		int texture1 = 0;
		services->setPixelShaderConstant("texture1",(float*)&texture1, 1);
		int texture2 = 1;
		services->setPixelShaderConstant("texture2",(float*)&texture2, 1);
	}
}
 
PostProcessing::PostProcessing(scene::ISceneManager *smgr, const c8 *filename_gl,
	const c8 *filename_dx, video::E_PIXEL_SHADER_TYPE type_ps, s32 res_x, s32 res_y)
{
	driver = smgr->getVideoDriver();
 
	if(driver->getDriverType() == video::EDT_OPENGL || driver->getDriverType() == video::EDT_DIRECT3D9)
	{
		vertices[0] = video::S3DVertex(-1.0f, -1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 0.0f, 1.0f);
		vertices[1] = video::S3DVertex(-1.0f,  1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 0.0f, 0.0f);
		vertices[2] = video::S3DVertex( 1.0f,  1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 1.0f, 0.0f);
		vertices[3] = video::S3DVertex( 1.0f, -1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 1.0f, 1.0f);
		
		video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
		scenemgr = smgr;
 
		prevstage = NULL;
		nextstage = NULL;
 
		setShaderParameters();
		shadercallback.setShaderParameters(&shaderparameters[0]);
 
		if(driver->getDriverType() == video::EDT_OPENGL)
		{
			matid = gpu->addHighLevelShaderMaterialFromFiles
			(
				"PP_GL_Vertex.fx",
				"main",
				video::EVST_VS_1_1,
				filename_gl,
				"main",
				type_ps,
				&shadercallback,
				video::EMT_SOLID,
				1
			);
		}else
		{
			matid = gpu->addHighLevelShaderMaterialFromFiles
			(
				"PP_DX_Vertex.fx",
				"main",
				video::EVST_VS_1_1,
				filename_dx,
				"main",
				type_ps,
				&shadercallback,
				video::EMT_SOLID,
				0
			);
		}
 
		firstmap = driver->addRenderTargetTexture(core::dimension2d<s32>(res_x,res_y));
		secondmap = NULL;
		material.Wireframe = false;
		material.Lighting = false;
		material.setTexture(0,firstmap);
		material.TextureLayer[0].TextureWrap = video::ETC_CLAMP;
		material.MaterialType = (video::E_MATERIAL_TYPE)matid;
	}
}
 
PostProcessing::~PostProcessing()
{
	if(nextstage != NULL)
	{
		delete nextstage;
	}
}
 
PostProcessing *PostProcessing::addMaterial(const c8 *filename_gl, const c8 *filename_dx,
	video::E_PIXEL_SHADER_TYPE type_ps, s32 res_x, s32 res_y)
{
	nextstage = new PostProcessing(scenemgr,filename_gl,filename_dx,type_ps,res_x,res_y);
 
	return nextstage;
}
 
void PostProcessing::renderEffect()
{
	u16 indices[] = {0,1,2,0,2,3};
	driver->setMaterial(material);
 
	if(nextstage != NULL)
	{
		driver->setRenderTarget(nextstage->getFirstMap(), true, true, video::SColor(255,150,180,255));
		driver->drawIndexedTriangleList(vertices, 6, indices, 2);
		driver->setRenderTarget(0);
 
		nextstage->renderEffect();
	}else
	{
		driver->drawIndexedTriangleList(vertices, 6, indices, 2);
	}
}
