#ifndef INC_IPHYSXMESH_H
#define INC_IPHYSXMESH_H

#include "IrrPhysxTypes.h"
#include <irrlicht.h>

using namespace irr;

namespace IrrPhysx {

/**
Interface to Physx meshes
*/
class IPhysxMesh {

	public:
		/**
		\brief Constructor. Avoid using, rather use functions from IPhysxManager
		\param type - the type of the mesh
		\param scale - the scale the mesh was created at
		*/
		IPhysxMesh(E_MESH_TYPE type, const core::vector3df& scale) {

			Type = type;
			Scale = scale;

		}
		/**
		\brief Destructor. Avoid using, rather IPhysxManager::removeMesh()
		*/
		virtual ~IPhysxMesh() {}
		
		/**
		\brief Returns the number of objects referencing this mesh

		If this does not return 0 then IPhysxManager::removeMesh() will not remove the mesh
		\return a u32
		*/
		virtual u32 getReferenceCount() = 0;

		/**
		\brief Returns the type of the mesh
		\return a E_MESH_TYPE
		*/
		inline E_MESH_TYPE getType() { return Type; }

	protected:
		E_MESH_TYPE Type;
		core::vector3df Scale;

};

}

#endif /* INC_IPHYSXMESH_H */
