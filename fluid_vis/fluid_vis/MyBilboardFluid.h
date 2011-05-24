#pragma once

#include "myfluid.h"
#include "GfxObject.h"
#include <boost/shared_ptr.hpp>

class MyBilboardFluid :
	public MyFluid
{
	boost::shared_ptr<GfxObject> _gfxObject;

public:
	MyBilboardFluid(NxScene* scene, NxFluidDesc& fluidDesc, const NxVec3& color, float particleSize, const boost::shared_ptr<GfxObject>& gfxObject);
	~MyBilboardFluid(void);

	virtual void render(vmml::mat4f& projectionMatrix, vmml::mat4f& modelViewMatrix);
};

