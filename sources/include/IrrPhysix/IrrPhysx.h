#ifndef INC_IRRPHYSX_H
#define INC_IRRPHYSX_H

#include "IPhysxManager.h"
#include "IPhysxObject.h"
#include "IPhysxMesh.h"
#include "IClothPhysxObject.h"

#define IRRPHYSX_VERSION 0.2f

/*! \mainpage IrrPhysx 0.2 API documentation
 *
 * <div align="center"><img src="irrlichtirrphysxlogo.png" ></div>
 *
 * <div align="center"><img src="physxlogo.png" ></div>
 *
 * \section intro Introduction
 *
 * Welcome to the IrrPhysx API documentation.
 * Here you'll find any information you'll need to develop applications with
 * the IrrPhysx library. If you are looking for a tutorial on how to start, you'll
 * find some inside the SDK in the examples directory.
 *
 * IrrPhysx is intended to be an easy-to-use wrapper to use the Physx by Nvidia library with the Irrlicht Graphics Engine,
 * so this documentation is an important part of it. If you have any questions or
 * suggestions, just send a email to the author of the library, Chris Mash
 * (chris.mash (at) talk21.com).
 *
 *
 * \section links Links
 *
 * <A HREF="annotated.html">Class list</A>: List of all classes with descriptions.<BR>
 * <A HREF="functions.html">Class members</A>: Good place to find forgotten features.<BR>
 *
 */

namespace IrrPhysx {

	/**
	\brief Creates a new physx manager
	\param device - the IrrlichtDevice to use the manager with
	\param sceneDesc - the descrition of the Physx scene to be created
	\param disableHardware - whether or not to use any available accelerator hardware. This is useful is when a client and server app must be run on the same machine. Under normal circumstances the hardware will be used by the first application which attempts to use it. In the case of a client and server it is desirable to have the server run in software mode and allow the client to use the hardware
	\return a IPhysxManager*
	*/
	IPhysxManager* createPhysxManager(IrrlichtDevice* device, const SSceneDesc& sceneDesc, bool disableHardware = false);

	/**
	\brief Remove the specified physx manager when you're finished using it
	\param manager - the IPhysxManager to be removed
	*/
	void removePhysxManager(IPhysxManager* manager);

} // end IrrPhysx namespace

#endif /* INC_IRRPHYSX_H */
