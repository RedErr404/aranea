#include "CClothPhysxObject.h"
#include "CPhysxObject.h"

namespace IrrPhysx {

CClothPhysxObject::CClothPhysxObject(NxScene* scene, CCloth* cloth) : IClothPhysxObject(EOT_CLOTH) {

	Scene = scene;
	Cloth = cloth;
	Cloth->getNxCloth()->userData = this;

}

CClothPhysxObject::~CClothPhysxObject() {

	delete Cloth;
		
}

void CClothPhysxObject::setCollisionGroup(u16 group) {

	IPhysxObject::setCollisionGroup(group);

	Cloth->getNxCloth()->setGroup(group);
	
}

void CClothPhysxObject::getPosition(core::vector3df& pos) {

	NxVec3 p = Cloth->getNxCloth()->getPosition(0);
	pos.X = p.x;
	pos.Y = p.y;
	pos.Z = p.z;

}

void CClothPhysxObject::setPosition(const core::vector3df& pos) {

	Cloth->getNxCloth()->setPosition(NxVec3(pos.X, pos.Y, pos.Z), 0);

}

u32 CClothPhysxObject::getNumberOfVertices() {

	return Cloth->getNxCloth()->getNumberOfParticles();

}

void CClothPhysxObject::attachToObject(IPhysxObject* object) {

	if (object->getType() == EOT_CLOTH) return;

	Cloth->getNxCloth()->attachToShape(*((CPhysxObject*)object)->getActor()->getShapes(), 0);

}

void CClothPhysxObject::attachToCollidingObjects() {

	Cloth->getNxCloth()->attachToCollidingShapes(0);

}

void CClothPhysxObject::applyWindToCloth(const core::vector3df& windVector) {

	Cloth->getNxCloth()->setWindAcceleration(NxVec3(windVector.X, windVector.Y, windVector.Z));

}

void CClothPhysxObject::update() {

	Cloth->update();

}

bool CClothPhysxObject::raycast(const core::line3df& ray, core::vector3df* intersectionPoint, u32* vertexIndex) {

	NxVec3 hitPos;
	u32 vertexId;
	NxRay worldRay;
	const core::vector3df dir = ray.getVector().normalize();
	worldRay.dir = NxVec3(dir.X, dir.Y, dir.Z);
	worldRay.orig = NxVec3(ray.start.X, ray.start.Y, ray.start.Z);

	bool hit = Cloth->getNxCloth()->raycast(worldRay, hitPos, vertexId);

	if (hit) {
		if (intersectionPoint) {
			intersectionPoint->X = hitPos.x;
			intersectionPoint->Y = hitPos.y;
			intersectionPoint->Z = hitPos.z;
		}
		if (vertexIndex) *vertexIndex = vertexId;
	}

	return hit;

}

bool CClothPhysxObject::tearVertex(u32 vertexIndex, const core::vector3df& normal) {

	return Cloth->getNxCloth()->tearVertex(vertexIndex, NxVec3(normal.X, normal.Y, normal.Z));

}

void CClothPhysxObject::setLinearVelocity(const core::vector3df& vel) {

	NxVec3 velocity(vel.X, vel.Y, vel.Z);

	for (u32 i = 0 ; i < Cloth->getNxCloth()->getNumberOfParticles() ; ++i)
		Cloth->getNxCloth()->setVelocity(velocity, i);

}

void CClothPhysxObject::getLinearVelocity(core::vector3df& vel) {

	NxVec3 velocity = Cloth->getNxCloth()->getVelocity(0);
	vel.X = velocity.x;
	vel.Y = velocity.y;
	vel.Z = velocity.z;

}

void CClothPhysxObject::setAngularVelocity(const core::vector3df& vel) {

	setLinearVelocity(vel);

}

void CClothPhysxObject::getAngularVelocity(core::vector3df& vel) {

	getLinearVelocity(vel);

}

void CClothPhysxObject::setLinearDamping(f32 damping) {

	if (damping > 1.0f) damping = 1.0f;

	Cloth->getNxCloth()->setDampingCoefficient(damping);

}

f32 CClothPhysxObject::getLinearDamping() {

	return Cloth->getNxCloth()->getDampingCoefficient();

}

void CClothPhysxObject::setAngularDamping(f32 damping) {

	setLinearDamping(damping);

}

f32 CClothPhysxObject::getAngularDamping() {

	return getLinearDamping();

}

void CClothPhysxObject::setVelocity(u32 vertexIndex, const core::vector3df& vel) {

	Cloth->getNxCloth()->setVelocity(NxVec3(vel.X, vel.Y, vel.Z), vertexIndex);

}

void CClothPhysxObject::getVelocity(u32 vertexIndex, core::vector3df& vel) {

	NxVec3 velocity = Cloth->getNxCloth()->getVelocity(vertexIndex);
	vel.X = velocity.x;
	vel.Y = velocity.y;
	vel.Z = velocity.z;

}
void CClothPhysxObject::setVelocities(core::vector3df* velocities) {

	Cloth->getNxCloth()->setVelocities(velocities, sizeof(core::vector3df));

}

void CClothPhysxObject::getVelocities(core::vector3df* velocities) {

	Cloth->getNxCloth()->getVelocities(velocities, sizeof(core::vector3df));

}

void CClothPhysxObject::setPositions(core::vector3df* positions) {

	Cloth->getNxCloth()->setPositions(positions, sizeof(core::vector3df));

}
		
void CClothPhysxObject::getPositions(core::vector3df* positions) {

	Cloth->getNxCloth()->getPositions(positions, sizeof(core::vector3df));

}

} // end IrrPhysx namespace
