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


void loadMainMenu()
{

    video::IVideoDriver* driver = device->getVideoDriver();
	//ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();


    //show main menu back image
    isBackImg = 1;
    isInGame  = 0;
	fpsValOn  = 0;

    //show the cursor
    device->getCursorControl()->setVisible(true);

    screen_fader_black = guienv->addInOutFader();
    screen_fader_black->setColor(video::SColor(0,0,0,0));

    screenShot_fader = guienv->addInOutFader();
    screenShot_fader->setColor(video::SColor(0,255,255,255));

    escFadeToBlack = guienv->addInOutFader();
    escFadeToBlack->setColor(video::SColor(0,0,0,0));

    driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);


    guienv->loadGUI("../data/logic/config/main_menu_skin.xml");
	switchToSmallFont();

    //load background for main menu + please wait screen + instructions screens
    mainMenuGUI        = driver->getTexture("../data/gui/main_menu/hud/mainmenu.png");
    pleaseWaitImg      = driver->getTexture("../data/gui/main_menu/hud/pleasewait.jpg");
    companyInfo        = driver->getTexture("../data/gui/main_menu/hud/selogo.jpg");
    binGUI             = driver->getTexture("../data/gui/ingame/bin.png");
	mapSelect          = driver->getTexture("../data/gui/main_menu/hud/map_select.png");
	creditsBack        = driver->getTexture("../data/gui/main_menu/hud/credits.png");
	optionsBack        = driver->getTexture("../data/gui/main_menu/hud/options.png");
	levelPreview       = driver->getTexture("../data/gui/main_menu/hud/levelPreview.jpg");

	title_animate       = driver->getTexture("../data/gui/main_menu/hud/title_animate.png");
	loadingBar          = driver->getTexture("../data/gui/main_menu/hud/loadingBar.png");
   

    //create the single player window
    singleplayer_window = guienv->addWindow(rect<s32>(0,0,screenResX,screenResY), false, L"");
    singleplayer_window->getCloseButton()->setVisible(false);

	//first add the background
	guienv->addImage(mapSelect, core::position2d<s32>(0, 0), false, singleplayer_window);

    //add maps list
    maps_list = guienv->addListBox(rect<s32>(26,114, 307, 682), singleplayer_window, -1, true);

   irr::io::IXMLReader* xml2 = device->getFileSystem()->createXMLReader("../data/maps/map_list.xml");
    //load maps list
    while(xml2 && xml2->read())
   {
      if (core::stringw("maps") == xml2->getNodeName())
      {
      XMLmapCount  = xml2->getAttributeValueAsInt(L"count");
      }

      for( s32 j=0; j < XMLmapCount; ++j)
      {
      if (core::stringw(j) == xml2->getNodeName())
         {
         XMLmapName  = xml2->getAttributeValue(L"map_name");
         maps_list->addItem(XMLmapName.c_str());
         XMLfogValueR = xml2->getAttributeValueAsInt(L"fog_value_r");
         XMLfogValueG = xml2->getAttributeValueAsInt(L"fog_value_g");
         XMLfogValueB = xml2->getAttributeValueAsInt(L"fog_value_b");
         XMLskyValue = xml2->getAttributeValue(L"sky");
         }
      }

   }
    maps_list->setSelected(0); //automatically select first map

    guienv->addButton(rect<s32>(811, 722, 990, 757), singleplayer_window, 104, loadMapLValue.c_str());
    guienv->addButton(rect<s32>(602, 722, 781, 757), singleplayer_window, 105, cancelLValue.c_str());

    guienv->addImage(levelPreview, core::position2d<s32>(339, 115), false, singleplayer_window);

	briefingBox = guienv->addEditBox(L"", core::rect<s32>(350,424,975,680), false, singleplayer_window, 256);
	briefingBox->setMultiLine(true);
	briefingBox->setWordWrap(true);
	briefingBox->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	briefingBox->setEnabled(false);

	//done adding elements, hide this
    singleplayer_window->setVisible(false);


	//options window
	options_window = guienv->addWindow(rect<s32>(0,0,screenResX,screenResY), false, L"");
	options_window->getCloseButton()->setVisible(false);
    guienv->addImage(optionsBack, core::position2d<s32>(0, 0), false, options_window);
	guienv->addButton(rect<s32>(611, 722, 990, 757), options_window, 112, saveAndCloseLValue.c_str());


	if (wfullscreen==1){ isWFullscreen = 1; }else{ isWFullscreen = 0;}
	guienv->addCheckBox(isWFullscreen, rect<s32>(100, 150, 390, 200), options_window, 301, L"FULLSCREEN");

	if (musicOn==1){ isMusicOn = 1; }else{ isMusicOn = 0;}
	guienv->addCheckBox(isMusicOn, rect<s32>(100, 220, 390, 270), options_window, 302, L"MUSIC ON");

	if (sfxOn==1){ isSfxOn = 1; }else{ isSfxOn = 0;}
	guienv->addCheckBox(isSfxOn, rect<s32>(100, 290, 390, 340), options_window, 303, L"AUDIO SFX ON");

	if (skipIntro==1){ isSkipIntro = 1; }else{ isSkipIntro = 0;}
	guienv->addCheckBox(isSkipIntro, rect<s32>(100, 360, 390, 410), options_window, 304, L"SKIP INTRO");

	if (bloom==1){ isBloom = 1; }else{ isBloom = 0;}
	guienv->addCheckBox(isBloom, rect<s32>(400, 150, 700, 200), options_window, 305, L"BLOOM");

    options_window->setVisible(false);




   //add the start game button
   startGameBtn = guienv->addButton(rect<s32>(screenResX/3,screenResY/2-60,screenResX/3+screenResX/3,screenResY/2-10), 0, 101, startGameLValue.c_str());

   //add the options button
   optionsBtn = guienv->addButton(rect<s32>(screenResX/3,screenResY/2+0,screenResX/3+screenResX/3,screenResY/2+50), 0, 111, optionsLValue.c_str());

   //add the credits button
   creditsBtn = guienv->addButton(rect<s32>(screenResX/3,screenResY/2+60,screenResX/3+screenResX/3,screenResY/2+110), 0, 102, creditsLValue.c_str());

   //add the exit game button
   quitGameBtn = guienv->addButton(rect<s32>(screenResX/3,screenResY/2+120,screenResX/3+screenResX/3,screenResY/2+170), 0, 103, quitGameLValue.c_str());

   //add the loaded profile name
   guienv->addStaticText(lastUsedProfile.c_str(), rect<s32>(10,10, 200,80), false, false, 0, -1, false);

   //add the version value
   guienv->addStaticText(releaseValue.c_str() , rect<s32>(960,10, 980,80), false, false, 0, -1, false);
   guienv->addStaticText(L"." , rect<s32>(980,10, 990,80), false, false, 0, -1, false);
   guienv->addStaticText(buildValue.c_str(), rect<s32>(990,10, 1024,80), false, false, 0, -1, false);

   if(musicOn==1){ result = systemFMOD->playSound(FMOD_CHANNEL_FREE, mainMenuBGMusic, false, &channelMusic); ERRCHECK(result); }else{}

   screen_fader_black->fadeIn(2000);


   printf("[INFO] Main Menu loaded\n");
}
