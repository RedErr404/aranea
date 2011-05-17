#ifndef INC_MATHCONVERSION_H
#define INC_MATHCONVERSION_H

#include <NxPhysics.h>
#include <irrlicht.h>

using namespace irr;

namespace IrrPhysx {

#define ROUND(value) floor(value+0.5)

/**
\brief Convert a core::vector3df rotation vector to an NxMat33 rotation matrix
\param rot - the Irrlicht vector to convert
\param mat - the Physx matrix to recieve the conversion
*/
void rotationVectorToPhysxMatrix(const core::vector3df& rot, NxMat33& mat);
/**
\brief Convert an NxMat33 rotation matrix to a core::vector3df rotation vector

Useful for turning the orientation of a Physx object into a vector to rotate a node by
\param mat - the Physx matrix to convert
\param rot - the Irrlicht vector to recieve the conversion
*/
void physxMatrixToRotationVector(const NxMat33& mat, core::vector3df& rot);

} // end IrrPhysx namespace

#endif /* INC_MATHCONVERSION_H */
