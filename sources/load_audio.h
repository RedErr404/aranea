#include <stdio.h>
#include "fmod.hpp"
#include "fmod_errors.h"

using namespace std;
using namespace FMOD;

#pragma comment(lib, "fmodex_vc.lib")

  FMOD::System *systemFMOD;
  FMOD_RESULT result;

  FMOD::Channel *channelMusic = 0;
  FMOD::Channel *channelSfxAudioNoLoop = 0;

  FMOD::Sound *mainMenuBGMusic, *ambientWind;
  FMOD::Sound *sfxClick;
  FMOD::Sound *sfxPistolReload, *sfxPistolShoot, *sfxPistolEmpty;
  FMOD::Sound *sfxNearExplosion, *sfxDistantExplosion;

  bool isPlaying;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("[ERROR] FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        
    }
}

void loadAudioFiles()
{
if (musicOn == 1)
{
  // load music
  result = FMOD::System_Create(&systemFMOD);
  ERRCHECK(result);
  result = systemFMOD->setOutput(FMOD_OUTPUTTYPE_DSOUND);
  ERRCHECK(result);
  result = systemFMOD->init(32, FMOD_INIT_NORMAL, 0);
  ERRCHECK(result);

  systemFMOD->createSound("../data/audio/music/main_menu.ogg",       FMOD_HARDWARE, 0, &mainMenuBGMusic); ERRCHECK(result);
  systemFMOD->createSound("../data/audio/sfx/ambient/wind_loop.ogg", FMOD_HARDWARE, 0, &ambientWind);     ERRCHECK(result);
  systemFMOD->createSound("../data/audio/sfx/click.ogg",             FMOD_HARDWARE, 0, &sfxClick);        ERRCHECK(result);

  systemFMOD->createSound("../data/audio/sfx/pistol/pistol_reload.ogg", FMOD_HARDWARE, 0, &sfxPistolReload); ERRCHECK(result); 
  systemFMOD->createSound("../data/audio/sfx/pistol/pistol_shoot.ogg",  FMOD_HARDWARE, 0, &sfxPistolShoot);  ERRCHECK(result); 
  systemFMOD->createSound("../data/audio/sfx/pistol/pistol_empty.ogg",  FMOD_HARDWARE, 0, &sfxPistolEmpty);  ERRCHECK(result); 

 systemFMOD->createSound("../data/audio/sfx/explosions/near_explosion.ogg",    FMOD_HARDWARE, 0, &sfxNearExplosion);    ERRCHECK(result);
 systemFMOD->createSound("../data/audio/sfx/explosions/distant_explosion.ogg", FMOD_HARDWARE, 0, &sfxDistantExplosion); ERRCHECK(result);



  result = systemFMOD->update();
  ERRCHECK(result);

}

}
