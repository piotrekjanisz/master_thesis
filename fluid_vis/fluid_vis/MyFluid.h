#pragma once

#include <NxPhysics.h>
#include <boost/shared_ptr.hpp>
#include <vmmlib/vmmlib.hpp>
#include <cstring>

class MyFluid
{
protected:
	static const unsigned COMPONENT_COUNT;

	virtual void simpleRender();
	float* _posBuffer;
	float* _currentPosBuffer;
	unsigned _currentPartCount;
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

	float* getPositions()
	{
		return _posBuffer;
	}

	float* getPositionsAsync() 
	{ 
		memcpy(_currentPosBuffer, _posBuffer, _particleCount*4*sizeof(float));
		_currentPartCount = _particleCount;
		return _currentPosBuffer; 
	}

	float* getDummyPositions()
	{
		float pos[4] = {0.0f, 3.0f, 0.0f, 0.0f};
		_currentPartCount = 1;
		memcpy(_currentPosBuffer, pos, _currentPartCount * 4 * sizeof(float));
		return _currentPosBuffer;
	}

	int getParticlesCount() const { return _particleCount; }

	int getParticlesCountAsync() const { return _currentPartCount; }
};

