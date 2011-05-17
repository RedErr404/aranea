#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

vector3df lastPosition;
int nextShakeCue = 0;

void shakeCameraExplosion()
{

ISceneManager* smgr = device->getSceneManager();
lastPosition = camera->getTarget();

u32 timeNow4 = device->getTimer()->getTime();

//start to shake
//camera->setTarget(core::vector3df(lastPosition.X, lastPosition.Y+rand()%2, lastPosition.Z));

//stop shake after 1 second
if ((timeThen4 + 30) < timeNow4)
{
	nextShakeCue = nextShakeCue + 1;
	if (nextShakeCue == 1){ camera->setTarget(core::vector3df(lastPosition.X-rand()%20, lastPosition.Y+rand()%20, lastPosition.Z)); }
	if (nextShakeCue == 2){ camera->setTarget(core::vector3df(lastPosition.X, lastPosition.Y-rand()%20, lastPosition.Z));}
	if (nextShakeCue == 3){ camera->setTarget(core::vector3df(lastPosition.X+rand()%20, lastPosition.Y, lastPosition.Z+rand()%20));}
	if (nextShakeCue == 4){ camera->setTarget(core::vector3df(lastPosition.X-rand()%20, lastPosition.Y, lastPosition.Z));}
	if (nextShakeCue == 5){ camera->setTarget(core::vector3df(lastPosition.X-rand()%20, lastPosition.Y, lastPosition.Z+rand()%20));}
	if (nextShakeCue == 6){ camera->setTarget(core::vector3df(lastPosition.X, lastPosition.Y+rand()%20, lastPosition.Z-rand()%20));}
	if (nextShakeCue == 7){ camera->setTarget(core::vector3df(lastPosition.X+rand()%20, lastPosition.Y+rand()%20, lastPosition.Z)); }
	if (nextShakeCue == 8){ camera->setTarget(core::vector3df(lastPosition.X, lastPosition.Y+rand()%20, lastPosition.Z));}
	if (nextShakeCue == 9){ camera->setTarget(core::vector3df(lastPosition.X-rand()%20, lastPosition.Y, lastPosition.Z-rand()%20));}
	if (nextShakeCue == 10){ camera->setTarget(core::vector3df(lastPosition.X-rand()%20, lastPosition.Y, lastPosition.Z));}
	if (nextShakeCue == 11){ camera->setTarget(core::vector3df(lastPosition.X+rand()%20, lastPosition.Y, lastPosition.Z-rand()%20));}
	if (nextShakeCue == 12){ camera->setTarget(core::vector3df(lastPosition.X, lastPosition.Y-rand()%20, lastPosition.Z+rand()%20));}
   	if (nextShakeCue == 13){ nextShakeCue = 0; startToShakeCamera = 0; }
	
	timeThen4 = timeNow4;   

} else {}







}

