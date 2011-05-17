#pragma once

#include <NxPhysics.h>
#include <boost/shared_ptr.hpp>

class MyFluid
{
private:
	static const unsigned COMPONENT_COUNT;

	void simpleRender();
	float* _posBuffer;
	unsigned _particleCount;
	unsigned _maxParticles;
	NxFluid* _fluid;

// constructors / destructors / assignment operators
public:
	MyFluid(NxScene* scene, NxFluidDesc& fluidDesc, const NxVec3& color, float particleSize);
	~MyFluid(void);

public:
	void render();
};

