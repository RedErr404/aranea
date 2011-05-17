#ifndef INC_ICLOTHPHYSXOBJECT_H
#define INC_ICLOTHPHYSXOBJECT_H

#include "IPhysxObject.h"

namespace IrrPhysx {

/**
Interface to Physx objects
*/
class IClothPhysxObject : public IPhysxObject {

	public:
		/**
		\brief Constructor. Avoid using, rather use functions from IPhysxManager
		*/
		IClothPhysxObject(E_OBJECT_TYPE type) : IPhysxObject(type) {}
		/**
		\brief Destructor
		*/
		virtual ~IClothPhysxObject() {}
		
		/**
		\brief Returns the number of vertices in the cloth
		\return a u32
		*/
		virtual u32 getNumberOfVertices() = 0;
		/**
		\brief Attaches the cloth to the specified object

		The specified object should not be a cloth/softbody
		\param object - the object to attach to
		*/
		virtual void attachToObject(IPhysxObject* object) = 0;
		/**
		\brief Attaches the cloth to all objects which it is currently colliding with
		*/
		virtual void attachToCollidingObjects() = 0;
		/**
		\brief Applies the specified vector to the cloth as wind
		\param windVector - the wind
		*/
		virtual void applyWindToCloth(const core::vector3df& windVector) = 0;
		/**
		\brief Updates the cloth performing checks on tearing etc
		*/
		virtual void update() = 0;
		/**
		\brief Perform a raycast on this cloth, returning true if it was hit, false otherwise
		\param ray - the ray to be cast
		\param intersectionPoint - the vector to receive the intersection point
		\param vertexIndex - the closest vertex to the intersection point
		\return a bool
		*/
		virtual bool raycast(const core::line3df& ray, core::vector3df* intersectionPoint = NULL, u32* vertexIndex = NULL) = 0;
		/**
		\brief Attempts to tear the specified vertex, splitting the adjoining triangles according to the specified normal and returns true if successful

		Note that the cloth object does not need to have been created with the tearable flag for this to succeed but the cloth mesh used to create the object does
		\param vertexIndex - the index of the vertex to be torn
		\param normal - the normal which defines the split plane
		return a bool
		*/
		virtual bool tearVertex(u32 vertexIndex, const core::vector3df& normal) = 0;
		/**
		\brief Sets the specified vertex's velocity
		\param vertexIndex - the index of the vertex you wish to set the velocity of
		\param vel - the velocity to set the vertex to
		*/
		virtual void setVelocity(u32 vertexIndex, const core::vector3df& vel) = 0;
		/**
		\brief Sets the specified vector to the specified vertex's velocity
		\param vertexIndex - the index of the vertex you wish to get the velocity of
		\param vel - the vector to receive the velocity
		*/
		virtual void getVelocity(u32 vertexIndex, core::vector3df& vel) = 0;
		/**
		\brief Sets the velocities of all the vertices in the cloth to those specified by the velocities buffer

		Note that the buffer must be the same size as the number of vertices in the cloth
		\param velocities - the buffer of velocity values to set the cloth vertices to
		*/
		virtual void setVelocities(core::vector3df* velocities) = 0;
		/**
		\brief Sets the buffer of vectors to the velocities of all the vertices in the cloth

		Note that the buffer must be the same size as the number of vertices in the cloth
		\param velocities - the buffer of vectors to recieve the vertex velocities
		*/
		virtual void getVelocities(core::vector3df* velocities) = 0;
		/**
		\brief Sets the positions of all the vertices in the cloth to those specified by the positions buffer

		Note that the buffer must be the same size as the number of vertices in the cloth
		\param positions - the buffer of positions to set the cloth vertices to
		*/
		virtual void setPositions(core::vector3df* positions) = 0;
		/**
		\brief Sets the buffer of vectors to the positions of all the vertices in the cloth

		Note that the buffer must be the same size as the number of vertices in the cloth
		\param positions - the buffer of vectors to recieve the vertex positions
		*/
		virtual void getPositions(core::vector3df* positions) = 0;

};

} // end IrrPhysx namespace

#endif /* INC_ICLOTHPHYSXOBJECT_H */
