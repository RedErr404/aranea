
#include <irrlicht.h>
#include <IrrPhysx.h>
#include "ShapeCreation.h"
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

#define NUM_TEXT_MESHES 8
c8 textMeshPaths[NUM_TEXT_MESHES][25] = {
	"../media/text/I.ms3d",
	"../media/text/r.ms3d",
	"../media/text/r.ms3d",
	"../media/text/P.ms3d",
	"../media/text/h.ms3d",
	"../media/text/y.ms3d",
	"../media/text/s.ms3d",
	"../media/text/x.ms3d"
};

SPhysxAndNodePair* createLetter(s32 letterIndex, const core::vector3df& pos, const core::vector3df& scale) {

	scene::IMesh* mesh = smgr->getMesh(textMeshPaths[letterIndex]);

	SPhysxAndNodePair* pair = new SPhysxAndNodePair;
	// Ideally we would only create the following IPhysxMesh once and reuse it if we wanted another object that used the same mesh
	// In the IrrPhysx Game Example there's a simple mesh cache shown for physx meshes
	IPhysxMesh* physxMesh = physxManager->createConvexMesh(mesh->getMeshBuffer(0), scale);
	pair->PhysxObject = physxManager->createConvexMeshObject(physxMesh, pos, core::vector3df(0,0,0), objectDensity);
	pair->SceneNode = smgr->addMeshSceneNode(mesh, NULL, -1, pos, core::vector3df(0,0,0), scale);
	pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	
	return pair;

}

// Our event reciever to catch key presses
class CEventReceiver : public IEventReceiver {

	public:
		bool OnEvent(const SEvent& event) {

			if (event.EventType == EET_KEY_INPUT_EVENT) {
				
				if (event.KeyInput.PressedDown) return false;

				switch (event.KeyInput.Key) {
					case KEY_KEY_L:
						// Create a letter at the specified position
						objects.push_back(createLetter(rand()%NUM_TEXT_MESHES, core::vector3df(0.0f, 100.0f, 0.0f), core::vector3df(2,2,2)));
						return true;
					case KEY_SPACE:
						// Throw a sphere from the camera
						objects.push_back(createSphereFromCamera(physxManager, smgr, driver, camera, 8));
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
	device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(800,600), 24, false, false, true, &receiver);
	if (!device) {
		printf("failed irrlicht device creation\n");
		return 1;
	}

	// Set the window's title
	wchar_t wstr[50];
	swprintf(wstr, 50, L"IrrPhysx(%.2f)::Mesh Example", IRRPHYSX_VERSION);
	device->setWindowCaption(wstr);

	// Grab some useful pointers
	smgr = device->getSceneManager();
	driver = device->getVideoDriver();
	guienv = device->getGUIEnvironment();

	// Create an FPS camera so we can see the scene and easily move around
	camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(50,50,-50));
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

	// Add some convex meshes - always non-static
	for (u32 i = 0 ; i < NUM_TEXT_MESHES ; ++i) {
		core::vector3df pos((i - (NUM_TEXT_MESHES / 2.0f)) * 20.0f, 100.0f, 0);
		objects.push_back(createLetter(i, pos, core::vector3df(4,4,4)));
	}

	// Add a triangle mesh - always static
	scene::IMesh* mesh = smgr->getMesh("../media/star.ms3d");
	core::vector3df pos(0,0,100);
	core::vector3df rot(0,180,0);
	core::vector3df scale(2,2,2);
	SPhysxAndNodePair* pair = new SPhysxAndNodePair;
	IPhysxMesh* physxMesh = physxManager->createTriangleMesh(mesh->getMeshBuffer(0), scale);
	pair->PhysxObject = physxManager->createTriangleMeshObject(physxMesh, pos, rot);
	pair->SceneNode = smgr->addMeshSceneNode(mesh, NULL, -1, pos, rot, scale);
	pair->SceneNode->setMaterialTexture(0, driver->getTexture("../media/yellow.JPG"));
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	objects.push_back(pair);

	// Add a nice font and some useful gui texts
	guienv->getSkin()->setFont(guienv->getFont("../media/GillSans12.png"));
	guienv->addImage(core::rect<s32>(10,10,215,240));
	gui::IGUIStaticText* fpsText = guienv->addStaticText(L"FPS: 0", core::rect<s32>(15,10,200,200));
	fpsText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* numObjText = guienv->addStaticText(L"Num Objects: 0", core::rect<s32>(15,30,200,200));
	numObjText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* generalText = guienv->addStaticText(L"Cursors: Move\nMouse: Look\nQ: Toggle Mouse For Camera\nSPACE: Throw Sphere\nL: Create Letter\nC: Clear all objects\nV: Toggle debug data visible\nESC: Quit", core::rect<s32>(15,50,250,350));
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
