#include <irrlicht.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <cstdlib>
#include <IrrPhysx.h>
#include "ShapeCreation.h"
#include "CCapsuleSceneNode.h"
#include "CGUIParamChanger.h"
#include "irrXML.h"
#include "main.h"
#include "load_audio.h"
#include "switchFont.h"
#include "weaponMotion.h"
#include "special_effects.h"
#include "PostProcessing.h"
#include "load_map.h"
#include "main_menu.h"
#include "clear_scene.h"
#include "vehicleLogic.h"




using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;
using namespace FMOD;
using namespace IrrPhysx;

#pragma comment(lib, "Irrlicht.lib")



void *physicsAlloc(int size)
{

    return new char[size];

}

void physicsFree(void *pointer, int size)
{

	char *tmp;
	tmp = (char*) pointer;
	delete[] tmp;

} 

//here we ask the user if he wants to go to main menu,
//exit the game or just close the modal window
void menu_exit_cancel()
{
gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

if (escWait == 1)
{

  //load the GUI

    switchToSmallFont();

	modal_MEC_window = guienv->addWindow(rect<s32>(0,0,screenResX,screenResY ), true, L"");
	modal_MEC_window->getCloseButton()->setVisible(false);

     guienv->addButton(rect<s32>(331 ,270, 690, 310), modal_MEC_window, 107, mainMenuLValue.c_str());
     guienv->addButton(rect<s32>(331 ,329, 690, 368), modal_MEC_window, 108, quitGameLValue.c_str());
     guienv->addButton(rect<s32>(331 ,385, 690, 426), modal_MEC_window, 109, cancelLValue.c_str());

//add fadein to black
escFadeToBlack->fadeOut(400);


//show cursor | also check if in menu already or in game
device->getCursorControl()->setVisible(true);
if (isInGame == 1){camera->setInputReceiverEnabled(false);} else {}



escWait = 0;
printf("escape was pressed\n");
}else{
      printf("waiting for user input\n");
      }
}



////////////////////////////////////////////////////////////////////////////////



////////////////////////////// event receiver //////////////////////////////////


class MyEventReceiver : public IEventReceiver
{
public:
   virtual bool OnEvent(const SEvent &event)
   {

		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
		}





      //mouse events
       if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
      {
         switch (event.MouseInput.Event)
         {

          
          case irr::EMIE_LMOUSE_PRESSED_DOWN:


          //click LMB makes a sound in main menu
          if (isInGame == 0)
          {
           if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, sfxClick, false, &channelSfxAudioNoLoop); ERRCHECK(result); }else{}

          }else{}


           //shoot the weapon
             if (isInGame == 1 && binGUIisOn == 0 && isInVehicleJeeep == false)
             {
                 if(pistolIsVisible == 1)
                 {   
					    //if gun has bullets in it
						if(ammoStatsValue>=1)
						{
							if (animationEnded == 1 )
							{
                  //create projectile

                ISceneManager* smgr = device->getSceneManager();
  	            IVideoDriver* driver = device->getVideoDriver();


			// Perform a raycast straight out from the camera to detect the nearest object to the cam
			core::line3df line2;
			line2.start = camera->getPosition();
			line2.end = line2.start + (camera->getTarget() - line2.start).normalize() * 1000.0f;
			
			
			IPhysxObject* objectHit = physxManager->raycastClosestObject(line2, &intersection2, &normal2, false);
			


			if (objectHit == playa->PhysxObject) 
			{
				printf("[WARNING] Camera was hit. Low FPS!\n");
			}else{
			
			  for (u32 i = 0 ; i < objects.size() ; ++i)
			  {
				if (objectHit == objects[i]->PhysxObject && objects[i]->SceneNode->getID() == 25)				
				{					
					printf("[INFO] Barrel is hit\n");
					vector3df barrelNodePosition = barrelNodePair->SceneNode->getPosition();
					barrelNodePair->PhysxObject->setPosition(core::vector3df(barrelNodePosition.X, barrelNodePosition.Y+50, barrelNodePosition.Z));										
					barrelNodePair->SceneNode->setVisible(false);

					physxManager->removePhysxObject(objects[i]->PhysxObject);
					objects[i]->SceneNode->remove();
					delete objects[i];
					objects.erase(i);
					--i;
						

					//measure the distance to the barrel, and create sounds accordinglly
					irr::f32 distanceToExplosive = intersection2.getDistanceFrom(camera->getPosition()); 					
					if (distanceToExplosive <= 600)
					{
						if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, sfxNearExplosion, false, &channelSfxAudioNoLoop); ERRCHECK(result); }else{}
						startToShakeCamera = 1;
						
					} else {
							if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, sfxDistantExplosion, false, &channelSfxAudioNoLoop); ERRCHECK(result); }else{}
						   }

					//measure the distance to the barrel, and see if it hurts the player
					//the close he is, the more damage he gets
                    if (distanceToExplosive <= 150)
					{ lifeBarValue = lifeBarValue - (15.0f - distanceToExplosive/10); }else{}
					

					createSFXExplosion(10, intersection2);
					
				}else{}
			  }

			}
            //create debrie special effect

			//objects.push_back(createBulletFromCamera(physxManager, smgr, driver, camera, 2.0f, 9900000.0f));
			createSFXImpactDebrie();

        
/*
        //create decal
        scene::ISceneNode* bullHole;
        bullHole = smgr->addBillboardSceneNode(0, core::dimension2d<f32>(0.16f, 0.16f));
        bullHole->setMaterialFlag(video::EMF_LIGHTING, true);
        bullHole->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
        bullHole->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
        bullHole->setMaterialTexture(0, driver->getTexture("../data/shaders/bullethole_s.png"));
        bullHole->setPosition(intersection);
        //delete the decal after 30 secods
        ISceneNodeAnimator *fadeTheBullHole = smgr->createDeleteAnimator(30000);
        bullHole->addAnimator(fadeTheBullHole);

*/
							
				
				}





                      //create recoil
					  core::vector3df cameraTarget = camera->getTarget();
                      camera->setTarget(core::vector3df(cameraTarget.X, cameraTarget.Y+rand()%10, cameraTarget.Z));


                      //create animation
                      if (animationEnded == 1)
                      {
					  if (escWait == 1){
                      animationEnded = 0;
                      pistolNode->setAnimationSpeed(40.0f);
                      pistolNode->setFrameLoop(1,10);      //fire weapon
					  pistolNode->setLoopMode(false);
					  if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, sfxPistolShoot, false, &channelSfxAudioNoLoop); ERRCHECK(result); }else{}
					  //deduct one bullet
					  core::stringw ammoStatsValueFormated = L"";
					  ammoStatsValue = ammoStatsValue - 1;
                      ammoStatsValueFormated += ammoStatsValue;
                      ammoStatsText->setText(ammoStatsValueFormated.c_str());						  
						  }else{}
                      }
					  }else{ammoStatsValue=0; if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, sfxPistolEmpty, false, &channelSfxAudioNoLoop); ERRCHECK(result); }else{}}
                 }
             }
            //return true;
               default:
      break;
         }
      }






























//////////////////////////////////////////////////////////////////////////////


   if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
   {
         switch(event.KeyInput.Key)
         {
         case KEY_ESCAPE:
            {

              //hide singleplayer page if it's open
              if (singleplayer_window_on == 1)
              {
                singleplayer_window_on = 0;
                singleplayer_window->setVisible(false);
               }else{
               //modal window
               if(escWait == 0){escWait = 0;} else {escWait = 1;}
               menu_exit_cancel();

              }
           }
            return true;


            case KEY_F12:
            {
              if (isInGame == 0)
                 {
                  //console can be used only in game
                 }else{
                       if(escWait == 0){escWait = 0;} else {escWait = 1;}
                       modal_CON_window->setVisible(true);
					   consoleIsOn = 1;
					   switchToSmallFont();
                       camera->setInputReceiverEnabled(false);
                       device->getCursorControl()->setVisible(true);

                      }
            }
             return true;


        //create screenshot
        case irr::KEY_F9:
        {
        //get video::IImage from the last rendered frame
        video::IImage* image = device->getVideoDriver()->createScreenShot();

        irr::io::IXMLReader* photoIDxml  = device->getFileSystem()->createXMLReader("../album/cnt.xml");

        while(photoIDxml && photoIDxml->read())
        {
          if (core::stringw("screenshot") == photoIDxml->getNodeName())
          {
          lastPhotoID  = photoIDxml->getAttributeValueAsInt(L"lastID");
          }
        }
          printf("[INFO] Screenshot: %u\n", lastPhotoID);
        photoIDxml->drop();

        //construct a filename, consisting of local time and file extension
        irr::c8 filename[32];
        snprintf(filename, 32, "../album/pic_%u.jpg", lastPhotoID);
        //write screenshot to file
        device->getVideoDriver()->writeImageToFile(image, filename);
        screenShot_fader->fadeIn(300);

        ofstream photoIDxmlW;
        photoIDxmlW.open ("../album/cnt.xml");
        photoIDxmlW << "<?xml version=\"1.0\"?>\n\n" << "<screenshot lastID=\"" << lastPhotoID+1 << "\" />";
        photoIDxmlW.close();
        }
         return true;



		case irr::KEY_KEY_C:
		{
		// Throw a sphere from the camera
		ISceneManager* smgr = device->getSceneManager();
		video::IVideoDriver* driver = device->getVideoDriver();
		objects.push_back(createSphereFromCamera(physxManager, smgr, driver, camera, 10, 40000)); 
		
		}
		return true;

        //show/hide binoculars
        case irr::KEY_KEY_B:
        {
			if (isInGame == 1 && escWait == 1 && consoleIsOn == 0 && isInVehicleJeeep == false)
         {
               if (binGUIisOn == 1)
                {
                 camera->setFOV(PI / 2.5f);
                 binGUIisOn = 0;
                }

             else
                  {
                  camera->setFOV(PI / 15.0f);
                  binGUIisOn = 1;
                  }
         }else{}
        }

         return true;

		//do an action
		case irr::KEY_KEY_E:
		{
		
		if(isInGame == 1 && consoleIsOn == 0)
		{
		ISceneManager* smgr = device->getSceneManager();

        //first see what is in front of the player
        nodeInFrontOfPlayer = smgr->getSceneCollisionManager()->getSceneNodeFromCameraBB(camera);
		irr::f32 distanceToItem = nodeInFrontOfPlayer->getPosition().getDistanceFrom(camera->getPosition());
		//vector3df vehicleJeepPosV = vehicleJeep->SceneNode->getPosition();

/*
				if (isInVehicleJeeep == true) //if the player is in vehicle and he wants out
				{
				playa->PhysxObject->setPosition(core::vector3df(vehicleJeepPosV.X+5, vehicleJeepPosV.Y+20, vehicleJeepPosV.Z+5));
				camera->setTarget(core::vector3df( vehicleJeep->SceneNode->getPosition()));
				pistolIsVisible = 0; pistolNode->setVisible(false); //TODO 0							
				isInVehicleJeeep = false;
				printf("[INFO] Player has exit a vehicle.\n");
				}else{}
*/


				if (nodeInFrontOfPlayer->getID() == 10 && distanceToItem <= 100) // ammo
				{
					nodeInFrontOfPlayer->setVisible(false);
					smgr->addToDeletionQueue(nodeInFrontOfPlayer);
					ammoStatsValue = ammoStatsValue + 25;

				}


				if (nodeInFrontOfPlayer->getID() == 11 && distanceToItem <= 100) // health
				{
					nodeInFrontOfPlayer->setVisible(false);
					smgr->addToDeletionQueue(nodeInFrontOfPlayer);
					lifeBarValue = 100;

				}


/*
                if (nodeInFrontOfPlayer->getID() == 50 && distanceToItem <= 150) // vehicle
				{		 							
					nodeInFrontOfPlayerPos = nodeInFrontOfPlayer->getPosition();
					if (isInVehicleJeeep == false)//enter vehicle
					{
					//move the playa somewhere far
					playa->PhysxObject->setPosition(core::vector3df(0, 50000, 0));
					//place the camera inside the car
					core::vector3df nodeInFrontOfPlayerPos = nodeInFrontOfPlayer->getAbsolutePosition();
					camera->setPosition(core::vector3df(nodeInFrontOfPlayerPos.X, nodeInFrontOfPlayerPos.Y+100, nodeInFrontOfPlayerPos.Z));
					camera->setTarget(core::vector3df(nodeInFrontOfPlayer->getRotation()));
					pistolIsVisible = 0; pistolNode->setVisible(false); //TODO 0					
					printf("[INFO] Player entered a vehicle.\n");
					
					isInVehicleJeeep = true;
					} else { }
					
				}
				*/	
		}else{printf("e\n");}
				



		}
		return true;



        //show/hide pistol
        case irr::KEY_KEY_1:
        {
         if (isInGame == 1 && isInVehicleJeeep == false)
         {
               if (pistolIsVisible == 1)
                {
                 pistolNode->setVisible(false);
                 printf("pistol is not visible\n");
                 pistolIsVisible = 0;
                }

             else
                  {
                  pistolNode->setVisible(true);
                  printf("pistol is visible\n");
                  pistolIsVisible = 1;
                  }
         }else{}
        }

         return true;


        //reload weapon
        case irr::KEY_KEY_R:
        {
         if (isInGame == 1 && binGUIisOn == 0)
             {
                 if(pistolIsVisible == 1)
                 {
                    if (animationEnded == 1)
                    {
						if (escWait == 1)
						{
							if(ammoStatsValue>=1)
							{
                    animationEnded = 0;
                    pistolNode->setAnimationSpeed(30.0f);
                    pistolNode->setFrameLoop(101,200);					
                    pistolNode->setLoopMode(false);
					if(sfxOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, sfxPistolReload, false, &channelSfxAudioNoLoop); ERRCHECK(result); }else{}
							}else{}
					    }else{}
                    }
                }
             }
        }
         return true;


         default:
            break;


         }
   }





                if (event.EventType == EET_GUI_EVENT)
                {
                        s32 id = event.GUIEvent.Caller->getID();
                        IGUIEnvironment* guienv = device->getGUIEnvironment();
						video::IVideoDriver* driver = device->getVideoDriver();

                        switch(event.GUIEvent.EventType)
                        {


						case EGET_LISTBOX_CHANGED:
							{
							mapName = maps_list->getListItem(maps_list->getSelected());
							
							core::stringc newLevelPreviewValue = "../data/maps/";
							newLevelPreviewValue+= mapName;
							newLevelPreviewValue+= "/levelPreview.jpg";
							
							levelPreview = driver->getTexture(newLevelPreviewValue);
							
                            guienv->addImage(levelPreview, core::position2d<s32>(339, 115), false, singleplayer_window);

							//get the briefing text
							core::stringc newLevelBriefingValue = "../data/maps/";
							newLevelBriefingValue+= mapName;
							newLevelBriefingValue+= "/mapCfg.xml";

							irr::io::IXMLReader* xmlBriefing = device->getFileSystem()->createXMLReader(newLevelBriefingValue.c_str());

							if (xmlBriefing==0)
							{  }else{  }
							while(xmlBriefing && xmlBriefing->read())
								{ //get the last profile used and apply it
									if (core::stringw("map") == xmlBriefing->getNodeName())
									{
									mapBriefing = xmlBriefing->getAttributeValue(L"briefing");
									briefingBox->setText(mapBriefing.c_str());
									}
								}
							
							}

						break;




                        case EGET_BUTTON_CLICKED:
                                switch(id)
                                {



                                        //show level select
                                        case 101:
                                        {
                                            singleplayer_window->setVisible(true);
                                            IGUIElement *windowParent = singleplayer_window->getParent();
                                            windowParent->bringToFront(singleplayer_window);
                                        }
                                        return true;



                                        //show credits window
                                        case 102:
                                        {
                                          //create a window
                                          credits_window = guienv->addWindow(rect<s32>(0, 0, screenResX, screenResY), true, L"");
                                          guienv->addImage(creditsBack, core::position2d<s32>(0, 0), false, credits_window);
										  guienv->addButton(rect<s32>(811, 722, 990, 757), credits_window, 106, L"CLOSE");
                                        }
                                        return true;


                                        // are you sure you want to quit?
                                        case 103:
                                        {
                                                //hide singleplayer page if it's open
                                            if (singleplayer_window_on == 1)
                                            {
                                                singleplayer_window_on = 0;
                                                singleplayer_window->setVisible(false);
                                            }else{
                                                //modal window
                                                if(escWait == 0){escWait = 0;} else {escWait = 1;}
                                                menu_exit_cancel();
                                                  }
                                        }
                                        return true;



                                        //quit game
                                        case 108:
                                        {
                                        clearAllScene();
                                        device->closeDevice();
                                        }
                                        return true;



                                        //load a map
                                        case 104:
                                        {

                                          singleplayer_window->setVisible(false);
                                          startGameBtn->setVisible(false);
										  optionsBtn->setVisible(false);	
                                          creditsBtn->setVisible(false);
                                          quitGameBtn->setVisible(false);

                                          mapName = maps_list->getListItem(maps_list->getSelected());
                                          singleplayer_window_on = 0;
                                          isPleaseWaitImg=1;

                                        }
                                        return true;



                                        //'load map' "Cancel" button (hides the 'load map' window)
                                        case 105:
                                        {
                                          singleplayer_window->setVisible(false);
                                        }
                                        return true;

										//'credits' "Close" button (closes the 'credits' window)
                                        case 106:
                                        {
                                          credits_window->remove();
                                        }
                                        return true;



                                        //go to the main menu
                                        case 107:
                                        {
                                        modal_MEC_window->remove();
                                        if(modal_CON_window){modal_CON_window->remove();}else{}
                                        escWait = 1;

                                        escFadeToBlack->fadeIn(300);
                                        clearAllScene();
                                        loadMainMenu();

                                        //if it was in the game before calling the MEC window, hide the cursor
                                        if (isInGame == 1){device->getCursorControl()->setVisible(false);} else{}

                                        }
                                        return true;



                                        //close the MEC window
                                        case 109:
                                        {
                                        modal_MEC_window->remove();
                                        escFadeToBlack->fadeIn(300);
                                        escWait = 1;

										switchToBigFont();

                                        //if it was in the game before calling the MEC window, hide the cursor
                                        if (isInGame == 1)
                                        {
                                        device->getCursorControl()->setVisible(false);

                                            camera->setInputReceiverEnabled(true);

                                        } else {}

                                        }
                                        return true;


										//show the options window
                                        case 111:
                                        {

                                          IGUIElement *windowParent = options_window->getParent();
                                          windowParent->bringToFront(options_window);
                                          options_window->setVisible(true);

                                        }
                                        return true;



										//save and then close the options window
                                        case 112:
                                        {	
											IGUIElement *wFullscreenIn = guienv->getRootGUIElement()->getElementFromId(301, true);
											int iswFullscreenIn = ((IGUICheckBox*)wFullscreenIn)->isChecked();

											IGUIElement *musicOnIn = guienv->getRootGUIElement()->getElementFromId(302, true);
											int ismusicOnIn = ((IGUICheckBox*)musicOnIn)->isChecked();

											IGUIElement *sfxOnIn = guienv->getRootGUIElement()->getElementFromId(303, true);
											int issfxOnIn = ((IGUICheckBox*)sfxOnIn)->isChecked();

											IGUIElement *skipIntroIn = guienv->getRootGUIElement()->getElementFromId(304, true);
											int isskipIntroIn = ((IGUICheckBox*)skipIntroIn)->isChecked();

											IGUIElement *bloomIn = guienv->getRootGUIElement()->getElementFromId(305, true);
											int isBloomIn = ((IGUICheckBox*)bloomIn)->isChecked();

										    ofstream myOptions;
											myOptions.open("../data/logic/config/config.xml");
											myOptions << "<?xml version= \"1.0\"?>\n";
											myOptions << "<video screenResX=\"";
											myOptions << screenResX;
											myOptions << "\" screenResY=\"";
											myOptions << screenResY;
											myOptions << "\" wfullscreen=\"";
											myOptions << iswFullscreenIn;
											myOptions << "\" />\n";
											myOptions << "<detail ";
											myOptions << "mapDetail=\"1\" shadows=\"0\" bloom=\"";
											myOptions << isBloomIn;
											myOptions << "\" normalMapping=\"0\" grass=\"1\" farView=\"200000\" />\n";
											myOptions << "<audio musicOn=\"";
											myOptions << ismusicOnIn;
											myOptions << "\" sfxOn=\"";
											myOptions << issfxOnIn;
											myOptions << "\" />\n";
											myOptions << "<debug skipIntro=\"";
											myOptions << isskipIntroIn;
											myOptions << "\" />\n";

											myOptions.close();

                                          options_window->setVisible(false);

                                        }
                                        return true;


										//the submit button of the in-game console
										case 201:
										{
											ISceneManager* smgr = device->getSceneManager();
											video::IVideoDriver* driver = device->getVideoDriver();
											stringw consoleInputValue = consoleInput->getText();
											if (consoleInputValue == "help"){printf("[INFO] Need Help? Read the documentation.\n"); guienv->addMessageBox(L"AVAILABLE COMMANDS",L"help, bloomon, bloomoff, giveammo, givehealth, debug, gravityoff, gravityon, slowmoon, slowmooff", true, EMBF_OK, 0, 648);}
											if (consoleInputValue == "bloomon"){printf("[INFO] Bloom changed to ON.\n"); bloom=1; }
											if (consoleInputValue == "bloomoff"){printf("[INFO] Bloom changed to OFF.\n"); bloom=0; }
											if (consoleInputValue == "giveammo"){printf("[INFO] Here are 50 bullets.\n"); ammoStatsValue=50; }
											if (consoleInputValue == "givehealth"){printf("[INFO] Here is 100 health.\n"); lifeBarValue=100; }
											if (consoleInputValue == "debug"){printf("[INFO] Toggle debug info.\n"); physxManager->setDebugDataVisible(!physxManager->isDebugDataVisible()); }
											if (consoleInputValue == "gravityoff"){printf("[INFO] No gravity.\n"); physXGravityVal = 0.0f; }
											if (consoleInputValue == "gravityon"){printf("[INFO] Gravity enabled.\n"); physXGravityVal = -98.0f; }
											if (consoleInputValue == "slowmoon"){printf("[INFO] Slow motion enabled.\n"); physxMotion = 5000.0f; }
											if (consoleInputValue == "slowmooff"){printf("[INFO] Slow motion disabled.\n"); physxMotion = 500.0f; }
											consoleInput->setText(L"");

										}
										return true;


                                        //close the console window
                                        case 202:
                                        {
                                        modal_CON_window->setVisible(false);
										switchToBigFont();
										consoleIsOn = 0;
										fpsValOn = 0;
                                        escWait = 1;

                                        //if it was in the game before calling the MEC window, hide the cursor
                                        if (isInGame == 1){device->getCursorControl()->setVisible(false);
                                        camera->setInputReceiverEnabled(true);
                                        } else{}

                                        }
                                        return true;



                                }


                            default:
                                        return false;
                                                break;


                          }

                }







   return false;
   }





	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}



   private:
	bool KeyIsDown[KEY_KEY_CODES_COUNT];

};



int main()
{
    //First, create a NULL Irrlicht device, read the configuration file,
    //then create a Irrlicht device using the retrieved variables


   IrrlichtDevice* deviceNULL = createDevice(video::EDT_NULL);
   video::E_DRIVER_TYPE driverType;
   driverType = video::EDT_OPENGL;

   //read the profile file to find out the last used profile
   irr::io::IXMLReader* xmlProfile = deviceNULL->getFileSystem()->createXMLReader("../data/logic/profiles/profileIndex.xml");
   if (xmlProfile==0)
          {  }else{   }
	while(xmlProfile && xmlProfile->read())
	{ //get the last profile used and apply it
      if (core::stringw("profiles") == xmlProfile->getNodeName())
      {
      lastUsedProfile = xmlProfile->getAttributeValue(L"lastUsed");
	  }
	}

   //read the last used profile and see what language it's using
	core::stringc lastProfileURI = "../data/logic/profiles/";
        lastProfileURI+= lastUsedProfile;
		lastProfileURI+= "/profile.xml";
   irr::io::IXMLReader* xmlLastProfile = deviceNULL->getFileSystem()->createXMLReader(lastProfileURI.c_str());
   if (xmlProfile==0)
          {  }else{   }
	while(xmlLastProfile && xmlLastProfile->read())
	{ //get the last profile used and apply it
      if (core::stringw("detail") == xmlLastProfile->getNodeName())
      {
      selectedProfileLanguage = xmlLastProfile->getAttributeValue(L"profileLanguage");
	  }
	}

   //now open and load the specific language translations
	core::stringc languageURI = "../data/logic/languages/";
        languageURI+= selectedProfileLanguage;
		languageURI+= "/0000.xml";
   irr::io::IXMLReader* xmlUserLanguageDef = deviceNULL->getFileSystem()->createXMLReader(languageURI.c_str());
   if (xmlUserLanguageDef==0)
          {  }else{   }
	while(xmlUserLanguageDef && xmlUserLanguageDef->read())
	{ //get the last profile used and apply it
      if (core::stringw("mainMenu") == xmlUserLanguageDef->getNodeName())
      {
      startGameLValue = xmlUserLanguageDef->getAttributeValue(L"startGame");
      creditsLValue   = xmlUserLanguageDef->getAttributeValue(L"credits");
	  optionsLValue	  = xmlUserLanguageDef->getAttributeValue(L"options");
   saveAndCloseLValue = xmlUserLanguageDef->getAttributeValue(L"saveAndClose");
      quitGameLValue  = xmlUserLanguageDef->getAttributeValue(L"quitGame");
      loadMapLValue   = xmlUserLanguageDef->getAttributeValue(L"loadMap");
      cancelLValue    = xmlUserLanguageDef->getAttributeValue(L"cancel");
	  mainMenuLValue  = xmlUserLanguageDef->getAttributeValue(L"mainMenu");
	  }

	  if (core::stringw("inGame") == xmlUserLanguageDef->getNodeName())
      {
      consoleLValue = xmlUserLanguageDef->getAttributeValue(L"console");
      submitLValue   = xmlUserLanguageDef->getAttributeValue(L"submit");
      closeLValue  = xmlUserLanguageDef->getAttributeValue(L"close");
	  }
	}


	//read the game version file to display on main menu right corner
   irr::io::IXMLReader* xmlVersion = deviceNULL->getFileSystem()->createXMLReader("../updates/version.xml");
   if (xmlVersion==0)
          {  }else{   }
	while(xmlVersion && xmlVersion->read())
	{ 
      if (core::stringw("version") == xmlVersion->getNodeName())
      {
      releaseValue = xmlVersion->getAttributeValue(L"release");
	  buildValue   = xmlVersion->getAttributeValue(L"build");
	  }
	}


   //read the configuration file
   irr::io::IXMLReader* xml  = deviceNULL->getFileSystem()->createXMLReader("../data/logic/config/config.xml");
   if (xml==0)
          {  }else{  }

   while(xml && xml->read())
   {  //get video Information
      if (core::stringw("video") == xml->getNodeName())
      {
      screenResX  = xml->getAttributeValueAsInt(L"screenResX");
      screenResY  = xml->getAttributeValueAsInt(L"screenResY");
      wfullscreen = xml->getAttributeValueAsInt(L"wfullscreen");
	  if (wfullscreen==0){ wfullscreenBool = false; } else { wfullscreenBool = true; }
      }

      //get detail Information
      if (core::stringw("detail") == xml->getNodeName())
      {
      mapDetail      = xml->getAttributeValueAsInt(L"mapDetail");
      shadows        = xml->getAttributeValueAsInt(L"shadows");
      bloom          = xml->getAttributeValueAsInt(L"bloom");
      normalMapping  = xml->getAttributeValueAsInt(L"normalMapping");
	  grass          = xml->getAttributeValueAsInt(L"grass");
      farView        = xml->getAttributeValueAsInt(L"farView");
      }

      //get audio Information
      if (core::stringw("audio") == xml->getNodeName())
      {
      musicOn = xml->getAttributeValueAsInt(L"musicOn");
      sfxOn   = xml->getAttributeValueAsInt(L"sfxOn");
      musicVol= xml->getAttributeValueAsInt(L"musicVol");
      sfxVol  = xml->getAttributeValueAsInt(L"sfxVol");
      }

      //get debug Information
      if (core::stringw("debug") == xml->getNodeName())
      {
      skipIntro     = xml->getAttributeValueAsInt(L"skipIntro");
      }

      //get HUD Information
      if (core::stringw("gui") == xml->getNodeName())
      {
      reticule     = xml->getAttributeValueAsInt(L"reticule");

      }

   }


    deviceNULL->closeDevice();


	// Initialise Irrlicht device.



MyEventReceiver eventReceiver;

irr::SIrrlichtCreationParameters device_settings;
device_settings.DriverType = video::EDT_DIRECT3D9;//EDT_OPENGL
device_settings.Bits = 32;
device_settings.AntiAlias= true;
device_settings.Stencilbuffer= true;
device_settings.Fullscreen = wfullscreenBool;
device_settings.Vsync = true;
device_settings.WindowSize = core::dimension2d<s32>(screenResX, screenResY);
device_settings.EventReceiver = &eventReceiver;
device = createDeviceEx(device_settings);

    //load main menu

    video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	// Set the window's title
	//wchar_t wstr[50];
	//swprintf(wstr, 50, L"IrrPhysx(%.2f) Aranea", IRRPHYSX_VERSION);
	device->setWindowCaption(L"Supremacy Errands");


	PostProcessing *PP_Test;
	if (bloom==1)
    {
	PP_Test = new PostProcessing(smgr,"../data/shaders/logic/PP_GL_Bloom1.fx","../data/shaders/logic/PP_DX_Bloom1.fx",video::EPST_PS_1_4,1024,768);
    PP_Test->setShaderParameters(0.5f);//was 1.0
    PostProcessing *Test2 = PP_Test->addMaterial("../data/shaders/logic/PP_GL_Bloom2.fx","../data/shaders/logic/PP_DX_Bloom2.fx",video::EPST_PS_2_0,1024,1024);
    Test2->setShaderParameters(0.005f);
    Test2 = Test2->addMaterial("../data/shaders/logic/PP_GL_Bloom3.fx","../data/shaders/logic/PP_DX_Bloom3.fx",video::EPST_PS_2_0,1024,1024);
    Test2->setShaderParameters(0.005f);
    Test2 = Test2->addMaterial("../data/shaders/logic/PP_GL_Bloom4.fx","../data/shaders/logic/PP_DX_Bloom4.fx",video::EPST_PS_2_0,1024,1024);
    Test2->setShaderParameters(0.3f);
    Test2->setSecondMap(PP_Test->getFirstMap(),video::ETC_CLAMP);
	
	printf("[INFO] Shaders processed.\n");

    }


    //first show company info

    showCompanyInfo = 1;

    loadAudioFiles();
    loadMainMenu();

	//load IrrPhysX
	physxManager = createPhysxManager(device, sceneDesc);
	


    int lastFPS = -1;
    s32 lastTime66 = device->getTimer()->getTime();


////////////////////////////////////////////////////////////////////////////////



 	while(device->run())
	{

			s32 timeNow66 = device->getTimer()->getTime();
			s32 elapsedTime66 = timeNow66 - lastTime66;

			lastTime66 = timeNow66;
			// Set the physics simulation going
			physxManager->setGravity(core::vector3df(0, physXGravityVal, 0));
			physxManager->simulate(elapsedTime66/physxMotion);

			

		if (isInGame == 1 || fpsValOn == 1)
		{
		core::stringw str(L"FPS: ");
		str.append(core::stringw(driver->getFPS()));
		fpsVal->setText(str.c_str());		
		} else{	}


            physxManager->fetchResults();

			// Update our objects to the correct transformations
			for (u32 i = 0 ; i < objects.size() ; i++) 
				objects[i]->updateTransformation();



        driver->beginScene(true, true, video::SColor(0, 0,0,0));
		

            if (bloom==1)
            {
                driver->setRenderTarget(PP_Test->getFirstMap(),	true, true, video::SColor(255,255,255,255));
            }else{}


        //remove any particles (affectors) after X seconds

        if(isInGame==1 && startStoppingExplosionAnimators == true)
        {
                u32 timeNow5 = device->getTimer()->getTime();
                if ((timeThen5 + 1000) < timeNow5)
                {
					timeElapsedToDestroyEmiter = timeElapsedToDestroyEmiter + 1;
					
					if (timeElapsedToDestroyEmiter == 5) {if(burnSmallEffect)burnSmallEffect->setEmitter(0);}
					if (timeElapsedToDestroyEmiter == 15){if(smokeSmallEffect)smokeSmallEffect->setEmitter(0);}
                    if (timeElapsedToDestroyEmiter == 16){timeElapsedToDestroyEmiter = 0; startStoppingExplosionAnimators = false; }
					if (timeElapsedToDestroyEmiter >= 17){timeElapsedToDestroyEmiter = 0;}
					
                 timeThen5 = timeNow5;
                }
        }


		if (isInGame == 1 && isInVehicleJeeep == true) //if it's inside a vehicle
		{
			
			vehicleReceiver();

		}





//----------------------------------------------------------------------------------------------

		//move character / vehicle etc.
		if(eventReceiver.IsKeyDown(irr::KEY_KEY_W))
		{
			if (isInGame == 1 && isInVehicleJeeep == true) //it's in a vehicle
			{

				//core::vector3df vell = camera->getTarget() - camera->getPosition();
				//vell.normalize();

				directionVec = 1;
				torqueVal = torqueVal + 1.0f;
				if (torqueVal >= 300){ torqueVal = 300; }

				//vell*=torqueVal;
				//vehicleJeep->PhysxObject->setLinearVelocity(vell);
				
			}else{ //it's not in a vehicle
			
				core::vector3df vell = camera->getTarget() - camera->getPosition();
				vell.normalize();
				vell*=200.0f;
				playa->PhysxObject->setLinearVelocity(vell);				
			     }
		} else {  torqueVal = torqueVal - 0.5f; if (torqueVal <= 0){ torqueVal = 0; }}


		//move character / vehicle etc.
		if(eventReceiver.IsKeyDown(irr::KEY_KEY_S))
		{
			if (isInGame == 1 && isInVehicleJeeep == true) //it's in a vehicle
			{
				
				//core::vector3df vell = camera->getTarget() - camera->getPosition();
				//vell.normalize();

				directionVec = +1;
				torqueVal = torqueVal - 1.0f;
				if (torqueVal <= 0){ torqueVal = 0; }

				//vell*=torqueVal;
				//vehicleJeep->PhysxObject->setLinearVelocity(-vell);

			}else{ //it's not in a vehicle
			
				core::vector3df vell = camera->getTarget() - camera->getPosition();
				vell.normalize();
				vell*=180.0f;
				playa->PhysxObject->setLinearVelocity(-vell);
			     }
	    } else{}


		//move character / vehicle etc.
		if(eventReceiver.IsKeyDown(irr::KEY_KEY_A))
		{
			if (isInGame == 1 && isInVehicleJeeep == true) //it's in a vehicle
			{
				
				core::vector3df vell;
				if (torqueVal > 5)
				{
				jeepVehicleRotation = jeepVehicleRotation - 1.0f;
				if (jeepVehicleRotation >= 360){ jeepVehicleRotation = 0; }		
				}


			}else{ //it's not in a vehicle
			

				core::vector3df vell = camera->getTarget() - camera->getPosition();
			    vell.rotateXZBy(90);
				vell.normalize();
				vell*=180.0f;
				playa->PhysxObject->setLinearVelocity(vell);
			     }
		} else{}

		//move character / vehicle etc.
		if(eventReceiver.IsKeyDown(irr::KEY_KEY_D))
		{
			if (isInGame == 1 && isInVehicleJeeep == true) //it's in a vehicle
			{
				
				core::vector3df vell;
				if (torqueVal > 5)
				{
				jeepVehicleRotation = jeepVehicleRotation + 1.0f;
				if (jeepVehicleRotation >= 360){ jeepVehicleRotation = 0; }	
				}


			}else{ //it's not in a vehicle
			

				core::vector3df vell = camera->getTarget() - camera->getPosition();
			    vell.rotateXZBy(-90);
				vell.normalize();
				vell*=180.0f;
				playa->PhysxObject->setLinearVelocity(vell);
			     }
		} else{}




//----------------------------------------------------------------------------------------------







		smgr->drawAll();

		physxManager->renderDebugData(video::SColor(225,255,255,255));


		if (bloom==1)
        {
		 driver->setRenderTarget(0);
		 PP_Test->renderEffect();
        }

		if (isBackImg==1)      {driver->draw2DImage(mainMenuGUI,core::rect<s32>(0,0,screenResX,screenResY), core::rect<s32>(0,0,1024,768), 0,0,false);}
		if (isPleaseWaitImg==1){driver->draw2DImage(pleaseWaitImg,core::rect<s32>(0,0,screenResX,screenResY), core::rect<s32>(0,0,1024,768), 0,0,false);}

		if (isInGame==1)
		{	//show the life bar	

			//debug only
			 core::stringw ddddddddddddd = L"";					  
             ddddddddddddd += torqueVal;
             torqueValueText->setText(ddddddddddddd.c_str());

			if(lifeBarValue <= 15)
				{
				 driver->draw2DImage(lifeBarLow,  core::rect<s32>(27, 678,151 ,749), core::rect<s32>(0,0,124,70), 0,0,true);
				}else{
				       driver->draw2DImage(lifeBar,  core::rect<s32>(27, 678,151 ,749), core::rect<s32>(0,0,124,70), 0,0,true);
			         }
			//show the gun graphic and bullets value
			if(pistolIsVisible==1)
			{	
				if(ammoStatsValue <= 9)
				{
				  driver->draw2DImage(ammoStatsLow,core::rect<s32>(870,678,1000,749), core::rect<s32>(0,0,124,70), 0,0,true);
				}else{
					   driver->draw2DImage(ammoStats,core::rect<s32>(870,678,1000,749), core::rect<s32>(0,0,124,70), 0,0,true);
					 }

			 core::stringw lifeBarValueFormated = L"";					  
             lifeBarValueFormated += int(lifeBarValue);
             lifeBarText->setText(lifeBarValueFormated.c_str());

			 core::stringw ammoStatsValueFormated = L"";					  
             ammoStatsValueFormated += int(ammoStatsValue);
             ammoStatsText->setText(ammoStatsValueFormated.c_str());
			}else{
				  ammoStatsText->setText(L"");
				 }
		}


		if (binGUIisOn==1)     {driver->draw2DImage(binGUI,core::rect<s32>(0,0,screenResX,screenResY), core::rect<s32>(0,0,1024,768), 0,0,true);}


	if(isInGame==0)
	{

    // draw animated title
	u32 timeNow2 = device->getTimer()->getTime();

	if(drawNextSlide==1){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp1, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==2){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp2, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==3){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp3, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==4){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp4, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==5){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp5, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==6){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp6, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==7){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp7, 0, video::SColor(255,255,255,255), true);}
	if(drawNextSlide==8){driver->draw2DImage(title_animate, core::position2d<s32>(349,143), imp8, 0, video::SColor(255,255,255,255), true);}

               if ((timeThen2 + 150) < timeNow2)
                {
                  drawNextSlide = drawNextSlide + 1;
				  if (drawNextSlide>=8){drawNextSlide = 1;}else{}
                  timeThen2 = timeNow2;
                }
	}

        if(showCompanyInfo==1)//display the company and game info for a few seconds
        {
            if(skipIntro==0){

         driver->draw2DImage(companyInfo,core::rect<s32>(0,0,screenResX,screenResY), core::rect<s32>(0,0,1024,768), 0,0,true);
            startGameBtn->setVisible(false);
            creditsBtn->setVisible(false);
			optionsBtn->setVisible(false);
            quitGameBtn->setVisible(false);

          u32 timeNow1 = device->getTimer()->getTime();
                if ((timeThen1 + 5000) < timeNow1)
                {
                  showCompanyInfo = 0;
                  startGameBtn->setVisible(true); optionsBtn->setVisible(true); creditsBtn->setVisible(true); quitGameBtn->setVisible(true);
                  timeThen1 = timeNow1;
                }

            }else{printf("[INFO] Intro skipped.\n"); showCompanyInfo = 0;}
        }


		

		guienv->drawAll();
    	driver->endScene();


        if(isInGame==1 && startToShakeCamera==1){shakeCameraExplosion(); }else{}





		if(isInGame==1 && isInVehicleJeeep == false) { //attach the cameraSceneNode to the physX object, if it's not in the vehicle
			core::vector3df playaPosition = playa->SceneNode->getPosition();
			playa->PhysxObject->setRotation(core::vector3df(0,0,0));
			camera->setPosition(core::vector3df(playaPosition.X, playaPosition.Y+35, playaPosition.Z));} else {}


		if (isPleaseWaitImg==1)
        {
        clearAllScene();
        loadMap(mapName, XMLfogValueR, XMLfogValueG, XMLfogValueB, XMLskyValue);
        isPleaseWaitImg = 0;

        }else{}



        //add fog around the player
        if(isInGame==1){
        driver->setFog(video::SColor(0, XMLfogValueR, XMLfogValueG, XMLfogValueB), true, 1.0f, farView+5000.0f, 0.0005f, false);

        }else{}
    }

    // Clean up the physx manager
	removePhysxManager(physxManager);
	physxManager = 0;


	return 0;
}
