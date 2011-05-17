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

   IrrlichtDevice          *device          = 0;                                    //The Irrlicht device
   IVideoDriver            *driver          = 0;                                    //The Irrlicht video driver
   ISceneManager           *smgr            = 0;                                    //The Irrlicht scene manager
   IGUIEnvironment         *guienv          = 0;                                    //The irrlicht gui environment
   //NewtonWorld			   *nWorld;
   IPhysxManager*			physxManager = NULL;
   core::array<SPhysxAndNodePair*>objects;
   f32 objectDensity						= 50;
   CGUIParamChanger* paramChanger			= NULL;
   SSceneDesc								sceneDesc;
   f32 physxMotion							= 500.0f;
   f32 physXGravityVal						= -98.0f;
   SPhysxAndNodePair						*pair, *barrelNodePair;
   IPhysxObject								*terrObj;
   scene::IBillboardSceneNode				*intersectionBill;
																		//camera collision group





   ICameraSceneNode        *camera;                                                 //Player camera
   SPhysxAndNodePair	   *playa;												    //Player PhysX object

   SPhysxAndNodePair       *vehicleJeep;
   const c8                *nodeNameX;                                              //This stores the special nodes info

   ISceneNode              *cubeSceneNodeN, *nodeBull, *nodeWater;
   ISceneNode              *playaIrrNode, *node2 , *node;
   ISceneNode			   *nodeInFrontOfPlayer;
   IAnimatedMeshSceneNode  *pistolNode, *player1Body;

   IParticleSystemSceneNode *debrieDustEffect;                                      //bullet debrie dust effect
   IParticleSystemSceneNode *explosionSmallEffect, *burnSmallEffect, *smokeSmallEffect;
   IAnimatedMesh            *waterMesh;                                             //this is the water mesh

   
   stringw                 mapName, resolutionName, detailLevel;

   core::stringc		   newHeightmapName, newTerrainClName, newGrassmapName;


   s32                     node_ID, mapIDToBeCalled;                                //node ID , TODO: remove
   u32                     timeThen, timeNow;                                       //stores game time values
   u32                     timeThen2, timeNow2;                                     //stores game time values
   u32                     timeThen1, timeNow1;                                     //stores game time values
   u32                     timeThen3, timeNow3;                                     //stores game time values
   u32                     timeThen4, timeNow4;                                     //stores game time values
   u32                     timeThen5, timeNow5;                                     //stores game time values

   ITexture                *gameHUD, *binGUI;                                       //in-game HUD (or vignette), binoculars
   ITexture                *companyInfo;                                            //image that is displayed on game start
   ITexture                *mainMenuGUI, *pleaseWaitImg, *levelPreview, *mapSelect, *creditsBack, *optionsBack;
   ITexture                *title_animate, *loadingBar;
   ITexture				   *lifeBar, *ammoStats, *lifeBarLow, *ammoStatsLow;
   IGUIImage               *crosshair;                                              //crosshair in the middle of the screen
   ITexture                *grassTex1, *grassTex2;                                  //used by the grass node
   video::IImage		   *heightMap, *textureMap, *grassMap;						//used by the grass node also

   ISceneNodeAnimator      *menuSelectorMoveAnimator, *menuCamMoveAnimator;         //used for main menu -- animators
   ISceneNodeAnimator      *player1MoveAnimator, *player2MoveAnimator, *player3MoveAnimator, *player4MoveAnimator;  //used for multiplayer player movement
   ISceneNodeAnimator	   *cameraShakeAnimator;

   int                     showCompanyInfo = 0;                                     //starts the intro animation (company + game info)
   int                     isBackImg       = 1;                                     //used for deciding if it is in main menu, it dislpays the mainMenuGUI
   int                     isPleaseWaitImg = 0;                                     //used for deciding if is loading a level
   int                     escWait         = 1;                                     //escape key was pressed, waiting for the user to choose an option, you can't press escape again
   int                     isInGame = 0, animationEnded = 1;
   int                     updateNetNow = 0;                                        //value (game time) used for updating the network code in the main loop
   int                     pistolIsVisible = 0;
   int                     mainMenuView = 1;
   int                     singleplayer_window_on = 0;
   int                     lastPhotoID;                                             //used by the screenshot function (takes the last photo name and increments it, then saves a screenshot with that new name)
   int                     binGUIisOn = 0;
   int					   drawNextSlide = 1, drawNextSlideLB = 1;
   int                     fpsValOn = 0;											//turn on/off the fps value [*on only in game, for the console]
   int					   consoleIsOn = 0;
   int                     startToShakeCamera = 0;
   int					   directionVec = 1;										//this decides if the car is going forward or backwords

   bool					   startStoppingExplosionAnimators = false;
   bool					   isInVehicleJeeep = false;
   int					   timeElapsedToDestroyEmiter = 0;
   f32					   jeepVehicleRotation = 0;

   IGUIListBox             *maps_list;
   IGUIEditBox			   *consoleInput;

   IGUIStaticText          *lifeBarText, *ammoStatsText, *fpsVal, *torqueValueText;
   f32					    lifeBarValue = 100, ammoStatsValue = 48;
   f32						torqueVal = 0;

   IGUIInOutFader          *screen_fader_black, *screenShot_fader, *escFadeToBlack, *escFadeToNormal;
   IGUIWindow              *singleplayer_window, *options_window, *credits_window;
   IGUIWindow              *modal_MEC_window, *modal_CON_window;                     //MEC = Main menu, Exit game, Cancel / CON = Console window

   IGUIButton              *startGameBtn, *creditsBtn, *quitGameBtn, *optionsBtn;    //the main menu buttons
   IGUIEditBox			   *briefingBox;

   IAttributes             *attribs, *attribs2;                                                 //stores attributes from nodes found in .irr file

   core::vector3df			intersection, intersection2, normal2;
   core::vector3df			nodeInFrontOfPlayerPos;


   //title animation
	core::rect<s32> imp1(0  ,0  ,390,47 );
    core::rect<s32> imp2(0  ,47 ,390,94 );
	core::rect<s32> imp3(0  ,94 ,390,141);
    core::rect<s32> imp4(0  ,141,390,188);
	core::rect<s32> imp5(0  ,188,390,235);
    core::rect<s32> imp6(0  ,235,390,282);
	core::rect<s32> imp7(0  ,282,390,329);
    core::rect<s32> imp8(0  ,329,390,376);

	//loading bar
	core::rect<s32> lb1(0  ,0  ,128,128);
    core::rect<s32> lb2(128,0  ,256,128);
	core::rect<s32> lb3(256,0  ,384,128);
    core::rect<s32> lb4(384,0  ,512,128);
	core::rect<s32> lb5(512,0  ,640,128);
    core::rect<s32> lb6(640,0  ,768,128);
	core::rect<s32> lb7(768,0  ,896,128);
    core::rect<s32> lb8(896,0  ,1024,128);
	core::rect<s32> lb9(1024,0 ,1152,128);
    core::rect<s32> lb10(1152,0 ,1280,128);
	core::rect<s32> lb11(1280,0 ,1408,128);
    core::rect<s32> lb12(1408,0 ,1536,128);
	core::rect<s32> lb13(1536,0 ,1664,128);

	core::array<video::ITexture*> explosionTextures;


   	struct SParticleImpact
	{
		u32 when;
		core::vector3df pos;
		core::vector3df outVector;
	};

   array<SParticleImpact> Impacts;


// Windows variables

   char dateStr [9], timeStr [9];                                                   //stores system date and time, used for logging to file


// Misc variables -- default game values

   int screenResX = 800, screenResY = 600, wfullscreen=0;                           //video
   bool wfullscreenBool=false;
   bool isWFullscreen, isMusicOn, isSfxOn, isSkipIntro, isBloom;
   int mapDetail  = 1, shadows = 0, bloom = 0, normalMapping = 0, grass = 1, farView = 6000;   //detail
   int musicOn = 1, sfxOn = 1, musicVol = 1, sfxVol = 1;                            //audio
   int skipIntro = 0;																//debug
   int reticule = 2;                                                                //gui
   int XMLmapCount = 1;                                                             //map count
   stringw XMLmapName;                                                              //map name
   u32 XMLfogValueR, XMLfogValueG, XMLfogValueB;                                    //fog value
   stringw XMLskyValue;                                                             //sky value
   stringw lastUsedProfile, selectedProfileLanguage;								//profile and language
   stringw startGameLValue, optionsLValue, saveAndCloseLValue, creditsLValue, quitGameLValue, loadMapLValue, cancelLValue, mainMenuLValue; //main menu translations
   stringw consoleLValue, submitLValue, closeLValue;							    //in-game translations
   stringw releaseValue, buildValue;												//game version information
   stringw mapBriefing;																//get and store value from XML about the map briefing
