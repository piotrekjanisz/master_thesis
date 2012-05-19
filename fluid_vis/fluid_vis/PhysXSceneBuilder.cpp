#include "PhysXSceneBuilder.h"


PhysXSceneBuilder::PhysXSceneBuilder(void)
{
}


PhysXSceneBuilder::~PhysXSceneBuilder(void)
{
}

void PhysXSceneBuilder::createStaticBox(NxScene* nxScene, const NxVec3& pos, const NxVec3& dim)
{
	NxBoxShapeDesc boxShape;
	boxShape.dimensions = dim;
	boxShape.skinWidth = 0.05f;

	NxActorDesc boxActor;
	boxActor.shapes.pushBack(&boxShape);
	boxActor.body = NULL;
	boxActor.globalPose.t = pos;

	nxScene->createActor(boxActor)->userData = (void*)0;
}


void PhysXSceneBuilder::createCube(NxScene* nxScene, const NxVec3& pos, float size, const NxVec3& initialVelocity)
{
	if (nxScene == NULL)
		return;
	
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping = 0.5f;
	bodyDesc.linearVelocity = initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3(size, size, size);
	boxDesc.skinWidth = 0.05f;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = 10.0f;
	actorDesc.globalPose.t = pos;
	nxScene->createActor(actorDesc)->userData = (void*)1;
}

void PhysXSceneBuilder::createCubeFromEye(NxScene* nxScene, const NxVec3& position, const NxVec3& velocity, float size)
{
	createCube(nxScene, position, size, velocity);
}