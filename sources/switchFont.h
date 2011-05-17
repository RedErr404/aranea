#include "irrlicht.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


void switchToSmallFont()
{

    gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	IGUISkin* game_skin = guienv->getSkin();
	IGUIFont* menu_font = guienv->getFont("../data/gui/fonts/f0.xml");
	if (menu_font)
	game_skin->setFont(menu_font);


}

void switchToBigFont()
{

  gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

  IGUISkin* game_skinG = guienv->getSkin();
  IGUIFont* gameEventList_font = guienv->getFont("../data/gui/fonts/f1.xml");
        if (gameEventList_font)
                game_skinG->setFont(gameEventList_font);


}