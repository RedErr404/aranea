
#include <irrlicht.h>
#include <IrrPhysx.h>
#include "ShapeCreation.h"
#include "CGUIParamChanger.h"
#include <stdio.h>

#define WIND_RANGE 10.0f

using namespace irr;
using namespace IrrPhysx;

IrrlichtDevice* device = NULL;
scene::ISceneManager* smgr = NULL;
video::IVideoDriver* driver = NULL;
gui::IGUIEnvironment* guienv = NULL;
scene::ICameraSceneNode* camera = NULL;
IPhysxManager* physxManager = NULL;
core::array<SPhysxAndNodePair*> objects;
core::array<SPhysxAndNodePair*> flags;
core::array<IPhysxMesh*> tearableMeshes;
f32 objectDensity = 50;
IPhysxObject* staticBox = NULL;
IPhysxMesh* flatClothMesh10x5x1 = NULL;
IPhysxMesh* sydneyClothMesh = NULL;
CGUIParamChanger* paramChanger = NULL;

f32 rand(f32 lower, f32 upper) {
		 
	const f32 r = (f32)rand()/((f32)RAND_MAX+1);
	return r*(upper-lower) + lower;

}

void addClothFlag(const core::vector3df& pos) {

	// Create a flag pole to hold the flag
	SPhysxAndNodePair* flagpolePair = createMeshBoundingBox(physxManager, smgr, driver, smgr->getMesh("../media/flagpole.ms3d"), pos, core::vector3df(1,1,1), objectDensity);
	objects.push_back(flagpolePair);
	
	// Get our mesh to figure out some positions based on its dimensions
	scene::IAnimatedMesh* flagMesh = smgr->getMesh("../media/flatplane.ms3d");
	core::vector3df meshDim = flagMesh->getBoundingBox().getExtent();
	core::vector3df scale(10,5,1);
	// Create a piece of cloth from our previously created cloth physx mesh
	SPhysxAndNodePair* clothFlagPair = new SPhysxAndNodePair;
	clothFlagPair->PhysxObject = physxManager->createClothObject(flatClothMesh10x5x1, pos + core::vector3df((meshDim.X*scale.X)/2.0f,80-(meshDim.Y*scale.Y)/2.0f,0), core::vector3df(0,0,0), scale, objectDensity);
	// Attach the flag to the flagpole
	((IClothPhysxObject*)clothFlagPair->PhysxObject)->attachToObject(flagpolePair->PhysxObject);
	// We need a special type of scene node to represent the cloth visually
	clothFlagPair->SceneNode = physxManager->createClothSceneNode(smgr, clothFlagPair->PhysxObject);
	// The flag is only a single surface so its back is not rendered by default, so let's rectify that
	// This is also the reason why lighting is sometimes incorrect as the normals only face one direction but affect the lighting on both sides.
	// So with the mesh facing away from the light both sides are unlit, facing towards the light both sides are lit.
	clothFlagPair->SceneNode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
	// I'm just messing around with the texture matrix to try and get the flag to look nice, flipping the texture coords basically
	core::matrix4 mat;
	mat.setScale(core::vector3df(-1,1,1));
	clothFlagPair->SceneNode->getMaterial(0).setTextureMatrix(0, mat);
	// Give it a texture
	clothFlagPair->SceneNode->setMaterialTexture(0, driver->getTexture("../media/flag.jpg"));
	objects.push_back(clothFlagPair);
	flags.push_back(clothFlagPair);
		
}

void addClothSydney(const core::vector3df& pos) {

	// Create a piece of cloth from our previously created sydney cloth physx mesh, with internal pressure
	SPhysxAndNodePair* clothSydneyPair = new SPhysxAndNodePair;
	clothSydneyPair->PhysxObject = physxManager->createClothObject(sydneyClothMesh, pos, core::vector3df(0,0,0), core::vector3df(1,1,1), objectDensity, 1.0f);
	clothSydneyPair->SceneNode = physxManager->createClothSceneNode(smgr, clothSydneyPair->PhysxObject);
	clothSydneyPair->SceneNode->setMaterialTexture(0, driver->getTexture("../media/sydney.jpg"));
	objects.push_back(clothSydneyPair);
		
}

void addTearableCloth(IPhysxObject* attachObject) {
		
	core::vector3df pos;
	attachObject->getPosition(pos);
	// Get our mesh to figure out some positions based on its dimensions
	scene::IAnimatedMesh* clothMesh = smgr->getMesh("../media/flatplane.ms3d");
	core::vector3df meshDim = clothMesh->getBoundingBox().getExtent();
	core::vector3df scale(5,5,1);
	// Create a new cloth mesh for tearing
	IPhysxMesh* physxMesh = physxManager->createClothMesh(clothMesh->getMeshBuffer(0), scale, true);
	if (physxMesh) {
	tearableMeshes.push_back(physxMesh);
	// Create a cloth object
	SPhysxAndNodePair* clothPair = new SPhysxAndNodePair;
	if (clothPair) {
		clothPair->PhysxObject = physxManager->createClothObject(physxMesh, pos + core::vector3df(0,-(meshDim.Y*scale.Y)/2.0f,0), core::vector3df(0,0,0), scale, objectDensity, 0.0f, true);
		if (clothPair->PhysxObject) {
			// Attach the cloth to specified object
			((IClothPhysxObject*)clothPair->PhysxObject)->attachToObject(attachObject);
			clothPair->SceneNode = physxManager->createClothSceneNode(smgr, clothPair->PhysxObject);
			if (clothPair->SceneNode) {
				clothPair->SceneNode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
				// Give it a texture
				clothPair->SceneNode->setMaterialTexture(0, driver->getTexture("../media/flag.jpg"));
				core::matrix4 mat;
				// Flip the texture
				mat.setTextureScale(-1.0f,1.0f);
				clothPair->SceneNode->getMaterial(0).setTextureMatrix(0, mat);
				// Add it to the objects array
				objects.push_back(clothPair);	
	
				// Create a box which will tear the cloth
				f32 randNum = (meshDim.X*scale.X)/2.0f;//rand(0.0f, meshDim.X*scale.X);
				SPhysxAndNodePair* boxPair = createBox(physxManager, smgr, driver, pos + core::vector3df(randNum,2-randNum,0), core::vector3df(4,4,4), 20);
				if (boxPair) {
					objects.push_back(boxPair);
					// Attach it to the cloth
					((IClothPhysxObject*)clothPair->PhysxObject)->attachToObject(boxPair->PhysxObject);
				}
			}
		}
	}
	}

}

// Our event reciever to catch key presses
class CEventReceiver : public IEventReceiver {

	public:
		bool OnEvent(const SEvent& event) {

			if (event.EventType == EET_KEY_INPUT_EVENT) {
				
				if (event.KeyInput.PressedDown) return false;

				switch (event.KeyInput.Key) {
					case KEY_KEY_W:
						// Create a cube at the specified position
						objects.push_back(createBox(physxManager, smgr, driver, core::vector3df(0.0f, 100.0f, 0.0f), core::vector3df(2,2,2), objectDensity));
						return true;
					case KEY_KEY_F:
						// Create a cloth flag on a flagpole
						addClothFlag(core::vector3df(75,100,0));
						return true;
					case KEY_KEY_T:
						// Create a tearable piece of cloth
						addTearableCloth(staticBox);
						return true;
					case KEY_KEY_S:
						// Create a cloth sydney
						addClothSydney(core::vector3df(0,100,0));
						return true;
					case KEY_SPACE:
						// Throw a sphere from the camera
						objects.push_back(createSphereFromCamera(physxManager, smgr, driver, camera, 8, objectDensity)); 
						return true;
					case KEY_KEY_C:
						// Delete all objects
						for (u32 i = 0 ; i < objects.size() ; i++) {
							SPhysxAndNodePair* pair = objects[i];
							physxManager->removePhysxObject(pair->PhysxObject);
							pair->SceneNode->remove();
							delete pair;
						}
						objects.clear();
						flags.clear();
						// Delete tearable meshes
						for (u32 i = 0 ; i < tearableMeshes.size() ; i++) 
							physxManager->removePhysxMesh(tearableMeshes[i]);
						tearableMeshes.clear();
						return true;
					case KEY_KEY_V:
						// Toggle the debug data visibility
						physxManager->setDebugDataVisible(!physxManager->isDebugDataVisible());
						return true;
					case KEY_KEY_Q: 
						// Toggle the mouse controlling the camera on and off
						camera->setInputReceiverEnabled(!camera->isInputReceiverEnabled());
						device->getCursorControl()->setVisible(!device->getCursorControl()->isVisible());
						return true;
					case KEY_ESCAPE:
						// Close the device, and hence exit the game
						device->closeDevice();
						return true;
				}

			} else if (event.EventType == EET_GUI_EVENT) {
				
				if (event.GUIEvent.Caller == paramChanger->getUpdateButton()) {
					f32 value;
					if (paramChanger->getParam(L"Object Density", value))
						objectDensity = value;
					else
						objectDensity = 50;
					if (paramChanger->getParam(L"Gravity", value))
						physxManager->setGravity(core::vector3df(0,value,0));
					else
						physxManager->setGravity(core::vector3df(0,-9.81f,0));
					return false; // if we return true here then the button doesn't look like it's been pressed ;)
				}

			}

			return false;

		}

};

int main() {

	// Create our Irrlicht device
	CEventReceiver receiver;
	device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(800,600), 24, false, false, false, &receiver);
	if (!device) {
		printf("failed irrlicht device creation\n");
		return 1;
	}

	// Set the window's title
	wchar_t wstr[50];
	swprintf(wstr, 50, L"IrrPhysx(%.2f)::Cloth Example", IRRPHYSX_VERSION);
	device->setWindowCaption(wstr);

	// Grab some useful pointers
	smgr = device->getSceneManager();
	driver = device->getVideoDriver();
	guienv = device->getGUIEnvironment();

	// Create an FPS camera so we can see the scene and easily move around
	camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(50,50,50));
	camera->setTarget(core::vector3df(0,0,0));
	device->getCursorControl()->setVisible(false);

	// Add a light to make things look a bit nicer
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(0,150,-50), video::SColorf(1,1,1,1), 300);
	// Add a billboard to show the light's position
	scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(light, core::dimension2d<f32>(25,25));
	bill->setMaterialTexture(0, driver->getTexture("../media/light.jpg"));
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialFlag(video::EMF_LIGHTING, false);

	// Create our Physx manager which will help us deal with all the physics stuff
	SSceneDesc sceneDesc;
	physxManager = createPhysxManager(device, sceneDesc);

	// Add a floor at the origin, horizontal
	addPlane(physxManager, smgr, 0, core::vector3df(0,1,0));

	// Add a static box to attach tearable cloths to
	staticBox = createBox(physxManager, smgr, driver, core::vector3df(0,70,0), core::vector3df(15,5,5), 0)->PhysxObject;

	// Create our IPhysxMeshs for the cloths we're going to create, this only has to be done once and should be done before rendering begins as it can take time
	scene::IAnimatedMesh* flagMesh = smgr->getMesh("../media/flatplane.ms3d");
	flatClothMesh10x5x1 = physxManager->createClothMesh(flagMesh->getMeshBuffer(0), core::vector3df(10,5,1), false);
	// Now one of our sydney mesh
	scene::IAnimatedMesh* sydneyMesh = smgr->getMesh("../media/sydney.ms3d");
	sydneyClothMesh = physxManager->createClothMesh(sydneyMesh->getMeshBuffer(0), core::vector3df(1,1,1), false);

	// Add a nice font and some useful gui texts
	guienv->getSkin()->setFont(guienv->getFont("../media/GillSans12.png"));
	guienv->addImage(core::rect<s32>(10,10,215,290));
	gui::IGUIStaticText* fpsText = guienv->addStaticText(L"FPS: 0", core::rect<s32>(15,10,200,200));
	fpsText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* numObjText = guienv->addStaticText(L"Num Objects: 0", core::rect<s32>(15,30,200,200));
	numObjText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* generalText = guienv->addStaticText(L"Cursors: Move\nMouse: Look\nQ: Toggle Mouse For Camera\nSPACE: Throw Sphere\nW: Create Cube\nF: Create Cloth Flag & Pole\nS: Create Cloth Sydney\nC: Clear all objects\nV: Toggle debug data visible\nESC: Quit", core::rect<s32>(15,50,230,350));
	generalText->setOverrideColor(video::SColor(255,255,255,255));
	guienv->addImage(driver->getTexture("../media/irrlichtirrphysxlogo.png"), core::position2d<s32>(800-150,600-120));
	guienv->addImage(driver->getTexture("../media/nvidiaphysxlogo.png"), core::position2d<s32>(25,600-75));

	// Add a 'param changer' GUI element to allow us to easily change some params of the scene
	paramChanger = new CGUIParamChanger(guienv, guienv->getRootGUIElement(), -1, core::rect<s32>(600,0,800,105));
	paramChanger->setText(L"IrrPhysx Parameters");
	core::vector3df gravity;
	physxManager->getGravity(gravity);
	paramChanger->addParam(L"Gravity", gravity.Y);
	paramChanger->addParam(L"Object Density", objectDensity);
		
	s32 lastTime = device->getTimer()->getTime();
	// We're ready to render now!
	while (device->run()) {
		
		if (device->isWindowActive()) {

			// Calculate the elapsed time since the last frame
			s32 timeNow = device->getTimer()->getTime();
			s32 elapsedTime = timeNow - lastTime;
			lastTime = timeNow;

			// Apply some random wind to the flags
			for (u32 i = 0 ; i < flags.size() ; i++) 
				((IClothPhysxObject*)flags[i]->PhysxObject)->applyWindToCloth(
					core::vector3df( 5.0f * (25.0f + rand(-WIND_RANGE, WIND_RANGE)),
									 5.0f * rand(-WIND_RANGE, WIND_RANGE),
									5.0f * rand(-WIND_RANGE, WIND_RANGE) )
				);

			// Set the physics simulation going
			physxManager->simulate(elapsedTime/1000.0f);

			// Whilst we're waiting we can do some non-physics related work
			// you must NEVER add/move physics objects whilst the simulation
			// is being done though, doing so will cause all kinds of horrible crashes!
			core::stringw strw;
			strw = "FPS: ";
			strw += driver->getFPS();
			fpsText->setText(strw.c_str());
			strw = "Num Objects: ";
			strw += physxManager->getNumPhysxObjects();
			numObjText->setText(strw.c_str());
			// Wait for the physics simulation to finish
			physxManager->fetchResults();

			// Update our objects to the correct transformations
			for (u32 i = 0 ; i < objects.size() ; i++) 
				objects[i]->updateTransformation();
		
			// Begin the render process
			driver->beginScene(true, true, video::SColor(225,120,120,120));

			// Render the scene
			smgr->drawAll();
		
			// Render Physx debug data
			physxManager->renderDebugData(video::SColor(225,255,255,255));

			// Render the GUI
			guienv->drawAll();

			// Finish the render process
			driver->endScene();

		} else device->yield();

	}

	// Clean up the physx manager
	removePhysxManager(physxManager);
	physxManager = 0;

	// Clean up the device
	device->drop();
	device = 0;

	// And we're done.
    return 0;

}
