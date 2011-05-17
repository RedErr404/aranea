#ifndef INC_IRRPHYSXTYPES_H
#define INC_IRRPHYSXTYPES_H

/**
\brief Enum of different object types which can be created by a IPhysxManager
*/
enum E_OBJECT_TYPE {
	EOT_BOX,
	EOT_MULTISHAPE,
	EOT_SPHERE,
	EOT_CAPSULE,
	EOT_PLANE,
	EOT_HEIGHTFIELD,
	EOT_CONVEX_MESH,
	EOT_TRIANGLE_MESH,
	EOT_CLOTH,
	E_NUM_OBJECT_TYPES
};

/**
\brief Enum of different mesh types which can be created by a IPhysxManager
*/
enum E_MESH_TYPE {
	EMT_CONVEX,
	EMT_TRIANGLE,
	EMT_CLOTH,
	E_NUM_MESH_TYPES
};

#endif /* INC_IRRPHYSXTYPES_H */
