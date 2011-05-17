#include "IrrPhysx.h"
#include "CPhysxManager.h"

namespace IrrPhysx {

IPhysxManager* IrrPhysx::createPhysxManager(IrrlichtDevice* device, const SSceneDesc& sceneDesc, bool disableHardware) {
	
	return new CPhysxManager(device, sceneDesc, disableHardware);

}

void IrrPhysx::removePhysxManager(IPhysxManager* manager) {

	delete manager;
	
}

} // end IrrPhysx namespace
