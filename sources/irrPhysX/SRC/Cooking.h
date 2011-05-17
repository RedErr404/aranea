#ifndef INC_COOKING_H
#define INC_COOKING_H

#include <NxCooking.h>

class NxPMap;
class NxTriangleMesh;
class NxUserOutputStream;

bool hasCookingLibrary(); // check to see if the cooking library is available or not!
bool initCooking(NxUserAllocator* allocator = NULL, NxUserOutputStream* outputStream = NULL);
void closeCooking();
bool cookConvexMesh(const NxConvexMeshDesc& desc, NxStream& stream);
bool cookClothMesh(const NxClothMeshDesc& desc, NxStream& stream);
bool cookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream);
bool cookSoftBodyMesh(const NxSoftBodyMeshDesc& desc, NxStream& stream);
bool createPMap(NxPMap& pmap, const NxTriangleMesh& mesh, NxU32 density, NxUserOutputStream* outputStream = NULL);
bool releasePMap(NxPMap& pmap);

#endif /* INC_COOKING_H */
