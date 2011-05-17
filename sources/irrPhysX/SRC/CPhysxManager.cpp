
#include "IrrPhysx.h"
#include "CPhysxManager.h"
#include "CPhysxObject.h"
#include "CClothPhysxObject.h"
#include "CConvexPhysxMesh.h"
#include "CClothPhysxMesh.h"
#include "CTrianglePhysxMesh.h"
#include "CBufferSceneNode.h"
#include "Cooking.h"
#include "Streams.h"
#include "Utilities.h"
#include "MathConversion.h"
#include "CCloth.h"
#include "CRaycastReport.h"
#include <stdio.h>

#define CLOTH_TEAR_MEMORY_FACTOR 3

namespace IrrPhysx {

CPhysxManager::CPhysxManager(IrrlichtDevice* device, const SSceneDesc& sceneDesc, bool disableHardware) : IPhysxManager(device) {

	Scene = NULL;
	MaxBounds = NULL;
	Simulating = false;

	printf("IrrPhysx wrapper version %.2f for Physx by Nvidia %i.%i.%i\n", IRRPHYSX_VERSION, NX_SDK_VERSION_MAJOR, NX_SDK_VERSION_MINOR, NX_SDK_VERSION_BUGFIX);

	// Create the Physx SDK
	NxPhysicsSDKDesc sdkDesc;
	if (disableHardware) sdkDesc.flags |= NX_SDKF_NO_HARDWARE;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, &ErrorStream, sdkDesc, &errorCode);
	if(!PhysicsSDK) {
		c8 errorStr[50];
		getNxSDKCreateError(errorCode, errorStr, 50);
		printf("SDK create error (%d - %s).\nUnable to initialize the PhysX SDK.\n", errorCode, errorStr);
		return;
	}

	// Check for hardware acceleration (Physx PPU card/Compatible GeForce GFX card)
	HardwareSimulation = false;
#if !defined(NX_DISABLE_HARDWARE)
	NxHWVersion hwCheck = PhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE)  {
		printf("PhysX accelerator hardware not found, simulation will be done in software\n");
		HardwareSimulation = false;
	} else {
		printf("PhysX accelerator hardware found, simulation will be done in hardware\n");
		HardwareSimulation = true;
	}
#endif

	// Initialise the cooking SDK for creating meshes
	if (!initCooking(NULL, &ErrorStream))
		printf("Error: Unable to initialize the cooking library\n");
		
	// Set some parameters
	PhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01f);

	// Set the debug visualisation
	setDebugDataVisible(DebugDataVisible);

	// Create a contact report to know about object collisions
	ContactReport = new CContactReport();
	
	// Create a raycast report to know about object raycast hits
	RaycastReport = new CRaycastReport(this);

	// Create the debug renderer
	DebugRenderer = new CDebugRenderer(device->getVideoDriver());

	// Setup the scene
	resetScene(sceneDesc);

}

CPhysxManager::~CPhysxManager() {

	closeCooking();

	if(PhysicsSDK) {
		if(Scene) PhysicsSDK->releaseScene(*Scene);
		Scene = NULL;
		NxReleasePhysicsSDK(PhysicsSDK);
		PhysicsSDK = NULL;
	}

	if (RaycastReport) {
		delete RaycastReport;
		RaycastReport = NULL;
	}

	if (ContactReport) {
		delete ContactReport;
		ContactReport = NULL;
	}

	if (DebugRenderer) {
		delete DebugRenderer;
		DebugRenderer = NULL;
	}

}

void CPhysxManager::resetScene(const SSceneDesc& desc) {

	// Clean up the old scene
	for (u32 i = 0 ; i < RemovalQueue.size(); ++i) 
		delete RemovalQueue[i];
	RemovalQueue.clear();

	for (u32 i = 0 ; i < Explosions.size(); ++i) 
		delete Explosions[i];
	Explosions.clear();
	ExplosionsAddQueue.clear();

	RaycastHits.clear();

	if (MaxBounds) {
		delete MaxBounds;
		MaxBounds = NULL;
	}
	
	// Release the old scene
	if (Scene) PhysicsSDK->releaseScene(*Scene);
	
	// Create a new scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(desc.Gravity.X, desc.Gravity.Y, desc.Gravity.Z);
	if (desc.MaxBounds != core::aabbox3df()) {
		MaxBounds = new NxBounds3;
		MaxBounds->include(NxVec3(desc.MaxBounds.MaxEdge.X, desc.MaxBounds.MaxEdge.Y, desc.MaxBounds.MaxEdge.Z));
		MaxBounds->include(NxVec3(desc.MaxBounds.MinEdge.X, desc.MaxBounds.MinEdge.Y, desc.MaxBounds.MinEdge.Z));
	}
	sceneDesc.maxBounds = MaxBounds;
	sceneDesc.boundsPlanes = desc.BoundPlanes;
	sceneDesc.upAxis = desc.UpAxis;
	if (HardwareSimulation)
		sceneDesc.simType = NX_SIMULATION_HW;
	sceneDesc.userContactReport = ContactReport;
	Scene = PhysicsSDK->createScene(sceneDesc);
	if (!Scene) {
		printf("Error: Unable to create a PhysX scene.\n");
		return;
	}

	// Setup the object collisions we're interested in
	Scene->setActorGroupPairFlags(0, 0, NX_NOTIFY_ON_START_TOUCH);

	// Set the default material
	NxMaterial* defaultMaterial = Scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

}

void CPhysxManager::simulate(f32 elapsedTime) {

	// Can't start simulation if already in progress
	if (Simulating) {
		printf("IrrPhysx: Can't simulate, simulation alread running\n");
		return;
	}

	// Delete those objects that have been queued up
	for (u32 i = 0 ; i < RemovalQueue.size() ; ++i) 
		delete RemovalQueue[i];
	RemovalQueue.clear();

	// Add any new explosions
	for (u32 i = 0 ; i < ExplosionsAddQueue.size() ; ++i)
		Explosions.push_back(ExplosionsAddQueue[i]);
	ExplosionsAddQueue.clear();

	// Update the explosions
	updateExplosions(elapsedTime);

	// Start simulation (non blocking)
	Simulating = true;
	Scene->simulate(elapsedTime);

}

void CPhysxManager::fetchResults() {

	// Nothing to fetch if simulation isn't in progress
	if (!Simulating) {
		printf("IrrPhysx: Can't fetch results, no simulation running\n");
		return;
	}

	// Fetch simulation results
	Scene->flushStream();
	Scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	Simulating = false;

}

void CPhysxManager::renderDebugData(const video::SColor& colour) {

	if (DebugDataVisible)
		DebugRenderer->renderData(*(Scene->getDebugRenderable()), colour);

}

void CPhysxManager::setDebugDataVisible(bool val) {

	IPhysxManager::setDebugDataVisible(val);

	PhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, DebugDataVisible);
	PhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, DebugDataVisible);
	PhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_MESH, DebugDataVisible);
	PhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_VALIDBOUNDS, DebugDataVisible);

}

IPhysxObject* CPhysxManager::createBoxObject(const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale, f32 density, const core::vector3df* initialVelocity) {

	if (!Scene) return NULL;	
	
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;
	NxActorDesc actorDesc;
	
	bodyDesc.angularDamping	= 0.5f;
	if (initialVelocity) bodyDesc.linearVelocity = NxVec3(initialVelocity->X, initialVelocity->Y, initialVelocity->Z);
	if (density) {
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	} else {
		actorDesc.body = NULL;
	}
	boxDesc.dimensions = NxVec3(scale.X, scale.Y, scale.Z);
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.globalPose.t = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);

    NxActor* actor = Scene->createActor(actorDesc);
	if (actor) {
		//actor->userData = (void*)size_t(size);
	} else printf("Failed box actor creation\n");

	return createPhysxObject(actor, EOT_BOX);

}

IPhysxObject* CPhysxManager::createMultiShapeObject(const core::array<core::aabbox3df>& boxes, const core::array<SSphereDesc>& spheres, const core::array<SCapsuleDesc>& capsules, const core::vector3df& position, const core::vector3df& rotation, f32 density, const core::vector3df* initialVelocity) {

	if (!Scene) return NULL;

	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	if(initialVelocity) bodyDesc.linearVelocity = NxVec3(initialVelocity->X, initialVelocity->Y, initialVelocity->Z);
	
	// Create the box shapes
	NxBoxShapeDesc* boxDesc = NULL;
	if (boxes.size() > 0) {
		boxDesc = new NxBoxShapeDesc[boxes.size()];
		core::vector3df vec;
		for (u32 i = 0 ; i < boxes.size() ; ++i) {
			vec = boxes[i].getExtent();
			boxDesc[i].dimensions = NxVec3(vec.X, vec.Y, vec.Z);
			vec = boxes[i].getCenter() * 2.0f;
			boxDesc[i].localPose.t = NxVec3(vec.X, vec.Y, vec.Z);
			actorDesc.shapes.pushBack(&boxDesc[i]);
		}
	}

	// Create the sphere shapes
	NxSphereShapeDesc* sphereDesc = NULL;
	if (spheres.size() > 0) {
		sphereDesc = new NxSphereShapeDesc[spheres.size()];
		for (u32 i = 0 ; i < spheres.size() ; ++i) {
			sphereDesc[i].radius = spheres[i].Radius;
			sphereDesc[i].localPose.t = NxVec3(spheres[i].Position.X, spheres[i].Position.Y, spheres[i].Position.Z);
			actorDesc.shapes.pushBack(&sphereDesc[i]);
		}
	}

	// Create the capsule shapes
	NxCapsuleShapeDesc* capsuleDesc = NULL;
	if (capsules.size() > 0) {
		capsuleDesc = new NxCapsuleShapeDesc[capsules.size()];
		for (u32 i = 0 ; i < capsules.size() ; ++i) {
			capsuleDesc[i].radius = capsules[i].Radius;
			capsuleDesc[i].height = capsules[i].Height;
			capsuleDesc[i].localPose.t = NxVec3(capsules[i].Position.X, capsules[i].Position.Y, capsules[i].Position.Z);
			actorDesc.shapes.pushBack(&capsuleDesc[i]);
		}
	}

	if (density) {
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	} else {
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);

	// Create the actor
	NxActor* actor = Scene->createActor(actorDesc);
	if (actor) {
		//actor->userData = (size_t)size;
	} else printf("Failed multishape actor creation\n");

	delete [] boxDesc;
	delete [] sphereDesc;
	delete [] capsuleDesc;

	return createPhysxObject(actor, EOT_MULTISHAPE);

}

IPhysxObject* CPhysxManager::createSphereObject(const core::vector3df& position, const core::vector3df& rotation, f32 radius, f32 density, const core::vector3df* initialVelocity) {

	if (!Scene) return NULL;	

	NxBodyDesc bodyDesc;
	NxSphereShapeDesc sphereDesc;
	NxActorDesc actorDesc;

	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = NxVec3(initialVelocity->X, initialVelocity->Y, initialVelocity->Z);
	if (density) {
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	} else {
		actorDesc.body = NULL;
	}
	sphereDesc.radius = radius;
	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.globalPose.t = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);

    NxActor* actor = Scene->createActor(actorDesc);
	if (actor) {
		//actor->userData = (void*)size_t(size);
	} else printf("Failed sphere actor creation\n");

	return createPhysxObject(actor, EOT_SPHERE);

}

IPhysxObject* CPhysxManager::createCapsuleObject(const core::vector3df& position, const core::vector3df& rotation, f32 radius, f32 height, f32 density, const core::vector3df* initialVelocity) {

	if (!Scene) return NULL;	

	NxBodyDesc bodyDesc;
	NxCapsuleShapeDesc capsuleDesc;
	NxActorDesc actorDesc;

	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = NxVec3(initialVelocity->X, initialVelocity->Y, initialVelocity->Z);
	if (density) {
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	} else {
		actorDesc.body = NULL;
	}
	capsuleDesc.radius = radius;
	capsuleDesc.height = height;
	actorDesc.shapes.pushBack(&capsuleDesc);
	actorDesc.globalPose.t = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);

    NxActor* actor = Scene->createActor(actorDesc);
	if (actor) {
		//actor->userData = (void*)size_t(size);
	} else printf("Failed capsule actor creation\n");

	return createPhysxObject(actor, EOT_CAPSULE);

}

IPhysxObject* CPhysxManager::createPlaneObject(f32 d, const core::vector3df& normal) {

	if (!Scene) return NULL;

	NxPlaneShapeDesc planeDesc;
	planeDesc.d = d;
	planeDesc.normal = NxVec3(normal.X, normal.Y, normal.Z);
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	NxActor* actor = Scene->createActor(actorDesc);

	return createPhysxObject(actor, EOT_PLANE);

}

IPhysxObject* CPhysxManager::createHeightfieldObject(s16* data, u32 width, u32 height, f32 yMultiplier, const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale, u32 tessellation) {

	if (!Scene || !data) return NULL;

	NxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns = width;
	heightFieldDesc.nbRows = height;
	heightFieldDesc.convexEdgeThreshold = 0;

	// allocate storage for samples
	heightFieldDesc.samples = new NxU32[width*height];
	heightFieldDesc.sampleStride = sizeof(NxU32);

	// Chuck the data into our heightmap sample
	char* currentByte = (char*)heightFieldDesc.samples;
	for (NxU32 row = 0 ; row < width ; ++row) {
		for (NxU32 column = 0 ; column < height ; ++column) {
					
			NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;

			currentSample->height = (NxI16)data[width*row + column];
			currentSample->tessFlag = tessellation;

			currentByte += heightFieldDesc.sampleStride;
		}
	}

	NxHeightField* heightField = PhysicsSDK->createHeightField(heightFieldDesc);

	// data has been copied, we can free our buffer
	delete [] heightFieldDesc.samples;

	NxHeightFieldShapeDesc heightFieldShapeDesc;
	heightFieldShapeDesc.heightField	= heightField;
	heightFieldShapeDesc.shapeFlags		= NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;
	heightFieldShapeDesc.heightScale	= scale.Y / yMultiplier;
	heightFieldShapeDesc.rowScale		= scale.X;
	heightFieldShapeDesc.columnScale	= scale.Z;
	heightFieldShapeDesc.materialIndexHighBits = 0;
	heightFieldShapeDesc.holeMaterial = 2;
	NxMat33 rot;
	rot.id();
	heightFieldShapeDesc.localPose = NxMat34(rot, NxVec3(-((width-1)*scale.X)/2.0f,0,-((height-1)*scale.Z)/2.0f));

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&heightFieldShapeDesc);
	actorDesc.body = NULL;
	actorDesc.globalPose.t = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation + core::vector3df(0,90,0), actorDesc.globalPose.M);
	NxActor* actor = Scene->createActor(actorDesc);

	return createPhysxObject(actor, EOT_HEIGHTFIELD);

}

IPhysxObject* CPhysxManager::createHeightfieldObject(scene::ITerrainSceneNode* terrain, u32 smoothFactor) {

	#define Y_MULTIPLIER 100.0f

	if (!Scene || !terrain) return NULL;

	// Get the heightmap path 
	io::IAttributes* attribs = Device->getFileSystem()->createEmptyAttributes(); 
	if (!attribs) return NULL;
	terrain->serializeAttributes(attribs); 
	core::stringc heightmapName = attribs->getAttributeAsString("Heightmap"); 
	attribs->drop();
					
	// Get the terrain's parameters
	video::IImage* heightmap = Device->getVideoDriver()->createImageFromFile(heightmapName.c_str());
	u32 dimension = heightmap->getDimension().Width;
	core::vector3df position = terrain->getPosition();
	core::vector3df centre = terrain->getTerrainCenter() - position;
	core::vector3df rotation = -terrain->getRotation();
	core::vector3df scale = terrain->getScale();

	NxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns = dimension;
	heightFieldDesc.nbRows = dimension;
	heightFieldDesc.convexEdgeThreshold = 0;

	// allocate storage for samples
	heightFieldDesc.samples = new NxU32[dimension*dimension];
	heightFieldDesc.sampleStride = sizeof(NxU32);

	// Grab the raw heightmap data
	char* currentByte = (char*)heightFieldDesc.samples;
	for (NxU32 row = 0 ; row < dimension ; ++row) {
		for (NxU32 column = 0 ; column < dimension ; ++column) {			
			NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;

			currentSample->height = (NxI16)((heightmap->getPixel(row,column).getLuminance() - centre.Y) * Y_MULTIPLIER);
			currentSample->tessFlag = 1;

			currentByte += heightFieldDesc.sampleStride;
		}
	}	
	heightmap->drop(); // done with this

	// Smooth it out to match Irrlicht's ITerrainSceneNode's smoothing
	u32 run;
	u32 index;
	for (run = 0 ; run < smoothFactor ; ++run) {
		currentByte = (char*)heightFieldDesc.samples;
		currentByte += heightFieldDesc.sampleStride * 2;
		for (index = 2 ; index < (dimension * dimension - 2) ; ++index) {
			NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
			currentSample->height = (NxI16) ROUND( ( ((NxHeightFieldSample*)(currentByte - heightFieldDesc.sampleStride * 2))->height +
											         ((NxHeightFieldSample*)(currentByte - heightFieldDesc.sampleStride    ))->height +
											         ((NxHeightFieldSample*)(currentByte + heightFieldDesc.sampleStride    ))->height +
									                 ((NxHeightFieldSample*)(currentByte + heightFieldDesc.sampleStride * 2))->height   ) / 4.0f );
			currentByte += heightFieldDesc.sampleStride;
		}
	}

	for (run = 0 ; run < smoothFactor ; ++run) {
		currentByte = (char*)heightFieldDesc.samples;
		currentByte += heightFieldDesc.sampleStride * dimension;
		for (index = dimension ; index < (dimension * (dimension - 1)) ; ++index) {
			NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
			currentSample->height = (NxI16) ROUND( ( ((NxHeightFieldSample*)(currentByte - heightFieldDesc.sampleStride * dimension))->height +
											         ((NxHeightFieldSample*)(currentByte + heightFieldDesc.sampleStride * dimension))->height   ) / 2.0f );
			currentByte += heightFieldDesc.sampleStride;
		}
	}

	NxHeightField* heightField = PhysicsSDK->createHeightField(heightFieldDesc);

	// data has been copied, we can free our buffer
	delete [] heightFieldDesc.samples;

	NxHeightFieldShapeDesc heightFieldShapeDesc;
	heightFieldShapeDesc.heightField	= heightField;
	heightFieldShapeDesc.shapeFlags		= NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;
	heightFieldShapeDesc.heightScale	= scale.Y / Y_MULTIPLIER;
	heightFieldShapeDesc.rowScale		= -scale.X;
	heightFieldShapeDesc.columnScale	= scale.Z;
	heightFieldShapeDesc.materialIndexHighBits = 0;
	heightFieldShapeDesc.holeMaterial = 2;
	NxMat33 rot;
	rot.id();
	heightFieldShapeDesc.localPose = NxMat34(rot, NxVec3(centre.X, centre.Y, centre.Z) - NxVec3((dimension+1)*-scale.X/2.0f,0,(dimension-1)*scale.Z/2.0f));

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&heightFieldShapeDesc);
	actorDesc.body = NULL;
	actorDesc.globalPose.t = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);
	NxActor* actor = Scene->createActor(actorDesc);

	return createPhysxObject(actor, EOT_HEIGHTFIELD);

}

IPhysxObject* CPhysxManager::createConvexMeshObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation, f32 density) {

	if (!Scene || !mesh || mesh->getType() != EMT_CONVEX) return NULL;
	
	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.meshData = ((CConvexPhysxMesh*)mesh)->getConvexMesh();

	// Create actor
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	NxActorDesc actorDesc;
	if (density < 0.0f) density = 1.0f;
	actorDesc.shapes.pushBack(&convexShapeDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= density;
	actorDesc.globalPose.t  = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);

	NxActor* actor = Scene->createActor(actorDesc);
		
	return createPhysxObject(actor, EOT_CONVEX_MESH);
	
}

IPhysxObject* CPhysxManager::createTriangleMeshObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation) {

	if (!Scene || !mesh || mesh->getType() != EMT_TRIANGLE) return NULL;

	//create triangle mesh instance
	NxTriangleMeshShapeDesc meshShapeDesc;
	meshShapeDesc.meshData = ((CTrianglePhysxMesh*)mesh)->getTriangleMesh();

	NxBodyDesc bodyDesc;
	NxActorDesc actorDesc;

	actorDesc.shapes.pushBack(&meshShapeDesc);
	actorDesc.body          = NULL;
	actorDesc.globalPose.t  = NxVec3(position.X, position.Y, position.Z);
	rotationVectorToPhysxMatrix(rotation, actorDesc.globalPose.M);

	NxActor* actor = Scene->createActor(actorDesc);

	return createPhysxObject(actor, EOT_TRIANGLE_MESH);

}

IClothPhysxObject* CPhysxManager::createClothObject(IPhysxMesh* mesh, const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale, f32 density, f32 pressure, bool tearable) {

	if (!Scene || !mesh  || mesh->getType() != EMT_CLOTH) return NULL;

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(position.X,position.Y,position.Z);
	rotationVectorToPhysxMatrix(rotation, clothDesc.globalPose.M);
	clothDesc.thickness = 0.2f;
	clothDesc.density = density;
	clothDesc.bendingStiffness = 0.5f;
	clothDesc.stretchingStiffness = 1.0f;
	clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;
	clothDesc.flags |= NX_CLF_SELFCOLLISION;
	
	if (tearable && ((CClothPhysxMesh*)mesh)->isTearable()) 
		clothDesc.flags |= NX_CLF_TEARABLE;

	if (pressure >= 0.0f) {
		clothDesc.pressure = pressure;
		clothDesc.flags |= NX_CLF_PRESSURE;
	}

	if (HardwareSimulation) clothDesc.flags |= NX_CLF_HARDWARE;

	CCloth* cloth = new CCloth(Scene, clothDesc, (CClothPhysxMesh*)mesh, scale);
	if (!cloth->getNxCloth()) {
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete cloth;
		return NULL;
	} else {
		
	}

	return createClothPhysxObject(cloth);

}

scene::ISceneNode* CPhysxManager::createClothSceneNode(scene::ISceneManager* smgr, IPhysxObject* clothObject) {

	if (!smgr || !clothObject || clothObject->getType() != EOT_CLOTH) return NULL;

	CCloth* cloth = ((CClothPhysxObject*)clothObject)->getCloth();
	NxMeshData* meshData = cloth->getMeshData();
	void* ptr = meshData->verticesPosBegin;
	CBufferSceneNode* node = new CBufferSceneNode((video::S3DVertex*)ptr, meshData->numVerticesPtr, meshData->maxVertices, (u16*)meshData->indicesBegin, meshData->numIndicesPtr, smgr->getRootSceneNode(), smgr, -1);
	if (node) node->drop();
	return node;

}

IPhysxMesh* CPhysxManager::createClothMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale, bool tearable) {

	NxClothMeshDesc desc;

	desc.numVertices				= meshBuffer->getVertexCount();
	desc.numTriangles				= meshBuffer->getIndexCount() / 3;
	desc.pointStrideBytes			= sizeof(NxVec3);
	desc.triangleStrideBytes		= 3*sizeof(NxU32);
	desc.vertexMassStrideBytes		= sizeof(NxReal);
	desc.vertexFlagStrideBytes		= sizeof(NxU32);
	desc.points						= new NxVec3[desc.numVertices];
	desc.triangles					= new NxU32[desc.numTriangles*3];
	desc.vertexMasses				= 0;
	desc.vertexFlags				= 0;
	desc.flags						= NX_CLOTH_MESH_WELD_VERTICES;
	desc.weldingDistance			= 0.0001f;

	if (tearable)
		desc.flags |= NX_CLOTH_MESH_TEARABLE;

	u32 maxVertices = desc.numVertices;
	if (tearable)
		maxVertices *= CLOTH_TEAR_MEMORY_FACTOR;
	u32 maxIndices  = 3 * desc.numTriangles;

	// copy positions, texcoords and indices
	NxVec3* vDest = (NxVec3*)desc.points;
	u32 numTexCoords = desc.numVertices;
	core::vector2df* texCoords = new core::vector2df[numTexCoords];
	if (!texCoords) {
		delete [] desc.points;
		delete [] desc.triangles;
		return NULL;
	}
	switch (meshBuffer->getVertexType()) {
		case video::EVT_STANDARD: {
			video::S3DVertex* vertices = (video::S3DVertex*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) {
				vDest[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
				texCoords[i] = vertices[i].TCoords;
			}
			break;
		}
		case video::EVT_2TCOORDS: {
			video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) {
				vDest[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
				texCoords[i] = vertices[i].TCoords;
			}
			break;
		}
		case video::EVT_TANGENTS: {
			video::S3DVertexTangents* vertices = (video::S3DVertexTangents*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) {
				vDest[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
				texCoords[i] = vertices[i].TCoords;
			}
			break;
		}
	}
	for (u32 i = 0 ; i < meshBuffer->getIndexCount() ; ++i) 
		((NxU32*)desc.triangles)[i] = meshBuffer->getIndices()[i];
		
	u16* indexRenderBuffer = new u16[maxIndices];
	if (!indexRenderBuffer) {
		delete [] desc.points;
		delete [] desc.triangles;
		delete [] texCoords;
		return NULL;
	}
	memset(indexRenderBuffer, 0, sizeof(u16) * maxIndices);
	for (NxU32 i = 0; i < desc.numTriangles; ++i) {
		assert((desc.flags & NX_CF_16_BIT_INDICES) == 0);
		NxU32* tri = (NxU32*)(((char*)desc.triangles) + (desc.triangleStrideBytes * i));
		indexRenderBuffer[3*i+0] = tri[0];
		indexRenderBuffer[3*i+1] = tri[1];
		indexRenderBuffer[3*i+2] = tri[2];
	}

	// Cook the mesh
	CMemoryWriteBuffer wb;	
	if (!cookClothMesh(desc, wb)) {
		printf("Failed cloth mesh cooking\n");
		delete [] desc.points;
		delete [] desc.triangles;
		delete [] texCoords;
		delete [] indexRenderBuffer;
		return NULL;
	}
	CMemoryReadBuffer rb(wb.data);
	// Create the mesh
	NxClothMesh* clothMesh = PhysicsSDK->createClothMesh(rb);
	if (!clothMesh) {
		printf("Failed cloth mesh creation\n");
		delete [] desc.points;
		delete [] desc.triangles;
		delete [] texCoords;
		delete [] indexRenderBuffer;
		return NULL;
	}

	delete [] desc.points;
	delete [] desc.triangles;
	delete [] desc.vertexMasses;
	delete [] desc.vertexFlags;

	CClothPhysxMesh* clothPhysxMesh = new CClothPhysxMesh(Scene, clothMesh, tearable, scale);
	clothPhysxMesh->MaxVertices = maxVertices;
	clothPhysxMesh->MaxIndices = maxIndices;
	clothPhysxMesh->NumTexCoords = numTexCoords;
	clothPhysxMesh->TexCoords = texCoords;
	clothPhysxMesh->IndexRenderBuffer = indexRenderBuffer;
	clothPhysxMesh->InitNumVertices = desc.numVertices;
	clothPhysxMesh->InitNumTriangles = desc.numTriangles;

	return clothPhysxMesh;

}

IPhysxMesh* CPhysxManager::createConvexMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale) {

	if (!Scene || !meshBuffer) return NULL;

	NxConvexMesh* convexMesh = NULL;

	NxVec3* verts = new NxVec3[meshBuffer->getVertexCount()];
	switch (meshBuffer->getVertexType()) {
		case video::EVT_STANDARD: {
			video::S3DVertex* vertices = (video::S3DVertex*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) 
				verts[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_2TCOORDS: {
			video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) 
				verts[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_TANGENTS: {
			video::S3DVertexTangents* vertices = (video::S3DVertexTangents*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) 
				verts[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
	}
	
	// Create descriptor for convex mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= meshBuffer->getVertexCount();
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// Cooking from memory
	CMemoryWriteBuffer buf;
	if (cookConvexMesh(convexDesc, buf)) {

		convexMesh = PhysicsSDK->createConvexMesh(CMemoryReadBuffer(buf.data));

	} else printf("Failed convex mesh cooking\n");

	delete [] verts;

	if (!convexMesh) {
		printf("Failed to create convex mesh\n");
		return NULL;
	}

	return new CConvexPhysxMesh(Scene, convexMesh, scale);

}

IPhysxMesh* CPhysxManager::createTriangleMesh(scene::IMeshBuffer* meshBuffer, const core::vector3df& scale) {

	if (!Scene || !meshBuffer) return NULL;

	NxTriangleMesh* triMesh = NULL;

	NxVec3* verts = new NxVec3[meshBuffer->getVertexCount()];
	switch (meshBuffer->getVertexType()) {
		case video::EVT_STANDARD: {
			video::S3DVertex* vertices = (video::S3DVertex*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) 
				verts[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_2TCOORDS: {
			video::S3DVertex2TCoords* vertices = (video::S3DVertex2TCoords*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) 
				verts[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
		case video::EVT_TANGENTS: {
			video::S3DVertexTangents* vertices = (video::S3DVertexTangents*)meshBuffer->getVertices();
			for (u32 i = 0 ; i < meshBuffer->getVertexCount() ; ++i) 
				verts[i] = NxVec3(vertices[i].Pos.X * scale.X, vertices[i].Pos.Y * scale.Y, vertices[i].Pos.Z * scale.Z);
			break;
		}
	}

	// Build the triangle mesh.
	NxTriangleMeshDesc meshDesc;
	meshDesc.numVertices                = meshBuffer->getVertexCount();
	meshDesc.numTriangles               = meshBuffer->getIndexCount() / 3;
	meshDesc.pointStrideBytes           = sizeof(NxVec3);
	meshDesc.triangleStrideBytes        = 3*sizeof(u16);
	meshDesc.points						= verts;
	meshDesc.triangles                  = meshBuffer->getIndices();                           
	meshDesc.flags                      = NX_MF_16_BIT_INDICES ;

	CMemoryWriteBuffer buf;
	if (cookTriangleMesh(meshDesc, buf)) {

		//create triangle mesh instance
		NxTriangleMeshShapeDesc meshShapeDesc;

		//Create the mesh from the cooked data.
		triMesh = PhysicsSDK->createTriangleMesh(CMemoryReadBuffer(buf.data));

	} else printf("Failed triangle mesh cooking\n");

	delete [] verts;

	if (!triMesh) {
		printf("Failed to create triangle mesh\n");
		return NULL;
	}

	return new CTrianglePhysxMesh(Scene, triMesh, scale);

}

void CPhysxManager::removePhysxObject(IPhysxObject* object) {

	if (Simulating) 
		RemovalQueue.push_back(object);
	else
		delete object;

}

bool CPhysxManager::removePhysxMesh(IPhysxMesh* mesh) {

	if (mesh->getReferenceCount() == 0) {
		delete mesh;
		return true;
	} else
		return false;

}

void CPhysxManager::createExplosion(const core::vector3df& position, f32 radius, f32 force, f32 growthRate, f32 seconds) {

	SExplosion* explosion = new SExplosion;

	explosion->Position = NxVec3(position.X, position.Y, position.Z);
	explosion->MaxRadius = radius;
	explosion->CurrentRadius = 1;
	explosion->InitialForce = force;
	explosion->CurrentForce = force;
	explosion->GrowthRate = growthRate;
	explosion->TimeToGo = seconds;

	if (Simulating) 
		ExplosionsAddQueue.push_back(explosion);
	else 
		Explosions.push_back(explosion);
	
}

const core::array<SRaycastHitData>& CPhysxManager::raycastAllRigidObjects(const core::line3df& ray) {

	RaycastHits.clear(); // wipe out any objects hit by last raycast

	NxRay worldRay;
	const core::vector3df dir = ray.getVector().normalize();
	worldRay.dir = NxVec3(dir.X, dir.Y, dir.Z);
	worldRay.orig = NxVec3(ray.start.X, ray.start.Y, ray.start.Z);

	Scene->raycastAllShapes(worldRay, *RaycastReport, NX_ALL_SHAPES); // Fills up RaycastHits with objects hit

	return RaycastHits;

}

IPhysxObject* CPhysxManager::raycastClosestObject(const core::line3df& ray, core::vector3df* intersectionPoint, core::vector3df* surfaceNormal, bool smoothedNormal, f32* distance, u32* faceIndex, core::vector2df* barycentricCoords) {

	NxRay worldRay;
	const core::vector3df dir = ray.getVector().normalize();
	worldRay.dir = NxVec3(dir.X, dir.Y, dir.Z);
	worldRay.orig = NxVec3(ray.start.X, ray.start.Y, ray.start.Z);

	u32 flags = 0;
	flags |= NX_RAYCAST_SHAPE;
	if (intersectionPoint) flags |= NX_RAYCAST_IMPACT;
	if (surfaceNormal) flags |= NX_RAYCAST_FACE_NORMAL;
	if (smoothedNormal) flags |= NX_RAYCAST_NORMAL;
	if (distance) flags |= NX_RAYCAST_DISTANCE;
	if (faceIndex) flags |= NX_RAYCAST_FACE_INDEX;
	if (barycentricCoords) flags |= NX_RAYCAST_UV;

	NxRaycastHit hit;
	NxShape* shape = Scene->raycastClosestShape(worldRay, NX_ALL_SHAPES, hit, 0xffffffff, NX_MAX_F32, flags, NULL, NULL);
	
	/*
	NxRaycastBit:
	NX_RAYCAST_SHAPE  "shape" member of NxRaycastHit is valid  
	NX_RAYCAST_IMPACT  "worldImpact" member of NxRaycastHit is valid  
	NX_RAYCAST_NORMAL  "worldNormal" member of NxRaycastHit is valid  
	NX_RAYCAST_FACE_INDEX  "faceID" member of NxRaycastHit is valid  
	NX_RAYCAST_DISTANCE  "distance" member of NxRaycastHit is valid  
	NX_RAYCAST_UV  "u" and "v" members of NxRaycastHit are valid  
	NX_RAYCAST_FACE_NORMAL  Same as NX_RAYCAST_NORMAL but computes a non-smoothed normal.  
	NX_RAYCAST_MATERIAL  "material" member of NxRaycastHit is valid  
	*/
	
	if (intersectionPoint) {
		intersectionPoint->X = hit.worldImpact.x;
		intersectionPoint->Y = hit.worldImpact.y;
		intersectionPoint->Z = hit.worldImpact.z;
	}

	if (surfaceNormal) {
		surfaceNormal->X = hit.worldNormal.x;
		surfaceNormal->Y = hit.worldNormal.y;
		surfaceNormal->Z = hit.worldNormal.z;
	}

	if (distance) *distance = hit.distance;

	if (faceIndex) *faceIndex = hit.faceID;

	if (barycentricCoords) {
		barycentricCoords->X = hit.u;
		barycentricCoords->Y = hit.v;
	}
	
	if (shape) {
		return (IPhysxObject*)shape->getActor().userData;
	} else { 
		return NULL;
	}

}

void CPhysxManager::setContactCallback(void (*cb)(IPhysxObject*, IPhysxObject*)) {

	ContactReport->setCallback(cb);

}

void CPhysxManager::setGravity(const core::vector3df& vec) {

	Scene->setGravity(NxVec3(vec.X, vec.Y, vec.Z));

}

void CPhysxManager::getGravity(core::vector3df& vec) {

	NxVec3 gravity;
	Scene->getGravity(gravity);
	vec.X = gravity.x;
	vec.Y = gravity.y;
	vec.Z = gravity.z;

}

u32 CPhysxManager::getNumPhysxObjects() {

	return Scene->getNbActors() + Scene->getNbCloths();

}

f32 CPhysxManager::getSkinWidth() {

	return PhysicsSDK->getParameter(NX_SKIN_WIDTH);

}

IPhysxObject* CPhysxManager::createPhysxObject(NxActor* actor, E_OBJECT_TYPE type) {

	if (!actor) return NULL;

	return new CPhysxObject(Scene, actor, type);

}

IClothPhysxObject* CPhysxManager::createClothPhysxObject(CCloth* cloth) {

	if (!cloth) return NULL;

	return new CClothPhysxObject(Scene, cloth);

}

void CPhysxManager::updateExplosions(f32 elapsedTime) {

	if (Explosions.size() == 0) return;

	NxActor** actors = Scene->getActors();
	u32 numActors = Scene->getNbActors();
	NxActor* actor = NULL;
	NxVec3 vec;	
	  
	for (u32 e = 0 ; e < Explosions.size() ; ++e) {
		
		// If explosion hasn't run out of time
		if (Explosions[e]->TimeToGo > 0.0f) {
			// Update it
			Explosions[e]->TimeToGo -= elapsedTime;
			Explosions[e]->CurrentRadius += Explosions[e]->GrowthRate * elapsedTime;
			if (Explosions[e]->CurrentRadius < Explosions[e]->MaxRadius) Explosions[e]->CurrentRadius = Explosions[e]->MaxRadius;
			//Explosions[e]->CurrentForce = ((Explosions[e]->MaxRadius - Explosions[e]->CurrentRadius) / Explosions[e]->MaxRadius) * Explosions[e]->InitialForce;
			// Iterate through actors in the scene
			for (u32 i = 0; i < numActors; ++i) {
				actor = actors[i];
				
				// If the actor is static ignore it
				if (actor->getMass() == 0) continue;

				vec = actor->getGlobalPosition() - Explosions[e]->Position;

				// If the actor is too far away, ignore it
				if (vec.magnitude() > Explosions[e]->CurrentRadius) continue;

				// Could call contact callback with explosion hitting object?
				
				// Add force of explosion to actor
				vec.setMagnitude(1);
				actor->addForce(Explosions[e]->CurrentForce * vec, NX_IMPULSE);
			}
		} else {
			delete Explosions[e];
			Explosions.erase(e);
			e--;
		}
    }

}

} // end IrrPhysx namespace
