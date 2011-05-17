#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Cooking.h"
#include <NxPhysicsSDK.h>
#include <NxPMap.h>
#include <PhysXLoader.h>

#if NX_SDK_VERSION_NUMBER >= 250
#define COOKING_INTERFACE 1
#endif

#ifdef COOKING_INTERFACE
NxCookingInterface* gCooking = 0;
#endif

bool cookingInitialised = false;

bool hasCookingLibrary(void) {// check to see if the cooking library is available or not!

	bool ret = true;

#ifdef COOKING_INTERFACE
	if (gCooking == 0) {
		gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
		if (gCooking == 0) ret = false;
	}
#endif

	return ret;

}

bool cookConvexMesh(const NxConvexMeshDesc& desc, NxStream& stream) {

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) return false;
	return gCooking->NxCookConvexMesh(desc,stream);
#else
	return NxCookConvexMesh(desc,stream);
#endif

}

bool cookClothMesh(const NxClothMeshDesc& desc, NxStream& stream) {

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) return false;
 	return gCooking->NxCookClothMesh(desc,stream);
#else
	return NxCookClothMesh(desc,stream);
#endif

}

bool cookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream) {

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) return false;
	return gCooking->NxCookTriangleMesh(desc,stream);
#else
	return NxCookTriangleMesh(desc,stream);
#endif

}

bool cookSoftBodyMesh(const NxSoftBodyMeshDesc& desc, NxStream& stream) {

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) return false;
	return gCooking->NxCookSoftBodyMesh(desc,stream);
#else
	return NxCookSoftBodyMesh(desc,stream);
#endif

}

bool initCooking(NxUserAllocator* allocator, NxUserOutputStream* outputStream) {

	if (cookingInitialised) return true;

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) cookingInitialised = false;
	else cookingInitialised = gCooking->NxInitCooking(allocator, outputStream);
#else
	cookingInitialised = NxInitCooking(allocator, outputStream);
#endif

	return cookingInitialised;

}

void closeCooking() {

	if (!cookingInitialised) return;

#ifdef COOKING_INTERFACE
	if (!gCooking) cookingInitialised = false;
	gCooking->NxCloseCooking();
	cookingInitialised = false;
#else
	cookingInitialised = NxCloseCooking();
#endif

}

bool createPMap(NxPMap& pmap, const NxTriangleMesh& mesh, NxU32 density, NxUserOutputStream* outputStream) {

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) return false;
	return gCooking->NxCreatePMap(pmap,mesh,density,outputStream);
#else
	return NxCreatePMap(pmap,mesh,density,outputStream);
#endif

}

bool releasePMap(NxPMap& pmap) {

#ifdef COOKING_INTERFACE
	hasCookingLibrary();
	if (!gCooking) return false;
	return gCooking->NxReleasePMap(pmap);
#else
	 return NxReleasePMap(pmap);
#endif

}
