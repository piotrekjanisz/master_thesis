#pragma once

#include "BaseException.h"
#include "Properties.h"
#include "ScreenSpaceParticleRenderer.h"
#include <surface_extraction\data_types.h>
#include <utils/BaseConfigurationFactory.h>
#include <NxPhysics.h>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

class ConfigurationFactory : public Utils::BaseConfigurationFactory
{
public:
	ConfigurationFactory(const std::string& configurationRoot) throw(Utils::ConfigurationFactoryException);
	~ConfigurationFactory();


	AUTO_GET_START_NAME(NxFluidDesc, createFluidDesc)
		INT_MEMBER(maxParticles);
		FLOAT_MEMBER(kernelRadiusMultiplier);
		FLOAT_MEMBER(restParticlesPerMeter);
		FLOAT_MEMBER(restDensity);
		FLOAT_MEMBER(viscosity);
		FLOAT_MEMBER(stiffness);
		FLOAT_MEMBER(damping);
		FLOAT_MEMBER(surfaceTension);
		FLOAT_MEMBER(collisionDistanceMultiplier);
		FLOAT_MEMBER(dynamicFrictionForStaticShapes);
		FLOAT_MEMBER(restitutionForStaticShapes);
		FLOAT_MEMBER(motionLimitMultiplier);
		INT_MEMBER(packetSizeMultiplier);
		NXVEC3_MEMBER(externalAcceleration);
		ENUM_MEMBER(simulationMethod);
	AUTO_GET_END()

	AUTO_GET_START_NAME(NxFluidEmitterDesc, createFluidEmitterDesc)
		INT_MEMBER(maxParticles);
		FLOAT_MEMBER(dimensionX);
		FLOAT_MEMBER(dimensionY);
		ENUM_MEMBER(type);
		FLOAT_MEMBER(rate);
		FLOAT_MEMBER(fluidVelocityMagnitude);
		FLOAT_MEMBER(particleLifetime);
		ENUM_MEMBER(shape);
		FLOAT_MEMBER(repulsionCoefficient);
		NXVEC3_MEMBER(randomPos);
		FLOAT_MEMBER(randomAngle);
	AUTO_GET_END()

	AUTO_GET_START_NAME(SurfaceExtractorDesc, createSurfaceExtractorDesc)
		DOUBLE_MEMBER(xMin);
		DOUBLE_MEMBER(xMax);
		DOUBLE_MEMBER(yMin);
		DOUBLE_MEMBER(yMax);
		DOUBLE_MEMBER(zMin);
		DOUBLE_MEMBER(zMax);
		DOUBLE_MEMBER(rc);
		DOUBLE_MEMBER(isoTreshold);
		DOUBLE_MEMBER(blockSize);
		DOUBLE_MEMBER(cubeSize);
		INT_MEMBER(maxParticles);
		INT_MEMBER(maxTrianglesPerThread);
		INT_MEMBER(maxVerticesPerThread);
		INT_MEMBER(threads);
	AUTO_GET_END()

	AUTO_GET_START_NAME(ScreenSpaceRendererDesc, createScreenSpaceRendererDesc)
		FLOAT_MEMBER(particleSize);
		FLOAT_MEMBER(particleThickness);
		FLOAT_MEMBER(thicknessGaussSigma);
		INT_MEMBER(thicknessGaussSize);
		FLOAT_MEMBER(thicknessTextureSize);

		FLOAT_MEMBER(minDensity);
		FLOAT_MEMBER(normalDensity);
		FLOAT_MEMBER(maxParticleSize);
		INT_MEMBER(particleThicknessExp);
		FLOAT_MEMBER(refractionMult);

		FLOAT_MEMBER(timeStep);
		INT_MEMBER(blurIterations);
		FLOAT_MEMBER(edgeTreshold);
	AUTO_GET_END()
};

