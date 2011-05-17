#ifndef INC_CRAYCASTREPORT_H
#define INC_CRAYCASTREPORT_H

#include "CPhysxManager.h"

namespace IrrPhysx {

class CRaycastReport : public NxUserRaycastReport {

	public:
		CRaycastReport(CPhysxManager* physxManager) {
			PhysxManager = physxManager;
		}

		bool onHit(const NxRaycastHit& hit) {

			CPhysxObject* object = (CPhysxObject*)hit.shape->getActor().userData;
			PhysxManager->addObjectHitByRaycast(object, hit.worldImpact, hit.worldNormal);

			return true;
		}

	private:
		CPhysxManager* PhysxManager;

};

} // end IrrPhysx namespace

#endif /* INC_CRAYCASTREPORT_H */
