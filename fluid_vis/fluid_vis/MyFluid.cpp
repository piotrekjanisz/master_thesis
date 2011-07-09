#include <iostream>
#include "MyFluid.h"

using namespace std;

const unsigned MyFluid::COMPONENT_COUNT = 4;

MyFluid::MyFluid(NxScene* scene, NxFluidDesc& fluidDesc, const NxVec3& color, float particleSize)
	: _posBuffer(NULL)
{
	_maxParticles = fluidDesc.maxParticles;
	_posBuffer = new float[_maxParticles * COMPONENT_COUNT];
	_currentPosBuffer = new float[_maxParticles * COMPONENT_COUNT];
	for (int i = 0; i < _maxParticles; i++) {
		_posBuffer[i * COMPONENT_COUNT + COMPONENT_COUNT - 1] = 1.0f;
	}

	NxParticleData particleData;
	particleData.numParticlesPtr = &_particleCount;
	particleData.bufferPos = _posBuffer;
	particleData.bufferPosByteStride = COMPONENT_COUNT * sizeof(float);

	fluidDesc.userData = this;
	fluidDesc.particlesWriteData = particleData;
	_fluid = scene->createFluid(fluidDesc);
	if (_fluid == NULL) {
		// TODO throw exception
		cerr << "Fluid initialization failed" << endl;
	}
}


MyFluid::~MyFluid(void)
{
	if (_posBuffer) {
		delete[] _posBuffer;
		_posBuffer = NULL;
	}
	_fluid->getScene().releaseFluid(*_fluid);
}


void MyFluid::simpleRender()
{
	// TODO
}

void MyFluid::render(vmml::mat4f& projectionMatrix, vmml::mat4f& modelViewMatrix)
{
	simpleRender();
}

void MyFluid::createEmitter(NxFluidEmitterDesc& emitterDesc)
{
	_fluidEmitter = _fluid->createEmitter(emitterDesc);
}

void MyFluid::render()
{
}
