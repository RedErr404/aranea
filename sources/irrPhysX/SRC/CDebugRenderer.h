/*----------------------------------------------------------------------------*\
|
|						     AGEIA PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/

#ifndef INC_CDEBUGRENDERER_H
#define INC_CDEBUGRENDERER_H

#include <irrlicht.h>

using namespace irr;

class NxDebugRenderable;

namespace IrrPhysx {

class CDebugRenderer {
	
	public:
		CDebugRenderer(video::IVideoDriver* driver);
		void renderData(const NxDebugRenderable& data, const video::SColor& colour) const;

	private:
		video::IVideoDriver* driver;
		video::SMaterial material;

};

} // end IrrPhysx namespace

#endif /* INC_CDEBUGRENDERER_H */

//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005-2006 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
