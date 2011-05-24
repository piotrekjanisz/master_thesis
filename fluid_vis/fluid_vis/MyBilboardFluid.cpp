#include "MyBilboardFluid.h"
#include <iostream>


MyBilboardFluid::MyBilboardFluid(NxScene* scene, NxFluidDesc& fluidDesc, const NxVec3& color, float particleSize, const boost::shared_ptr<GfxObject>& gfxObject)
	:MyFluid(scene, fluidDesc, color, particleSize), _gfxObject(gfxObject)
{
	_gfxObject->addAttribute("vertex", NULL, _maxParticles, 4, DYNAMIC_ATTR);
	_gfxObject->addUniformMat4f("projectionMatrix");
	_gfxObject->addUniformMat4f("modelViewMatrix");
}


MyBilboardFluid::~MyBilboardFluid(void)
{
}

void MyBilboardFluid::render(vmml::mat4f& projectionMatrix, vmml::mat4f& modelViewMatrix) {
	_gfxObject->updateUniformMat4f("modelViewMatrix", modelViewMatrix);
	_gfxObject->updateUniformMat4f("projectionMatrix", projectionMatrix);
	_gfxObject->updateAttribute("vertex", _posBuffer, _particleCount);
	_gfxObject->render(_particleCount, GL_POINTS);
}
