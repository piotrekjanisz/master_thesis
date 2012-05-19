#include <NxCooking.h>
#include "BasinSceneBuilder.h"
#include "MyFluid.h"

BasinSceneBuilder::BasinSceneBuilder(void)
{
}


BasinSceneBuilder::~BasinSceneBuilder(void)
{
}


void BasinSceneBuilder::buildScene(NxScene* nxScene, ConfigurationFactory& configurationFactory, bool accelerateFluid)
{
	try {
		NxMaterial* defaultMaterial = nxScene->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(0.0f);
		defaultMaterial->setStaticFriction(0.5f);
		defaultMaterial->setDynamicFriction(0.5f);

		NxPlaneShapeDesc planeDesc;
		planeDesc.skinWidth = 0.05f;
		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&planeDesc);
		nxScene->createActor(actorDesc);

		// create basin for water
		createStaticBox(nxScene, NxVec3(2.0f, 1.0f, 0.0f), NxVec3(0.2f, 1.0f, 2.0f));
		createStaticBox(nxScene, NxVec3(-2.0f, 1.0f, 0.0f), NxVec3(0.2f, 1.0f, 2.0f));
		createStaticBox(nxScene, NxVec3(0.0f, 1.0f, 2.0f), NxVec3(2.0f, 1.0f, 0.2f));
		createStaticBox(nxScene, NxVec3(0.0f, 1.0f, -2.0f), NxVec3(2.0f, 1.0f, 0.2f));

		// create drains
		NxPlaneShapeDesc drainDesc;
		drainDesc.shapeFlags |= NX_SF_FLUID_DRAIN;
		drainDesc.normal = NxVec3(1.0, 0.0, 0.0);
		drainDesc.d = -4;

		NxActorDesc drainActor;
		drainActor.shapes.push_back(&drainDesc);
		nxScene->createActor(drainActor);

		drainDesc.normal = NxVec3(-1.0, 0.0, 0.0);
		nxScene->createActor(drainActor);
	
		drainDesc.normal = NxVec3(0.0, 0.0, 1.0);
		nxScene->createActor(drainActor);

		drainDesc.normal = NxVec3(0.0, 0.0, -1.0);
		nxScene->createActor(drainActor);

		drainDesc.d = -9;
		drainDesc.normal = NxVec3(0.0, -1.0, 0.0);
		nxScene->createActor(drainActor);


		// Create fluid
		NxFluidDesc fluidDesc = configurationFactory.createFluidDesc("water1");
		if (accelerateFluid) {
			fluidDesc.flags |= NX_FF_HARDWARE;
		} else {
			fluidDesc.flags &= ~NX_FF_HARDWARE;
		}

		MyFluid* fluid = new MyFluid(nxScene, fluidDesc, NxVec3(1.0f, 0.0f, 0.0f), 100.0f);

		NxFluidEmitterDesc emitterDesc = configurationFactory.createFluidEmitterDesc("emitter1");

		float data[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 5.0f, 
			0.0f, 0.0f, 1.0f, -3.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data);
		fluid->createEmitter(emitterDesc);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	} catch (...) {
		std::cerr << "Strange error during fluid creation" << std::endl;
	}
}