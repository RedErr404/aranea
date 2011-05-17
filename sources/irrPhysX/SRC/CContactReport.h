#ifndef INC_CCONTACTREPORT_H
#define INC_CCONTACTREPORT_H

#include <NxPhysics.h>
#include "IPhysxObject.h"

namespace IrrPhysx {

class CContactReport : public NxUserContactReport { 
  
	public:
		CContactReport() { ContactCallback = NULL; }
		
		inline void setCallback(void (*cb)(IPhysxObject*, IPhysxObject*)) { ContactCallback = cb; }

	private:
		void (*ContactCallback)(IPhysxObject*, IPhysxObject*);

		inline void onContactNotify(NxContactPair& pair, NxU32 events) {

			if (ContactCallback)
				ContactCallback((IPhysxObject*)pair.actors[0]->userData, (IPhysxObject*)pair.actors[1]->userData);

		}

};

} // end IrrPhysx namespace

#endif /* INC_CCONTACTREPORT_H */
