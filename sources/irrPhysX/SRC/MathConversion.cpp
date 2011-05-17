#include "MathConversion.h"

namespace IrrPhysx {

void rotationVectorToPhysxMatrix(const core::vector3df& rot, NxMat33& mat) {

	NxMat33 rotX, rotY, rotZ;
	rotX.rotX(rot.X * core::DEGTORAD);
	rotY.rotY(rot.Y * core::DEGTORAD);
	rotZ.rotZ(rot.Z * core::DEGTORAD);
	mat = rotZ * rotY * rotX;

}

void physxMatrixToRotationVector(const NxMat33& mat, core::vector3df& rot) {

	f32 fM[16];
	mat.getColumnMajorStride4(fM);
	core::matrix4 irrM;
	irrM.setM(fM);
	rot = irrM.getRotationDegrees();

}

} // end IrrPhysx namespace
