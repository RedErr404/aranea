#ifndef INC_CPHYSXOBJECT_H
#define INC_CPHYSXOBJECT_H

#include <NxPhysics.h>
#include "IPhysxObject.h"

namespace IrrPhysx {

class CPhysxObject : public IPhysxObject {

	public:
		CPhysxObject(NxScene* scene, NxActor* actor, E_OBJECT_TYPE type);
		virtual ~CPhysxObject();

		void setCollisionGroup(u16 group);
		void getPosition(core::vector3df& pos);
		void setPosition(const core::vector3df& pos);
		void getRotation(core::vector3df& rot);
		void setRotation(const core::vector3df& rot);
		void setLinearVelocity(const core::vector3df& vel);
		void getLinearVelocity(core::vector3df& vel);
		void setAngularVelocity(const core::vector3df& vel);
		void getAngularVelocity(core::vector3df& vel);
		void setLinearDamping(f32 damping);
		f32 getLinearDamping();
		void setAngularDamping(f32 damping);
		f32 getAngularDamping();

		inline NxActor* getActor() { return Actor; }

	protected:
		NxScene* Scene;
		NxActor* Actor;


};

} // end IrrPhysx namespace

#endif /* INC_CPHYSXOBJECT_H */
