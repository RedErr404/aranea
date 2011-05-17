#ifndef INC_CCLOTHPHYSXOBJECT_H
#define INC_CCLOTHPHYSXOBJECT_H

#include <NxPhysics.h>
#include "IClothPhysxObject.h"
#include "CCloth.h"

namespace IrrPhysx {

class CClothPhysxObject : public IClothPhysxObject {

	public:
		CClothPhysxObject(NxScene* scene, CCloth* cloth);
		~CClothPhysxObject();
		void getRotation(core::vector3df& rot) {}
		void setRotation(const core::vector3df& rot) {}

		u32 getNumberOfVertices();
		void setCollisionGroup(u16 group);
		void getPosition(core::vector3df& pos);
		void setPosition(const core::vector3df& pos);
		void attachToObject(IPhysxObject* object);
		void attachToCollidingObjects();
		void applyWindToCloth(const core::vector3df& windVector);
		void update();
		bool raycast(const core::line3df& ray, core::vector3df* intersectionPoint = NULL, u32* vertexIndex = NULL);
		bool tearVertex(u32 vertexIndex, const core::vector3df& normal);
		void setLinearVelocity(const core::vector3df& vel);
		void getLinearVelocity(core::vector3df& vel);
		void setAngularVelocity(const core::vector3df& vel);
		void getAngularVelocity(core::vector3df& vel);
		void setLinearDamping(f32 damping);
		f32 getLinearDamping();
		void setAngularDamping(f32 damping);
		f32 getAngularDamping();
		void setVelocity(u32 vertexIndex, const core::vector3df& vel);
		void getVelocity(u32 vertexIndex, core::vector3df& vel);
		void setVelocities(core::vector3df* velocities);
		void getVelocities(core::vector3df* velocities);
		void setPositions(core::vector3df* positions);
		void getPositions(core::vector3df* positions);

		inline CCloth* getCloth() { return Cloth; }

	private:
		NxScene* Scene;
		CCloth* Cloth;

};

} // end IrrPhysx namespace

#endif /* INC_CCLOTHPHYSXOBJECT_H */
