
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

// Our event reciever to catch key presses
class CEventReceiver : public IEventReceiver {

	public:
		bool OnEvent(const SEvent& event) {

			if (event.EventType == EET_KEY_INPUT_EVENT) {
				
				if (event.KeyInput.PressedDown) return false;

				switch (event.KeyInput.Key) {
					case KEY_KEY_W:
						// Create a box at the specified position
						objects.push_back(createBox(physxManager, smgr, driver, core::vector3df(0.0f, 100.0f, 0.0f), core::vector3df(2,2,2), objectDensity));
						return true;
					case KEY_KEY_T:
						// Create a tower of 30 boxes
						createBoxTower(physxManager, smgr, driver, objects, 30, objectDensity);
						return true;
					case KEY_KEY_S:
						// Create a stack of boxes with 10 along the bottom
						createBoxStack(physxManager, smgr, driver, objects, 10, objectDensity); 
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
	swprintf(wstr, 50, L"IrrPhysx(%.2f)::Raycast Example", IRRPHYSX_VERSION);
	device->setWindowCaption(wstr);

	// Grab some useful pointers
	smgr = device->getSceneManager();
	driver = device->getVideoDriver();
	guienv = device->getGUIEnvironment();

	// Create an FPS camera so we can see the scene and easily move around
	camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(-100,50,-150));
	camera->setTarget(core::vector3df(0,0,0));
	device->getCursorControl()->setVisible(false);

	// Add a light to make things look a bit nicer
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(0,150,50), video::SColorf(1,1,1,1), 300);
	// Add a billboard to show the light's position
	scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(light, core::dimension2d<f32>(25,25));
	bill->setMaterialTexture(0, driver->getTexture("../media/light.jpg"));
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialFlag(video::EMF_LIGHTING, false);

	// Add billboard to show raycast intersection
	scene::IBillboardSceneNode * intersectionBill = smgr->addBillboardSceneNode();
	intersectionBill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	intersectionBill->setMaterialTexture(0, driver->getTexture("../media/particle.jpg"));
	intersectionBill->setMaterialFlag(video::EMF_LIGHTING, false);
	intersectionBill->setMaterialFlag(video::EMF_ZBUFFER, false);
	intersectionBill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	intersectionBill->setVisible(false);

	// Create a material for our 'normal' line
	video::SMaterial lineMaterial;
	lineMaterial.Lighting = false;
	lineMaterial.Thickness = 2.0f;

	// Create a material for our triangle
	video::SMaterial triMaterial;
	triMaterial.Lighting = false;

	// Set some Physx scene description parameters to optimise raycasting, see the documentation for SSceneDesc to understand these parameters
	SSceneDesc sceneDesc;
	// Set the 'bounding box' of the Physx scene to be 2400x1000x2400 (enough to enclose our scene nicely)
	sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-1200,-200,-1200), core::vector3df(1200,800,1200));
	// We want to create planes for the bounding box so objects won't go outside the bounds
	sceneDesc.BoundPlanes = true;
	// Our up axis is the Y axis
	sceneDesc.UpAxis = 1;
	// Create our Physx manager which will help us deal with all the physics stuff
	physxManager = createPhysxManager(device, sceneDesc);

	// Add the good ol' Quake 3 level
	core::vector3df pos(-1350,-130,-1400);
	device->getFileSystem()->addZipFileArchive("../media/q3level.pk3");
	scene::IMesh* q3levelmesh = smgr->getMesh("20kdm2.bsp")->getMesh(0);
	scene::ISceneNode* q3node = smgr->addOctTreeSceneNode(q3levelmesh);
	q3node->setPosition(pos);
	// Create a triangle mesh from each mesh buffer
	for (u32 i = 0 ; i < q3levelmesh->getMeshBufferCount() ; i++) {
		IPhysxMesh* triMesh = physxManager->createTriangleMesh(q3levelmesh->getMeshBuffer(i));
		physxManager->createTriangleMeshObject(triMesh, pos);
	}

	// Add a nice font and some useful gui texts
	guienv->getSkin()->setFont(guienv->getFont("../media/GillSans12.png"));
	guienv->addImage(core::rect<s32>(10,10,215,290));
	gui::IGUIStaticText* fpsText = guienv->addStaticText(L"FPS: 0", core::rect<s32>(15,10,200,200));
	fpsText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* numObjText = guienv->addStaticText(L"Num Objects: 0", core::rect<s32>(15,30,200,200));
	numObjText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* generalText = guienv->addStaticText(L"Cursors: Move\nMouse: Look\nQ: Toggle Mouse For Camera\nSPACE: Throw Sphere\nW: Create Cube\nT: Create Tower\nS: Create Stack (small)\nC: Clear all objects\nV: Toggle debug data visible\nESC: Quit", core::rect<s32>(15,50,250,350));
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

			// Perform a raycast straight out from the camera to detect the nearest object to the cam
			core::line3df line;
			line.start = camera->getPosition();
			line.end = line.start + (camera->getTarget() - line.start).normalize() * 1000.0f;
			core::vector3df intersection;
			core::vector3df normal;
			IPhysxObject* objectHit = physxManager->raycastClosestObject(line, &intersection, &normal);
			if (objectHit) {
				intersectionBill->setVisible(true);
				intersectionBill->setPosition(intersection);
				intersectionBill->updateAbsolutePosition();

				// Draw a 3D line showing the normal of the surface hit
				core::vector3df lineEnd = intersection + normal * 10.0f;
				driver->setTransform(video::ETS_WORLD, core::matrix4());
				driver->setMaterial(lineMaterial);
				driver->draw3DLine(intersection, lineEnd, video::SColor(255,0,0,255));
			}
		
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
