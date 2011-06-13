#pragma once

#include <NxPhysics.h>
#include <boost/shared_ptr.hpp>
#include <vmmlib/vmmlib.hpp>

class MyFluid
{
protected:
	static const unsigned COMPONENT_COUNT;

	virtual void simpleRender();
	float* _posBuffer;
	unsigned _particleCount;
	unsigned _maxParticles;
	NxFluid* _fluid;
	NxFluidEmitter* _fluidEmitter;

// constructors / destructors / assignment operators
public:
	MyFluid(NxScene* scene, NxFluidDesc& fluidDesc, const NxVec3& color, float particleSize);
	virtual ~MyFluid(void);

public:
	virtual void render(vmml::mat4f& projectionMatrix, vmml::mat4f& modelViewMatrix);
	void render();

	// TODO throw exception
	void createEmitter(NxFluidEmitterDesc& emitterDesc);

	int getEmitedCount() 
	{
		return _fluidEmitter == NULL ? 0 : _fluidEmitter->getNbParticlesEmitted();
	}
};

