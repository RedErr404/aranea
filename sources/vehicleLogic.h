#include "irrlicht.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;



void vehicleReceiver()
{

   core::vector3df nodeInFrontOfPlayerPos = vehicleJeep->SceneNode->getPosition();
   camera->setPosition(core::vector3df(nodeInFrontOfPlayerPos.X, nodeInFrontOfPlayerPos.Y+100, nodeInFrontOfPlayerPos.Z));

    core::vector3df vell = camera->getTarget() - camera->getPosition();
	vell.normalize();	
	vell*=torqueVal;
	//propulsate car
	vehicleJeep->PhysxObject->setLinearVelocity(f32(directionVec)*vell);

	//rotate car
	core::vector3df vellRot;
	camera->setRotation(core::vector3df(vellRot.X, vellRot.Y+jeepVehicleRotation, vellRot.Z));
	vehicleJeep->PhysxObject->setRotation(core::vector3df(vellRot.X, vellRot.Y+jeepVehicleRotation, vellRot.Z));








}
