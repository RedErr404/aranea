#ifndef INC_IPHYSXOBJECT_H
#define INC_IPHYSXOBJECT_H

#include <irrlicht.h>
#include "IrrPhysxTypes.h"

using namespace irr;

namespace IrrPhysx {

/**
Interface to Physx objects
*/
class IPhysxObject {

	public:
		/**
		\brief Constructor. Avoid using, rather use functions from IPhysxManager
		*/
		IPhysxObject(E_OBJECT_TYPE type) {
			Type = type;
			CollisionGroup = 0;
		}
		/**
		\brief Destructor. Avoid using, rather use IPhysxManager::removeObject()
		*/
		virtual ~IPhysxObject() {}

		/**
		\brief Returns the type of the object
		\return an u32
		*/
		inline virtual E_OBJECT_TYPE getType() { return Type; }
		/**
		\brief Sets the collision group of the object

		This tells Physx which groups this object belongs to so it knows which other objects it should be colliding with and which it shouldn't
		\param group - the group to set the object to
		*/
		inline virtual void setCollisionGroup(u16 group) { CollisionGroup = group; }
		/**
		\brief Returns the collision group of the objects
		\return an u16
		*/
		inline virtual u16 getCollisionGroup() { return CollisionGroup; }
		/**
		\brief Sets the user data of this object

		This can be used for various things such as pointing to one of your own class instances which owns/manages this object
		\param userData - the data to be used
		*/
		inline virtual void setUserData(void* userData) { UserData = userData; }
		/**
		\brief Returns the object's user data
		\return a void*
		*/
		inline virtual void* getUserData() { return UserData; }
			
		/**
		\brief Sets the specified vector to the current position of the object

		This can be used to sync your ISceneNode visual representation of the object with its physical representation
		Note that if the object is a cloth then this function will return the position of the first vertex in the cloth. IClothPhysxObject has functions to get the individual vertex positions
		\param pos - the vector to receive the position
		*/
		virtual void getPosition(core::vector3df& pos) = 0;
		/**
		\brief Sets the object's position to the specified vector

		Note that if the object is a cloth then this function will set the position of the first vertex in the cloth. IClothPhysxObject has functions to set the individual vertex positions
		\param pos - the position to set
		*/
		virtual void setPosition(const core::vector3df& pos) = 0;
		/**
		\brief Sets the specified vector to the current rotation of the object

		This can be used to sync your ISceneNode visual representation of the object with its physical representation
		Note that if this object is a cloth then this function has no effect on the vector passed
		\param rot - the vector to receive the rotation
		*/
		virtual void getRotation(core::vector3df& rot) = 0;
		/**
		\brief Sets the object's rotation to the specified vector

		Note that if this object is a cloth then this function has no effect
		\param rot - the rotation to set
		*/
		virtual void setRotation(const core::vector3df& rot) = 0;
		/**
		\brief Sets the object's linear velocity to the specified vector
		\param vel - the linear velocity to set
		*/
		virtual void setLinearVelocity(const core::vector3df& vel) = 0;
		/**
		\brief Sets the specified vector to the linear velocity of the object

		Note that if the object is a cloth then this function will return the velocity of the first vertex in the cloth. IClothPhysxObject has functions to get the individual vertex velocities
		\param vel - the vector to receive the velocity
		*/
		virtual void getLinearVelocity(core::vector3df& vel) = 0;
		/**
		\brief Sets the object's angular velocity to the specified vector

		Note that if the object is a cloth then this function will have the same effect as setLinearVelocity()
		Note also that each vertex in the cloth will be set to the same value. IClothPhysxObject has functions to set the individual vertex velocities
		\param vel - the angular velocity to set
		*/
		virtual void setAngularVelocity(const core::vector3df& vel) = 0;
		/**
		\brief Sets the specified vector to the angular velocity of the object
		\param vel - the vector to receive the velocity
		*/
		virtual void getAngularVelocity(core::vector3df& vel) = 0;
		/**
		\brief Sets the object's linear damping to the specified value, which should be positive

		Note that if the object is a cloth then the damping value cannot be larger than 1
		\param damping - the linear damping to set
		*/
		virtual void setLinearDamping(f32 damping) = 0;
		/**
		\brief Returns the linear damping of the object
		\return a f32
		*/
		virtual f32 getLinearDamping() = 0;
		/**
		\brief Sets the object's angular damping to the specified value, which should be positive

		Note that if the object is a cloth then this function will have the same effect as setLinearDamping()
		\param damping - the angular damping to set
		*/
		virtual void setAngularDamping(f32 damping) = 0;
		/**
		\brief Returns the angular damping of the object
		\return a f32
		*/
		virtual f32 getAngularDamping() = 0;
		
	protected:
		E_OBJECT_TYPE Type;
		u16 CollisionGroup;
		void* UserData;

};

} // end IrrPhysx namespace

#endif /* INC_IPHYSXOBJECT_H */
