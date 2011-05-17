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

// ----------------------- Start declaring variables ---------------------------

// Irrlicht varibles and declarations

   IrrlichtDevice			*device          = 0;                                    //The Irrlicht device
   IVideoDriver				*driver          = 0;                                    //The Irrlicht video driver
   ISceneManager			*smgr            = 0;                                    //The Irrlicht scene manager
   IGUIEnvironment			*guienv          = 0;                                    //The irrlicht gui environment


   ITexture					*backImage; 

   IGUIButton				*restoreDefaultBtn, *startGameBtn;