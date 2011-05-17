
#include <irrlicht.h>
#include <IrrPhysx.h>
#include "ShapeCreation.h"
#include "CCapsuleSceneNode.h"
#include "CGUIParamChanger.h"
#include <stdio.h>

using namespace irr;
using namespace IrrPhysx;

IrrlichtDevice* device = NULL;
scene::ISceneManager* smgr = NULL;
video::IVideoDriver* driver = NULL;
gui::IGUIEnvironment* guienv = NULL;
scene::ICameraSceneNode* camera = NULL;
IPhysxManager* physxManager = NULL;
core::array<SPhysxAndNodePair*> objects;
f32 objectDensity = 50;
CGUIParamChanger* paramChanger = NULL;

SPhysxAndNodePair* createCapsule(const core::vector3df& pos, f32 radius = 2.0f, f32 height = 2.0f, f32 density = 10.0f, core::vector3df* initialVelocity = NULL) {

	SPhysxAndNodePair* pair = new SPhysxAndNodePair;
	pair->PhysxObject = physxManager->createCapsuleObject(pos, core::vector3df(0,0,0), radius, height, density, initialVelocity);
	pair->SceneNode = new CCapsuleSceneNode(radius, height, 12, 12, smgr->getRootSceneNode(), smgr, -1, pos, core::vector3df(0,0,0), core::vector3df(1,1,1));
	pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	pair->SceneNode->drop();
	
	return pair;

}

// Creates a shape created from a box, sphere and capsule at the specified position, of the specified size and with the specified intial velocity (useful for throwing it)
SPhysxAndNodePair* createMultiShapeObject(const core::vector3df& pos, const core::vector3df& scale = core::vector3df(2,2,2), f32 density = 10.0f, core::vector3df* initialVelocity = NULL) {

	SPhysxAndNodePair* pair = new SPhysxAndNodePair;

	// Create a box description
	core::array<core::aabbox3df> boxes;
	core::aabbox3df box;
	core::vector3df halfScale = scale/2.0f;
	box.reset(core::vector3df(-0.5f,-0.5f,-0.5f) * halfScale);
	box.addInternalPoint(core::vector3df(0.5f,0.5f,0.5f) * halfScale);
	boxes.push_back(box);

	// Create a sphere description
	core::array<SSphereDesc> spheres;
	SSphereDesc sphere;
	sphere.Radius = 2;
	sphere.Position = core::vector3df(-0.5f,-0.5f,-0.5f) * halfScale;
	spheres.push_back(sphere);

	// Create a capsule description
	core::array<SCapsuleDesc> capsules;
	SCapsuleDesc capsule;
	capsule.Radius = 2;
	capsule.Height = 4;
	capsule.Position = core::vector3df(0.5f,0.5f,0.5f) * halfScale;
	capsules.push_back(capsule);

	// Create the multishape Physx object
	pair->PhysxObject = physxManager->createMultiShapeObject(boxes, spheres, capsules, pos, core::vector3df(0,0,0), density, initialVelocity);

	// Create scene nodes to represent it visually.
	// As the objects will always be stuck together (in this example at least) we can just parent them to keep them together
	pair->SceneNode = smgr->addCubeSceneNode(1, 0, -1, pos, core::vector3df(0,0,0), scale);
	pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 

	scene::ISceneNode* sphereNode = smgr->addSphereSceneNode(sphere.Radius, 16, pair->SceneNode, -1, sphere.Position / scale, core::vector3df(0,0,0), core::vector3df(1/scale.X, 1/scale.Y, 1/scale.Z));
	sphereNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	sphereNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 

	scene::ISceneNode* capsuleNode = new CCapsuleSceneNode(capsule.Radius, capsule.Height, 16, 16, pair->SceneNode, smgr, -1, capsule.Position / scale, core::vector3df(0,0,0), core::vector3df(1/scale.X, 1/scale.Y, 1/scale.Z));
	capsuleNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	capsuleNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 
	
	return pair;

}

// Our event reciever to catch key presses
class CEventReceiver : public IEventReceiver {

	public:
		bool OnEvent(const SEvent& event) {

			if (event.EventType == EET_KEY_INPUT_EVENT) {
				
				if (event.KeyInput.PressedDown) return false;

				switch (event.KeyInput.Key) {
					case KEY_KEY_W:
						// Create a double box at the specified position
						objects.push_back(createMultiShapeObject(core::vector3df(0.0f, 20.0f, 0.0f), core::vector3df(4,4,4), objectDensity));
						return true;
					case KEY_KEY_T:
						// Create a table object
						objects.push_back(createMeshBoundingBox(physxManager, smgr, driver, smgr->getMesh("../media/table.ms3d"), core::vector3df(0,50,0), core::vector3df(1,1,1), objectDensity));
						return true;
					case KEY_KEY_K:
						// Create a capsule object
						objects.push_back(createCapsule(core::vector3df(0,50,0), (f32)(rand()%10)+1, (f32)(rand()%10)+1, objectDensity));
						return true;
					case KEY_KEY_S:
						// Create a stack of tables with 10 along the bottom
						createMeshStack(physxManager, smgr, driver, objects, smgr->getMesh("../media/table.ms3d"), 5, objectDensity); 
						return true;
					case KEY_KEY_B:
						// Create a stack of tables with 30 along the bottom
						createMeshStack(physxManager, smgr, driver, objects, smgr->getMesh("../media/table.ms3d"), 15, objectDensity); 
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
	swprintf(wstr, 50, L"IrrPhysx(%.2f)::MultiShape Example", IRRPHYSX_VERSION);
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
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(0,150,50), video::SColorf(1,1,1,1), 300);
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

	// Add a nice font and some useful gui texts
	guienv->getSkin()->setFont(guienv->getFont("../media/GillSans12.png"));
	guienv->addImage(core::rect<s32>(10,10,215,330));
	gui::IGUIStaticText* fpsText = guienv->addStaticText(L"FPS: 0", core::rect<s32>(15,10,200,200));
	fpsText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* numObjText = guienv->addStaticText(L"Num Objects: 0", core::rect<s32>(15,30,200,200));
	numObjText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* generalText = guienv->addStaticText(L"Cursors: Move\nMouse: Look\nQ: Toggle Mouse For Camera\nSPACE: Throw Sphere\nW: Create Complex Shape\nK: Create Random Capsule\nT: Create Table\nS: Create Table Stack (small)\nB: Create Table Stack (big)\nC: Clear all objects\nV: Toggle debug data visible\nESC: Quit", core::rect<s32>(15,50,250,350));
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

			// Set the physics simulation going
			physxManager->simulate(elapsedTime/1000.0f);

			// Whilst we're waiting we can do some non-physics related work
			// you must NEVER add/remove/move physics objects whilst the simulation
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
