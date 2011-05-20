#include "irrlicht.h"
#include <string>
#include <iostream>
#include "CGrassPatchSceneNode.h"
#include "CWindGenerator.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;


   //variables for the grass
	#define width  10
	#define height 10
	#define total  width*height

class MyAnimationEndCallBack : public scene::IAnimationEndCallBack
{
public:
    virtual void OnAnimationEnd(scene::IAnimatedMeshSceneNode* node)
    {
       pistolNode->setLoopMode(true);
       pistolNode->setAnimationSpeed(20.0f);
       pistolNode->setFrameLoop(65,100);      //idle
       animationEnded = 1;
    }
};


void show_console()
{
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();



	modal_CON_window = guienv->addWindow(rect<s32>(1,1,screenResX-1,screenResY/2), false, consoleLValue.c_str());
	modal_CON_window->getCloseButton()->setVisible(false);

     guienv->addStaticText(L">", rect<s32>(10,40, 23,80), false, false, modal_CON_window, -1, false);
     consoleInput = guienv->addEditBox(L"", rect<s32>(25,40, screenResX-300,80), true, modal_CON_window, 200);
     guienv->addButton(rect<s32>(screenResX-290 ,40, (screenResX-160), 80), modal_CON_window, 201, submitLValue.c_str());
     guienv->addButton(rect<s32>(screenResX-150 ,40, (screenResX-40), 80), modal_CON_window, 202, closeLValue.c_str());

	 fpsValOn = 1;
     fpsVal = guienv->addStaticText(L"FPS: N/A", rect<s32>(10,100, 100,140), false, false, modal_CON_window, 222, false);

//TODO: delete these 2 lines    show cursor | also check if in menu already or in game
//device->getCursorControl()->setVisible(true);



}

void showLoadProgress()
{
	fpsValOn = 0;
	video::IVideoDriver* driver  = device->getVideoDriver();
    ISceneManager* smgr          = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();


			// draw loading bar animation
			//u32 timeNow3 = device->getTimer()->getTime();
			device->run();
			driver->beginScene(true, true, video::SColor(0, 0,0,0));
            smgr->drawAll();

            if (isPleaseWaitImg==1)
		    {
			driver->draw2DImage(pleaseWaitImg,core::rect<s32>(0,0,screenResX,screenResY), core::rect<s32>(0,0,1024,768), 0,0,false);
		    }

			if(drawNextSlideLB==1) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb1,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==2) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb2,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==3) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb3,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==4) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb4,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==5) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb5,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==6) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb6,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==7) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb7,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==8) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb8,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==9) {driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb9,  0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==10){driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb10, 0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==11){driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb11, 0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==12){driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb12, 0, video::SColor(255,255,255,255), true);}
			if(drawNextSlideLB==13){driver->draw2DImage(loadingBar, core::position2d<s32>(900,521), lb13, 0, video::SColor(255,255,255,255), true);}


            drawNextSlideLB = drawNextSlideLB + 1;
			if (drawNextSlideLB>=13){drawNextSlideLB = 1;}else{}

            guienv->drawAll();
			driver->endScene();
}





void loadMap(core::stringw mapNameb, u32 XMLfogValueR, u32 XMLfogValueG, u32 XMLfogValueB, core::stringw XMLskyValue)
{
	video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();


	screenShot_fader = guienv->addInOutFader();
    screenShot_fader->setColor(video::SColor(0,0,0,0));

    escFadeToBlack = guienv->addInOutFader();
    escFadeToBlack->setColor(video::SColor(0,0,0,0));

    //create the console, then hide it. It can be accesed with F12
    show_console();
    modal_CON_window->setVisible(false);


    smgr->setAmbientLight(video::SColorf(0.0f, 0.5f, 0.5f, 0.5f));

	//load the HUD elements
	lifeBar          = driver->getTexture("../data/gui/ingame/lifeBar.png");
	ammoStats        = driver->getTexture("../data/gui/ingame/ammoStats.png");

	lifeBarLow       = driver->getTexture("../data/gui/ingame/lifeBarLow.png");
	ammoStatsLow     = driver->getTexture("../data/gui/ingame/ammoStatsLow.png");

    //hide the cursor
    isInGame = 1;

    device->getCursorControl()->setVisible(false);
	showLoadProgress();
    
    //add fog
    driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);
    driver->setFog(video::SColor(0, XMLfogValueR, XMLfogValueG, XMLfogValueB), true, 0.0f, farView+50.0f, 0.0005f, false);
	showLoadProgress();
        //load map
        core::stringc newMapName = "../data/maps/";
        newMapName+= mapNameb;
        if (mapDetail==0){newMapName+= "/map_s.irr";}else{}
        if (mapDetail==1){newMapName+= "/map_h.irr";}else{}

        //TO-DO: create error safe + log here

        smgr->loadScene(newMapName.c_str());
	showLoadProgress();

	
   

   //add skydome
   core::stringc newXMLskyValue = "../data/maps/";
   newXMLskyValue+= mapNameb;
   newXMLskyValue+= "/mapdata/shaders/sky/";
   newXMLskyValue+= XMLskyValue;
   newXMLskyValue+= ".jpg";

   smgr->addSkyDomeSceneNode(driver->getTexture(newXMLskyValue),16,16,1.0f,1.1f);
   showLoadProgress();

   //camera
   playaIrrNode = smgr->getSceneNodeFromName("player", 0);
   camera = smgr->addCameraSceneNodeFPS(0, 45.0f, 0.00f, -1);

   	playa = new SPhysxAndNodePair;
	playa->PhysxObject = physxManager->createCapsuleObject(playaIrrNode->getPosition(), core::vector3df(0,0,0), 5.0f, 35.0f, 600000, 0);
	playa->SceneNode = smgr->addSphereSceneNode(70.0f, 12, 0, -1, playaIrrNode->getPosition(), core::vector3df(0,0,0), core::vector3df(1,1,1));
	playa->SceneNode->setVisible(false);	
	playa->PhysxObject->setLinearDamping(5.0f);
	objects.push_back(playa);

	camera->setFarValue(f32(farView));
    //camera->setAspectRatio(16/10);
    camera->setFOV(PI / 2.5f);
    camera->setNearValue(0.1f);

 
   if(playaIrrNode){playa->PhysxObject->setPosition(core::vector3df(playaIrrNode->getPosition()));}else{camera->setPosition(core::vector3df(0,1000,0));}
	showLoadProgress();


	//Loop through the nodes and apply collisions


	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // Find all nodes

	for (u32 i=0; i < nodes.size(); ++i)
	{
	showLoadProgress();
		

		scene::ISceneNode * node = nodes[i];

		switch(node->getType())
		{
		case scene::ESNT_CUBE:
		case scene::ESNT_SPHERE:
		case scene::ESNT_ANIMATED_MESH:
            node->setMaterialFlag(video::EMF_FOG_ENABLE, true);
		break;


        case scene::ESNT_MESH:
        {
		    node->setMaterialFlag(video::EMF_FOG_ENABLE, true);

            u32 nodeNamen = node->getID();
            nodeNameX  = node->getName();

			if (nodeNamen == 10 || nodeNamen == 11 || nodeNamen == 25 || nodeNamen == 50) //see NODE_ID_LIST.txt
            { 
			printf("[INFO]: Special ID node found : %s\n", nodeNameX);
			} else { 

			 if(nodeNamen == 0)
			 {
				//don't add collision data for this node
			 }else{
			ISceneManager* smgr = device->getSceneManager();
            attribs = device->getFileSystem()->createEmptyAttributes();
            node->serializeAttributes(attribs);
            core::stringc nameXX = attribs->getAttributeAsString("Mesh");
            attribs->drop();
            scene::IMesh* groundMesh = 0;
            groundMesh = smgr->getMesh(nameXX.c_str());

			core::vector3df pos  = node->getPosition();
			core::vector3df rot  = node->getRotation();
			core::vector3df sca  = node->getScale();

			SPhysxAndNodePair* pair = new SPhysxAndNodePair;
			IPhysxMesh* physxMesh = physxManager->createTriangleMesh(groundMesh->getMeshBuffer(0), sca);
			pair->PhysxObject = physxManager->createTriangleMeshObject(physxMesh, pos, rot);
			pair->SceneNode = smgr->addMeshSceneNode(groundMesh, NULL, -1, pos, rot, sca);
			pair->SceneNode->setVisible(false);
			objects.push_back(pair);
			 }
			}



			if (nodeNamen == 50) //vehicle Jeep
			{
			printf("[INFO]: Special ID node found (vehicle): %s\n", nodeNameX);
			ISceneManager* smgr = device->getSceneManager();
			
			core::vector3df pos2  = node->getPosition();
			core::vector3df rot2  = node->getRotation();
			core::vector3df sca2  = node->getScale();
			
            attribs2 = device->getFileSystem()->createEmptyAttributes();
            node->serializeAttributes(attribs2);
            core::stringc nameXX2 = attribs2->getAttributeAsString("Mesh");
            attribs2->drop();
            scene::IMesh* groundMesh2 = 0;
            groundMesh2 = smgr->getMesh(nameXX2.c_str());

  			vehicleJeep = new SPhysxAndNodePair;
			IPhysxMesh* groundMesh3 = physxManager->createConvexMesh(groundMesh2->getMeshBuffer(0), sca2);
			vehicleJeep->PhysxObject = physxManager->createConvexMeshObject(groundMesh3, pos2, core::vector3df(0,0,0), 20000);
			vehicleJeep->SceneNode = smgr->addMeshSceneNode(groundMesh2, 0, -1, pos2, core::vector3df(0,0,0), sca2);
			vehicleJeep->PhysxObject->setLinearDamping(1.0f);
			vehicleJeep->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
			vehicleJeep->SceneNode->setID(50);
			objects.push_back(vehicleJeep);

			node->setVisible(false);
			smgr->addToDeletionQueue(node);
			

			}



			if (nodeNamen == 25) //explosive container
            {            
            node->setVisible(false);

			ISceneManager* smgr = device->getSceneManager();		

			core::vector3df pos2  = node->getPosition();
			core::vector3df rot2  = node->getRotation();
			core::vector3df sca2  = node->getScale();

			
            attribs2 = device->getFileSystem()->createEmptyAttributes();
            node->serializeAttributes(attribs2);
            core::stringc nameXX2 = attribs2->getAttributeAsString("Mesh");
            attribs2->drop();
            scene::IMesh* groundMesh2 = 0;
            groundMesh2 = smgr->getMesh(nameXX2.c_str());


			barrelNodePair = new SPhysxAndNodePair;
			IPhysxMesh* groundMesh3 = physxManager->createConvexMesh(groundMesh2->getMeshBuffer(0), sca2);
			barrelNodePair->PhysxObject = physxManager->createConvexMeshObject(groundMesh3, pos2, core::vector3df(0,0,0), 100000);
			barrelNodePair->SceneNode = smgr->addMeshSceneNode(groundMesh2, 0, -1, pos2, core::vector3df(0,0,0), sca2);
			barrelNodePair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
			barrelNodePair->SceneNode->setID(25);
            objects.push_back(barrelNodePair);

            }




			

        }
		break;


		case scene::ESNT_TERRAIN:
            node->setMaterialFlag(video::EMF_FOG_ENABLE, true);
				
			terrObj = physxManager->createHeightfieldObject((scene::ITerrainSceneNode*)node, 0);
			node->setScale(vector3df(40.000000f, 4.000000f, 40.000000f));			
			terrObj->setPosition(core::vector3df(node->getPosition().X+280.0f, node->getPosition().Y - 760.0f, node->getPosition().Z+300.0f ));


if (grass == 1)
{
   // create wind...
   scene::IWindGenerator *wind;
   wind = scene::createWindGenerator( 10.0f, 3.0f );

	newHeightmapName = "../data/maps/"; newHeightmapName+= mapNameb; newHeightmapName+= "/mapdata/ents/terrain/heightmap.bmp";
	newTerrainClName = "../data/maps/"; newTerrainClName+= mapNameb; newTerrainClName+= "/mapdata/ents/terrain/terrain-grasscol.bmp";
	newGrassmapName  = "../data/maps/"; newGrassmapName+= mapNameb;  newGrassmapName+= "/mapdata/ents/terrain/grassmap.png";

 	heightMap  = driver->createImageFromFile (newHeightmapName.c_str());
 	textureMap = driver->createImageFromFile (newTerrainClName.c_str());
 	grassMap   = driver->createImageFromFile ( newGrassmapName.c_str());

     grassTex1 = driver->getTexture("../data/shaders/grass/grass.png");
     grassTex2 = driver->getTexture("../data/shaders/grass/grass.png");

     scene::CGrassPatchSceneNode *grass[width*height];
    //now we add some grass nodes

       for (int x=0; x<width; ++x)
       {
       for (int z=0; z<height; ++z)
       {
         // add a grass node
         grass[x*width + z] = new scene::CGrassPatchSceneNode((scene::ITerrainSceneNode*)node, smgr, -1, core::vector3d<s32>(x,0,z), "grass", heightMap, textureMap, grassMap, wind);
         grass[x*width + z]->setMaterialFlag(video::EMF_LIGHTING, false);
         grass[x*width + z]->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
         grass[x*width + z]->setMaterialFlag(video::EMF_FOG_ENABLE, true);
 		 grass[x*width + z]->getMaterial(0).TextureLayer[0].TextureWrap = video::ETC_CLAMP;
 		 grass[x*width + z]->getMaterial(0).MaterialTypeParam = 0.5f;
         grass[x*width + z]->setMaterialTexture(0, grassTex1);
         grass[x*width + z]->drop();
         }
       }

 	  smgr->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
} 




			break;


		case scene::ESNT_OCT_TREE:
            {
            node->setMaterialFlag(video::EMF_FOG_ENABLE, true);
            
			}
			break;



		default:
			// Don't create a selector for this node type
			break;
		}



}


    //load the GUI
	switchToBigFont();
	showLoadProgress();

  //create the player bullet
  nodeBull = smgr->addCubeSceneNode(0.1f, 0, -1, core::vector3df(0,-100,0), core::vector3df(0,0,0), core::vector3df(1,1,1));

  //load the weapons

  //load pistol

  scene::IAnimatedMesh* pistolMesh = smgr->getMesh("../data/models/ani_models/weapons/pistol_001/pistol.x");
	showLoadProgress();


  if (pistolMesh)
  {

  video::ITexture* normalMap3 = driver->getTexture("../data/models/ani_models/weapons/pistol_001/normalmap.tga");
  //driver->makeNormalMapTexture(normalMap3, 5);

  ((ISkinnedMesh*)pistolMesh)->convertMeshToTangents();
  scene::IMesh* tangentMesh3 = smgr->getMeshManipulator()->createMeshWithTangents(pistolMesh->getMesh(0));


  scene::IAnimationEndCallBack* cb = new MyAnimationEndCallBack;
  pistolNode = smgr->addAnimatedMeshSceneNode(pistolMesh);
  pistolNode->setAnimationEndCallback(cb);
  pistolNode->setMaterialTexture(1, normalMap3);
  pistolNode->setMaterialFlag(video::EMF_LIGHTING, true);
  pistolNode->getMaterial(0).Shininess = 0.1f;
  pistolNode->getMaterial(0).DiffuseColor = SColor(0,XMLfogValueR,XMLfogValueG,XMLfogValueB);
  pistolNode->getMaterial(0).AmbientColor = SColor(0,XMLfogValueR,XMLfogValueG,XMLfogValueB);
  pistolNode->getMaterial(0).SpecularColor = SColor(0,150,180,255);
  //pistolNode->setMaterialType(video::EMT_NORMAL_MAP_SOLID);
  //pistolNode->getMaterial(0).MaterialTypeParam = 0.535f;
  pistolNode->setParent(camera);
  pistolNode->setPosition(core::vector3df(-0.1f,-0.4f,0));  // left-right, up-down, close-far
  pistolNode->setScale(core::vector3df(0.02f,0.02f,0.02f));
  pistolNode->setRotation(core::vector3df(-90,0,0));
  pistolNode->setLoopMode(true);
  pistolNode->setAnimationSpeed(20.0f);
  pistolNode->setFrameLoop(65,100);      //idle

  pistolNode->setVisible(false);

  tangentMesh3->drop();
  cb->drop();
  }
  showLoadProgress();

  //create the reticule
    if (reticule == 1){ crosshair = guienv->addImage(driver->getTexture("../data/gui/ingame/crosshair/ch_001.png"),core::position2d<s32>(screenResX/2-16,screenResY/2-16),true,0,1111); }else{}
    if (reticule == 2){ crosshair = guienv->addImage(driver->getTexture("../data/gui/ingame/crosshair/ch_002.png"),core::position2d<s32>(screenResX/2-16,screenResY/2-16),true,0,1111); }else{}
    if (reticule == 3){ crosshair = guienv->addImage(driver->getTexture("../data/gui/ingame/crosshair/ch_003.png"),core::position2d<s32>(screenResX/2-16,screenResY/2-16),true,0,1111); }else{}
	showLoadProgress();
	
	// Add billboard to show raycast intersection
	intersectionBill = smgr->addBillboardSceneNode();
	intersectionBill->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	intersectionBill->setMaterialTexture(0, driver->getTexture("../data/gui/ingame/crosshair/ch_001.png"));
	intersectionBill->setMaterialFlag(video::EMF_LIGHTING, false);
	intersectionBill->setMaterialFlag(video::EMF_ZBUFFER, false);
	intersectionBill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	intersectionBill->setVisible(false);
	showLoadProgress();


	lifeBarText      = guienv->addStaticText(L"100", core::rect<s32>(82 ,695,142,740), false, false);
    ammoStatsText    = guienv->addStaticText(L"48",  core::rect<s32>(881,695,943,740), false, false);

	//torqueValueText  = guienv->addStaticText(L"0",  core::rect<s32>(10,10,100,60), false, false);

	lifeBarValue = 100;//default life bar
	ammoStatsValue = 48;//default bullets

	showLoadProgress();
    
	if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, ambientWind, false, &channelMusic); ERRCHECK(result); }else{}
    showLoadProgress();

 printf("[INFO] Map loaded.\n");
 
}