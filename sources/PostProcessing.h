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
 
#ifndef _POSTPROCESSING_H_
#define _POSTPROCESSING_H_
 
#include <irrlicht.h>
using namespace irr;
 
class PostProcessing_SetShaderConstants : public video::IShaderConstantSetCallBack
{
	public:
		virtual void OnSetConstants(video::IMaterialRendererServices *services,
		                            s32 userdata);
		void setShaderParameters(f32 *paras){shaderparameters = paras;}
 
	private:
		f32 *shaderparameters;
};
 
/**
 * \brief Class which manages postprocessing effects
 *
 * To apply the effect, run
 * \code
 * driver->setRenderTarget(postprocessing->getFirstMap(), true, true, video::SColor(255,150,180,255));
 * \endcode
 * or similar before you render anything and run
 * \code
 * driver->setRenderTarget(0);
 * postprocessing->renderEffect();
 * \endcode
 * to actually run the shaders and render the result to the screen.
 */
class PostProcessing
{
	public:
		/**
		 * \brief Constructor
		 * \param smgr Scene manager which is used for the post progressing
		 * \param filename_gl Path to the GLSL script
		 * \param filename_dx Path to the HLSL script
		 * \param type_ps Type of the pixel shader
		 * \param res_x Horizontal resolution of the used texture
		 * \param res_y Vertical resolution of the used texture
		 */
		PostProcessing(scene::ISceneManager *smgr,
		               const c8 *filename_gl,
		               const c8 *filename_dx,
		               video::E_PIXEL_SHADER_TYPE type_ps,
		               s32 res_x,
		               s32 res_y);
 
		/**
		 * \brief Destructor
		 */
		~PostProcessing();
 
		/**
		 * \brief Adds another stage and inserts it after this stage
		 * \param filename_gl Path to the GLSL script
		 * \param filename_dx Path to the HLSL script
		 * \param type_ps Type of the pixel shader
		 * \param res_x Horizontal resolution of the used texture
		 * \param res_y Vertical resolution of the used texture
		 */
		PostProcessing *addMaterial(const c8 *filename_gl,
		                            const c8 *filename_dx,
		                            video::E_PIXEL_SHADER_TYPE type_ps,
		                            s32 res_x,
		                            s32 res_y);
 
		/**
		 * \brief Renders this postprocessing chain
		 */
		void renderEffect();
 
		/**
		 * \brief Sets the second texture
		 */
		void setSecondMap(video::ITexture *tex, video::E_TEXTURE_CLAMP mode)
		{
			secondmap = tex;
			material.setTexture(1,secondmap);
			material.TextureLayer[1].TextureWrap = mode;
		}
 
		/**
		 * \brief Sets the parameters of the shader
		 */
		void setShaderParameters(f32 para1 = 0, f32 para2 = 0, f32 para3 = 0, f32 para4 = 0,
			f32 para5 = 0, f32 para6 = 0, f32 para7 = 0, f32 para8 = 0)
		{
			shaderparameters[0] = para1;
			shaderparameters[1] = para2;
			shaderparameters[2] = para3;
			shaderparameters[3] = para4;
			shaderparameters[4] = para5;
			shaderparameters[5] = para6;
			shaderparameters[6] = para7;
			shaderparameters[7] = para8;
		}
 
		/**
		 * \brief Returns a pointer to the material
		 */
		video::SMaterial *getMaterial(){return(&material);}
 
		/**
		 * \brief Returns a pointer to the first texture
		 */
		video::ITexture *getFirstMap(){return(firstmap);}
		/**
		 * \brief Returns a pointer to the second texture
		 */
		video::ITexture *getSecondMap(){return(secondmap);}
 
 
	private:
		PostProcessing_SetShaderConstants shadercallback;
 
		scene::ISceneManager *scenemgr;
		video::IVideoDriver *driver;
		video::S3DVertex vertices[4];
		video::SMaterial material;
		s32 matid;
		f32 shaderparameters[8];
		PostProcessing *prevstage;
		PostProcessing *nextstage;
 
		video::ITexture *firstmap;
		video::ITexture *secondmap;
};
 
#endif
