#include "CPhysxObject.h"
#include "MathConversion.h"

namespace IrrPhysx {

CPhysxObject::CPhysxObject(NxScene* scene, NxActor* actor, E_OBJECT_TYPE type) : IPhysxObject(type) {

	Scene = scene;
	Actor = actor;
	Actor->userData = this;

}

CPhysxObject::~CPhysxObject() {

	if (Scene) {
		Scene->releaseActor(*Actor);
		Actor = 0;
	}

}

void CPhysxObject::setCollisionGroup(u16 group) {

	IPhysxObject::setCollisionGroup(group);

	Actor->setGroup(group);
    NxShape*const* shapes = Actor->getShapes();
    NxU32 nShapes = Actor->getNbShapes();
    while (nShapes--)
        shapes[nShapes]->setGroup(group);
	
}

void CPhysxObject::getPosition(core::vector3df& pos) {

	NxVec3 p = Actor->getGlobalPosition();
	pos.X = p.x;
	pos.Y = p.y;
	pos.Z = p.z;

}

void CPhysxObject::setPosition(const core::vector3df& pos) {

	Actor->setGlobalPosition(NxVec3(pos.X, pos.Y, pos.Z));

}

void CPhysxObject::getRotation(core::vector3df& rot) {

	physxMatrixToRotationVector(Actor->getGlobalOrientation(), rot);

}

void CPhysxObject::setRotation(const core::vector3df& rot) {

	NxMat33 mat;
	rotationVectorToPhysxMatrix(rot, mat);
	Actor->setGlobalOrientation(mat);

}

void CPhysxObject::setLinearVelocity(const core::vector3df& vel) { 
		
	if (Type != EOT_TRIANGLE_MESH &&
		Type != EOT_HEIGHTFIELD &&
		Type != EOT_PLANE &&
		Actor->getMass() != 0) 
		Actor->setLinearVelocity(NxVec3(vel.X, vel.Y, vel.Z)); 
	
}

void CPhysxObject::getLinearVelocity(core::vector3df& vel) { 
			
	NxVec3 velocity = Actor->getLinearVelocity(); 
	vel.X = velocity.x;
	vel.Y = velocity.y;
	vel.Z = velocity.z;
		
}
		
void CPhysxObject::setAngularVelocity(const core::vector3df& vel) { 
			
	if (Type != EOT_TRIANGLE_MESH &&
		Type != EOT_HEIGHTFIELD &&
		Type != EOT_PLANE &&
		Actor->getMass() != 0) 
		Actor->setAngularVelocity(NxVec3(vel.X, vel.Y, vel.Z)); 
		
}

void CPhysxObject::getAngularVelocity(core::vector3df& vel) { 
			
	NxVec3 velocity = Actor->getAngularVelocity(); 
	vel.X = velocity.x;
	vel.Y = velocity.y;
	vel.Z = velocity.z;
		
}
		
void CPhysxObject::setLinearDamping(f32 damping) { 
			
	if (Type != EOT_TRIANGLE_MESH &&
		Type != EOT_HEIGHTFIELD &&
		Type != EOT_PLANE &&
		Actor->getMass() != 0) 
		Actor->setLinearDamping(damping); 
	
}

f32 CPhysxObject::getLinearDamping( ) { 
			
	return Actor->getLinearDamping(); 
	
}
		
void CPhysxObject::setAngularDamping(f32 damping) { 
			
	if (Type != EOT_TRIANGLE_MESH &&
		Type != EOT_HEIGHTFIELD &&
		Type != EOT_PLANE &&
		Actor->getMass() != 0) 
		Actor->setAngularDamping(damping); 
		
}

f32 CPhysxObject::getAngularDamping() { 
			
	return Actor->getAngularDamping(); 
		
}

} // end IrrPhysx namespace
