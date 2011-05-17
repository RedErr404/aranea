#include "irrlicht.h"
#include "launcher_main.h"
#include <string>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;

#pragma comment(lib, "Irrlicht.lib")



class MyEventReceiver : public IEventReceiver
{
public:
   virtual bool OnEvent(const SEvent &event)
   {
	  if (event.EventType == EET_GUI_EVENT)
         {
			s32 id = event.GUIEvent.Caller->getID();
            IGUIEnvironment* guienv = device->getGUIEnvironment();
			video::IVideoDriver* driver = device->getVideoDriver();

			switch(event.GUIEvent.EventType)
            {

            case EGET_BUTTON_CLICKED:
            switch(id)
            {
                case 101:
                {

                }
                return true;


                 default:
                 return false;
                 break;
			}
			}
	  }
   return false;
   }


};


int main()
{


MyEventReceiver eventReceiver;

irr::SIrrlichtCreationParameters device_settings;
device_settings.DriverType = video::EDT_OPENGL;
device_settings.Bits = 32;
device_settings.AntiAlias= false;
device_settings.Stencilbuffer= false;
device_settings.Fullscreen = false;
device_settings.Vsync = false;
device_settings.WindowSize = core::dimension2d<s32>(800, 600);
device_settings.EventReceiver = &eventReceiver;
device = createDeviceEx(device_settings);


    video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	//load font
    guienv->loadGUI("../data/logic/config/main_menu_skin.xml");
    IGUISkin* game_skin = guienv->getSkin();
    IGUIFont* menu_font = guienv->getFont("../data/gui/fonts/f0.xml");
        if (menu_font)
                game_skin->setFont(menu_font);

    //load background image
	backImage = driver->getTexture("../data/gui/launcher/background.png");

	//add buttons
    restoreDefaultBtn = guienv->addButton(rect<s32>(368, 559, 570, 589), 0, 101, L"Restore Default");
	startGameBtn      = guienv->addButton(rect<s32>(582, 559, 784, 589), 0, 102, L"Launch Game");
















while(device->run())
	{

        driver->beginScene(true, true, video::SColor(0, 0,0,0));

		smgr->drawAll();

        driver->draw2DImage(backImage,core::rect<s32>(0,0,800,600), core::rect<s32>(0,0,800,600), 0,0,false);

		guienv->drawAll();
    	driver->endScene();


	}

	return 0;
}