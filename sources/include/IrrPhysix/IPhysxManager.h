#ifndef INC_IPHYSXMANAGER_H
#define INC_IPHYSXMANAGER_H

#include "IPhysxObject.h"
#include "IClothPhysxObject.h"
#include "IPhysxMesh.h"
#include <irrlicht.h>

using namespace irr;

namespace IrrPhysx {

	/**
	Struct containing information on a raycast hit
	*/
	struct SRaycastHitData {
		/**
		\brief Constructor
		*/
		SRaycastHitData() {
			Object = NULL;
		}
		/**
		the object that was hit
		*/
		IPhysxObject* Object; 
		/**
		the world position it was hit at
		*/
		core::vector3df HitPosition; 
		/** 
		the world normal of the surface that was hit 
		*/
		core::vector3df SurfaceNormal; 
		/**
		the distance from the ray start to the intersection point
		*/
		f32 Distance; 
		/**
		the face index of the triangle hit from the original mesh buffer used to create the object
		*/
		u32 FaceIndex; 
		/**
		the barycentric coordinates of the intersection point 
		*/
		core::vector2df BarycentricCoords; 
	};

	/**
	Struct to describe a scene when creating it
	*/
	struct SSceneDesc {
		/**
		\brief Constructor
		*/
		SSceneDesc() {
			UpAxis = 0;
			BoundPlanes = false;
			Gravity.Y = -9.81f;
		}
		/**
		this accelerates scene-level collision queries (e.g. raycasting). When using MaxBounds, you have to make sure created objects stay within the scene bounds.
		In particular, the position of dynamic shapes should stay within the provided bounds. Otherwise the shapes outside the bounds will not be taken into account
		by all scene queries (raycasting, sweep tests, overlap tests, etc). They will nonetheless still work correctly for the main physics simulation
		*/
		core::aabbox3df MaxBounds;
		/**
		enable/disable 6 planes around MaxBounds (if available)
		*/
		bool BoundPlanes; 
		/**
		defines the up axis for your world. This is used to accelerate scene queries like raycasting or sweep tests.
		Internally, a 2D structure is used instead of a 3D one whenever an up axis is defined. This saves memory and is usually faster.
		Use 1 for Y = up, 2 for Z = up, or 0 to disable this feature. It is not possible to use X = up
		*/
		u32 UpAxis;
		/**
		defines the gravity for your world
		*/
		core::vector3df Gravity; 
	};

	/**
	Struct to describe a sphere
	*/
	struct SSphereDesc {
		/**
		the radius of the sphere
		*/
		f32 Radius; 
		/**
		the position of the sphere
		*/
		core::vector3df Position; 
	};

	/**
	Struct to describe a capsule
	*/
	struct SCapsuleDesc  {
		/**
		the radius of the capsule
		*/
		f32 Radius; 
		/**
		the height of the capsule; the distance between the centres of the hemispheres
		*/
		f32 Height; 
		/**
		the position of the capsule
		*/
		core::vector3df Position; 
	};

	/**
	Interface for Physx managers. Used to handle all the access to the Physx SDK
	*/
	class IPhysxManager {

		public:

			/**
			\brief Constructor. Don't use this, rather use createPhysxManager(IrrlichtDevice*)
			\param device - the IrrlichtDevice* for the manager to be used with
			*/
			IPhysxManager(IrrlichtDevice* device) { Device = device; DebugDataVisible = false; }
			/**
			\brief Destructor
			*/
			virtual ~IPhysxManager() {}
			/**
			\brief Sets whether Physx should prepare debug renderings or not
			\param vis - whether debug renderings should be prepared
			*/
			virtual void setDebugDataVisible(bool vis) { DebugDataVisible = vis; }
			/**
			\brief Returns whether Physx is set to prepare debug renderings or not
			\return a bool
			*/
			virtual bool isDebugDataVisible() { return DebugDataVisible; }
			
			/**
			\brief Resets the Physx scene

			This is useful for when you may be changing levels in your game and require the whole scene to be cleared out
			\param desc - the description of the scene to be created
			*/
			virtual void resetScene(const SSceneDesc& desc) = 0;
			/**
			\brief Starts Physx simulating the rigid body scene
			\param elapsedTime - the amount of time that has elapsed since the last simulation (in seconds)
			*/
			virtual void simulate(f32 elapsedTime) = 0;
			/**
			\brief Fetches the results of the rigid body scene simulation

			This is a blocking call and will wait until the simulation is complete
			*/
			virtual void fetchResults() = 0;
			/**
			\brief Renders the debug renderings, i.e. collision shapes

			This should be called after ISceneManager::drawAll() and IVideoDriver::beginScene(...) and before IGUIEnvironment::drawAll() and IVideoDriver::endScene()
			Debug data will only be rendered if isDebugDataVisible() returns true
			\param colour - the colour to render the debug renderings in
			*/
			virtual void renderDebugData(const video::SColor& colour = video::SColor(255,255,255,255)) = 0;
			/**
			\brief Creates a box Physx object

			Passing a density of 0 will cause the box to be created as a static, immovable object. If you don't wish the cube to have any initial velocity then
			simply pass NULL or 0.
			\param position - the position of the box
			\param rotation - the rotation of the box
			\param scale - the scale of the box
			\param density - the density of the box
			\param initialVelocity - the initial velocity of the box
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createBoxObject(const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(2,2,2), f32 density = 10.0f, const core::vector3df* initialVelocity = NULL) = 0;
			/**
			\brief Creates a multi shape Physx object made up of the specified axis-aligned bounding boxes, spheres and capsules

			Passing a density of 0 will cause the object to be created as a static, immovable object. If you don't wish the object to have any initial velocity then
			simply pass NULL or 0.
			\param boxes - the boxes to create shapes from
			\param spheres - the spheres to create shapes from
			\param capsules - the capsules to create shapes from
			\param position - the position of the object
			\param rotation - the rotation of the object
			\param density - the density of the object
			\param initialVelocity - the initial velocity of the object
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createMultiShapeObject(const core::array<core::aabbox3df>& boxes, const core::array<SSphereDesc>& spheres, const core::array<SCapsuleDesc>& capsules, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 density = 10.0f, const core::vector3df* initialVelocity = NULL) = 0;
			/**
			\brief Creates a sphere Physx object

			Passing a density of 0 will cause the cube to be created as a static, immovable object. If you don't wish the cube to have any initial velocity then
			simply pass NULL or 0.
			\param position - the position of the sphere
			\param rotation - the rotation of the sphere
			\param radius - the radius of the sphere
			\param density - the density of the sphere
			\param initialVelocity - the initial velocity of the sphere
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createSphereObject(const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 radius = 2.0f, f32 density = 10.0f, const core::vector3df* initialVelocity = NULL) = 0;
			/**
			\brief Creates a capsule Physx object
			\param position - the position of the capsule
			\param rotation - the rotation of the capsule
			\param radius - the radius of the capsule
			\param height - the height of the capsule
			\param density - the density of the capsule
			\param initialVelocity - the initial velocity of the capsule
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createCapsuleObject(const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 radius = 2.0f, f32 height = 2.0f, f32 density = 10.0f, const core::vector3df* initialVelocity = NULL) = 0;
			/**
			\brief Creates a plane Physx object with the specified parameters
			\param d - distance from origin
			\param normal - the normal of the plane
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createPlaneObject(f32 d, const core::vector3df& normal) = 0;
			/**
			\brief Creates a heightfield Physx object

			Note that the data being passed in must be an s16 array. Your heightmap data may be in f32 form and if so you can multiply each of the values by yMultiplier in order to get similar results to the f32 array
			Note also that an s16 can only hold numbers between -32768 and 32768 so don't multiply them out of that range!
			The tessellation parameter specifies which way the quad is split into triangles. This is defined by the data passed and will probably be down to a matter of trial and error. If you turn on Physx debug rendering and the heightfield mesh seems to have holes in it (is penetrating the visual representation) then changing this parameter may help
			\param data - the heightmap to create the heightfield from
			\param width - the width of the heightmap
			\param height - the height of the heightmap
			\param yMultiplier - the amount the data has been divided by
			\param position - the position of the heightfield
			\param rotation - the rotation of the heightfield
			\param scale - the scale of the heighfield
			\param tessellation - the tessellation of the heightfield quads, should be 0 or 1
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createHeightfieldObject(s16* data, u32 width, u32 height, f32 yMultiplier, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(1,1,1), u32 tessellation = 0) = 0;
			/**
			\brief Creates a heightfield Physx object from a ITerrainSceneNode
			\param terrain - the terrain node to create the heightfield from
			\param smoothFactor - the smoothing factor of the heightfield
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createHeightfieldObject(scene::ITerrainSceneNode* terrain, u32 smoothFactor = 0) = 0;
			/**
			\brief Creates a convex mesh Physx object

			There is a 256 polygon limit for convex meshes, though this only affects the resulting convex mesh rather than the input mesh
			The convex mesh's density must be larger than 0
			\param mesh - the mesh to create the convex mesh from
			\param position - the position of the convex mesh
			\param rotation - the rotation of the convex mesh
			\param density - the density of the convex mesh
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createConvexMeshObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), f32 density = 10.0f) = 0;
			/**
			\brief Creates a triangle mesh Physx object

			Triangle mesh objects are static objects that can't be moved
			\param mesh - the mesh to create the triangle mesh from
			\param position - the position of the triangle mesh
			\param rotation - the rotation of the triangle mesh
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* createTriangleMeshObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0)) = 0;
			/**
			\brief Creates a cloth Physx object from the specified IPhysxMesh

			Passing -1.0f for pressure turns off the pressure calculations.
			If pressure if desired it should be within the range of 0.0f to 1.0f and the IMesh should be completely enclosed so that the 'air' cannot leak out
			If all parts of the mesh buffer are not joined together then they will fall apart during simulation
			Passing a density of 0 will cause the cube to be created as a static, immovable object
			\param mesh - the mesh to create the cloth from
			\param position - the position of the cloth
			\param rotation - the rotation of the cloth
			\param scale - the scale of the cloth
			\param density - the density of the cloth
			\param pressure - the pressure of the cloth
			\param tearable - whether the cloth can be torn or not
			\return a IClothPhysxObject*
			*/
			virtual IClothPhysxObject* createClothObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(1,1,1), f32 density = 10.0f, f32 pressure = -1.0f, bool tearable = false) = 0;
			/**
			\brief Creates a cloth scene node from the specified cloth Physx object
			\param smgr - the scene manager to create the node with
			\param clothObject - the cloth Physx object to create the node for
			\return a ISceneNode*
			*/
			virtual scene::ISceneNode* createClothSceneNode(scene::ISceneManager* smgr, IPhysxObject* clothObject) = 0;
			/**
			\brief Creates a cloth mesh which can then be used to create a cloth object

			This function should be executed during a loading period in your application as it can take a fair amount of time
			\param meshBuffer - the mesh buffer to create the cloth mesh from
			\param scale - the scale to create the cloth mesh with
			\param tearable - whether this cloth mesh should be tearable
			*/
			virtual IPhysxMesh* createClothMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale = core::vector3df(1,1,1), bool tearable = false) = 0;
			/**
			\brief Creates a convex mesh which can then be used to create a convex mesh object

			This function should be executed during a loading period in your application as it can take a fair amount of time
			\param meshBuffer - the meshBuffer to create the convex mesh from
			\param scale - the scale to create the convex mesh with
			*/
			virtual IPhysxMesh* createConvexMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale = core::vector3df(1,1,1)) = 0;
			/**
			\brief Creates a triangle mesh which can then be used to create a triangle mesh object

			This function should be executed during a loading period in your application as it can take a fair amount of time
			\param meshBuffer - the mesh buffer to create the triangle mesh from
			\param scale - the scale to create the triangle mesh with
			*/
			virtual IPhysxMesh* createTriangleMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale = core::vector3df(1,1,1)) = 0;
			/**
			\brief Remove the specified Physx object

			If the simulation is in process the object will not be removed until the next call to simulate()
			\param object - the Physx object to be removed
			*/
			virtual void removePhysxObject(IPhysxObject* object) = 0;
			/**
			\brief Remove the specified Physx mesh, returning true if successful. If the mesh's reference count is not 0 it will not be removed
			\param mesh - the Physx mesh to be removed
			*/
			virtual bool removePhysxMesh(IPhysxMesh* mesh) = 0;
			/**
			\brief Creates an explosion in the Physx scene

			The explosion radius will increase in size at the growth rate (growth of radius per second) until it reaches the max radius and will continue acting on objects until the specified time has passed
			The explosion force is inversely proportional to the explosion radius
			If the simulation is in process the explosion will not be added until the next call to simulate()
			\param position - the position of the explosion
			\param radius - the max radius of the explosion
			\param force - the initial force of the explosion
			\param growthRate - the rate at which the explosion will grow
			\param seconds - the number of seconds the explosion will be active for
			*/
			virtual void createExplosion(const core::vector3df& position, f32 radius, f32 force, f32 growthRate, f32 seconds) = 0;
			/**
			\brief Perform a raycast on all the rigid body objects, returning an array of SRaycastHitData giving information on the hit(s)

			Cloth and soft body objects are not included in this raycast and should be raycasted individually with their corresponding functions
			\param ray - the ray to be cast
			\ return a core::array of SRaycastHitData
			*/
			virtual const core::array<SRaycastHitData>& raycastAllRigidObjects(const core::line3df& ray) = 0;
			/**
			\brief Perform a raycast on all the rigid body objects, returning the first object hit

			The pointer parameters allow you to show what data you wish to be returned, specifying NULL will not generate the data and so should be quicker
			If you're wondering what barycentric coordinates are try google ;)
			Cloth and soft body objects are not included in this raycast and should be raycasted individually with their corresponding functions
			\param ray - the ray to be cast
			\param intersectionPoint - the intersection point
			\param surfaceNormal - the suface normal at the intersection point
			\param smoothedNormal - whether the normal returned should be smoothed or just the face normal
			\param distance - the distance from the ray start to the intersection point
			\param faceIndex - the index of the triangle hit from the original mesh buffer used to create the object
			\param barycentricCoords - the barycentric coordinates of the intersection point
			\return a IPhysxObject*
			*/
			virtual IPhysxObject* raycastClosestObject(const core::line3df& ray, core::vector3df* intersectionPoint = NULL, core::vector3df* surfaceNormal = NULL, bool smoothedNormal = false, f32* distance = NULL, u32* faceIndex = NULL, core::vector2df* barycentricCoords = NULL) = 0;
			/**
			\brief Sets the callback function to receive contact events between Physx objects

			Note that the contact callback will be called whilst the simulation is in progress so you must not add new objects within the callback but wait until the simulation has ended
			You may create explosions and remove objects from the callback, though, as they are buffered if a simulation is in progress and get added before the next simulation begins
			\param cb - the callback function
			*/
			virtual void setContactCallback(void (*cb)(IPhysxObject*, IPhysxObject*)) = 0;
			/**
			\brief Sets the gravity of the scene to the specified vector
			\param vec - the gravity vector
			*/
			virtual void setGravity(const core::vector3df& vec) = 0;
			/**
			\brief Returns the gravity of the scene in the specified vector
			\param vec - the gravity vector
			*/
			virtual void getGravity(core::vector3df& vec) = 0;
			/**
			\brief Returns the number of Physx objects in the scene
			\return a u32
			*/
			virtual u32 getNumPhysxObjects() = 0;
			/**
			\brief Returns the default width of the skin of Physx objects
			\return a f32
			*/
			virtual f32 getSkinWidth() = 0;
			
		protected:
			IrrlichtDevice* Device;
			bool DebugDataVisible;

	};

} // end IrrPhysx namespace

#endif /* INC_IPHYSXMANAGER_H */
