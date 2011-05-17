#ifndef INC_CPHYSXMANAGER_H
#define INC_CPHYSXMANAGER_H

#include "IPhysxManager.h"
#include "CDebugRenderer.h"
#include "CContactReport.h"
#include "CErrorStream.h"
#include "IrrPhysxTypes.h"
#include <NxPhysics.h>

namespace IrrPhysx {

class CRaycastReport;
class CCloth;

struct SExplosion {
	NxVec3 Position;
	f32 InitialForce;
	f32 CurrentForce;
	f32 CurrentRadius;
	f32 MaxRadius;
	f32 GrowthRate;
	f32 TimeToGo;
};

class CPhysxManager : public IPhysxManager {

	public:
		CPhysxManager(IrrlichtDevice* device, const SSceneDesc& sceneDesc, bool disableHardware = false);
		~CPhysxManager();

		void resetScene(const SSceneDesc& desc);
		void simulate(f32 elapsedTime);
		void fetchResults();
		void renderDebugData(const video::SColor& colour);
		void setDebugDataVisible(bool val);
		IPhysxObject* createBoxObject(const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(2,2,2), f32 density = 10.0f, const core::vector3df* initialVelocity = NULL);
		IPhysxObject* createMultiShapeObject(const core::array<core::aabbox3df>& boxes, const core::array<SSphereDesc>& spheres, const core::array<SCapsuleDesc>& capsules, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 density = 10.0f, const core::vector3df* initialVelocity = NULL);
		IPhysxObject* createSphereObject(const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 radius = 2.0f, f32 density = 10.0f, const core::vector3df* initialVelocity = NULL);
		IPhysxObject* createCapsuleObject(const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 radius = 2.0f, f32 height = 2.0f, f32 density = 10.0f, const core::vector3df* initialVelocity = NULL);
		IPhysxObject* createPlaneObject(f32 d, const core::vector3df& normal);
		IPhysxObject* createHeightfieldObject(s16* data, u32 width, u32 height, f32 yMultiplier, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(1,1,1), u32 tessellation = 0);
		IPhysxObject* createHeightfieldObject(scene::ITerrainSceneNode* terrain, u32 smoothFactor = 0);
		IPhysxObject* createConvexMeshObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 density = 10.0f);
		IPhysxObject* createTriangleMeshObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0));
		IClothPhysxObject* createClothObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(1,1,1), f32 density = 10.0f, f32 pressure = -1.0f, bool tearable = false);
		scene::ISceneNode* createClothSceneNode(scene::ISceneManager* smgr, IPhysxObject* clothObject);
		IPhysxMesh* createClothMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale = core::vector3df(1,1,1), bool tearable = false);
		IPhysxMesh* createConvexMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale = core::vector3df(1,1,1));
		IPhysxMesh* createTriangleMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale = core::vector3df(1,1,1));
		void removePhysxObject(IPhysxObject* object);
		bool removePhysxMesh(IPhysxMesh* mesh);
		void createExplosion(const core::vector3df& position, f32 radius, f32 force, f32 growthRate, f32 seconds);
		const core::array<SRaycastHitData>& raycastAllRigidObjects(const core::line3df& ray);
		IPhysxObject* raycastClosestObject(const core::line3df& ray, core::vector3df* intersectionPoint = NULL, core::vector3df* surfaceNormal = NULL, bool smoothedNormal = false, f32* distance = NULL, u32* faceIndex = NULL, core::vector2df* barycentricCoords = NULL);
		void setContactCallback(void (*cb)(IPhysxObject*, IPhysxObject*));
		void setGravity(const core::vector3df& vec);
		void getGravity(core::vector3df& vec);
		u32 getNumPhysxObjects();
		f32 getSkinWidth();

		inline void addObjectHitByRaycast(IPhysxObject* object, const NxVec3& worldImpact, const NxVec3& worldNormal) {
			SRaycastHitData data;
			data.Object = object;
			data.HitPosition = core::vector3df(worldImpact.x, worldImpact.y, worldImpact.z);
			data.SurfaceNormal = core::vector3df(worldNormal.x, worldNormal.y, worldNormal.z);
			RaycastHits.push_back(data);
		}

	private:
		NxPhysicsSDK* PhysicsSDK;
		NxScene* Scene;
		CContactReport* ContactReport;
		CRaycastReport* RaycastReport;
		CDebugRenderer* DebugRenderer;
		CErrorStream ErrorStream;
		bool HardwareSimulation;
		bool Simulating;
		core::array<IPhysxObject*> RemovalQueue;
		core::array<SRaycastHitData> RaycastHits;
		core::array<SExplosion*> Explosions;
		core::array<SExplosion*> ExplosionsAddQueue;
		NxBounds3* MaxBounds;

		IPhysxObject* createPhysxObject(NxActor* actor, E_OBJECT_TYPE type);
		IClothPhysxObject* createClothPhysxObject(CCloth* cloth);
		void updateExplosions(f32 elapsedTime);

};

} // end IrrPhysx namespace

#endif /* INC_CPHYSXMANAGER_H */
