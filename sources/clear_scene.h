#include "irrlicht.h"
#include <string>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;


void clearAllScene()
{

	//video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	//safely remove physx world
	for (u32 i = 0 ; i < objects.size() ; i++)
	{
		/*SPhysxAndNodePair* pair = objects[i];
		physxManager->removePhysxObject(pair->PhysxObject);
		pair->SceneNode->remove();
		delete pair;
        */

		physxManager->removePhysxObject(objects[i]->PhysxObject);
		objects[i]->SceneNode->remove();
		delete objects[i];
		objects.erase(i);
		--i;


	}
	objects.clear();
	physxManager->resetScene(sceneDesc);


//safely remove elements
pistolIsVisible = 0;
binGUIisOn = 0;
fpsValOn   = 0;

//stop audio
if (musicOn == 1)
{
	channelMusic->stop();
	systemFMOD->update();
}

//first remove all models loaded, clear the scene, leave some textures

smgr->clear();

guienv->clear();

smgr->getMeshCache()->clear();


//disable main menu background image
isBackImg = 0;
}
