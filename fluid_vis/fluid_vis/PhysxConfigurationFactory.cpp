#include "PhysxConfigurationFactory.h"

#include <iostream>
#include <boost/make_shared.hpp>
#include <vmmlib\vmmlib.hpp>

using namespace std;
using namespace boost::filesystem;


/**
 *    Macros for shortening the code in "create.." methods
 */
#define SET_FLOAT(data, properties, propertyName) \
	if (properties->hasKeyOfType<float>(#propertyName)) \
		data.propertyName = properties->get<float>(#propertyName)

#define SET_DOUBLE(data, properties, propertyName) \
	if (properties->hasKeyOfType<double>(#propertyName)) \
		data.propertyName = properties->get<double>(#propertyName)

#define SET_INT(data, properties, propertyName) \
	if (properties->hasKeyOfType<int>(#propertyName)) \
		data.propertyName = properties->get<int>(#propertyName)

#define SET_VEC3(data, properties, propertyName) \
	if (properties->hasKeyOfType<vmml::vec3f>(#propertyName)) \
		data.propertyName = NxVec3(properties->get<vmml::vec3f>(#propertyName).array)

#define SET_UINT_CONST(data, properties, propertyName, stringToConstant) \
	if (properties->hasKeyOfType<std::string>(#propertyName)) { \
		std::string str = properties->get<std::string>(#propertyName); \
		if (stringToConstant.count(str) == 0) { \
			cerr << "WARNING: unrecognized value of " #propertyName " in " #data << str << endl; \
		} else { \
			data.propertyName = stringToConstant[str]; \
		} \
	}

PhysxConfigurationFactory::PhysxConfigurationFactory(const std::string& configurationRoot) throw(PhysxConfigurationException)
	: _configurationRoot(configurationRoot) 
{
	_configurationRoot = absolute(_configurationRoot).normalize();
	if (!exists(_configurationRoot)) {
		throw PhysxConfigurationException(_configurationRoot.string() + " doesn't exist");
	} else if (!is_directory(_configurationRoot)) {
		throw PhysxConfigurationException(_configurationRoot.string() + " is not a directory");
	}
	initStringToConstant();
}


PhysxConfigurationFactory::~PhysxConfigurationFactory(void)
{
}

void PhysxConfigurationFactory::initStringToConstant()
{
	_stringToConstant["NX_FE_CONSTANT_FLOW_RATE"] = NX_FE_CONSTANT_FLOW_RATE;
	_stringToConstant["NX_FE_CONSTANT_PRESSURE"] = NX_FE_CONSTANT_PRESSURE;
	_stringToConstant["NX_FE_RECTANGULAR"] = NX_FE_RECTANGULAR;
	_stringToConstant["NX_FE_ELLIPSE"] = NX_FE_ELLIPSE;
	_stringToConstant["NX_F_SPH"] = NX_F_SPH;
	_stringToConstant["NX_F_MIXED_MODE"] = NX_F_MIXED_MODE;
	_stringToConstant["NX_F_NO_PARTICLE_INTERACTION"] = NX_F_NO_PARTICLE_INTERACTION;
}

string PhysxConfigurationFactory::getConfigFilePath(const string name) throw(PhysxConfigurationException)
{
	path configPath = _configurationRoot;
	configPath /= (name + ".cfg");
	configPath = absolute(configPath).normalize();

	if (!exists(configPath)) {
		throw PhysxConfigurationException(configPath.string() + " doesn't exist");
	}

	return configPath.string();
}

boost::shared_ptr<Properties> PhysxConfigurationFactory::getProperties(const std::string& name) throw(PhysxConfigurationException)
{
	if (_descriptions.count(name) == 0) {
		boost::shared_ptr<Properties> prop = boost::make_shared<Properties>();

		try {
			prop->load(getConfigFilePath(name));
		} catch (PropertiesException& ex) {
			throw PhysxConfigurationException(ex.what());
		}
		_descriptions[name] = prop;
	}

	return _descriptions[name];
}

NxFluidDesc PhysxConfigurationFactory::createFluidDesc(const std::string& name) throw(PhysxConfigurationException)
{
	NxFluidDesc retVal;

	boost::shared_ptr<Properties> properties = getProperties(name);

	SET_INT(retVal, properties, maxParticles);
	SET_FLOAT(retVal, properties, kernelRadiusMultiplier);
	SET_FLOAT(retVal, properties, restParticlesPerMeter);
	SET_FLOAT(retVal, properties, restDensity);
	SET_FLOAT(retVal, properties, viscosity);
	SET_FLOAT(retVal, properties, stiffness);
	SET_FLOAT(retVal, properties, damping);
	SET_FLOAT(retVal, properties, surfaceTension);
	SET_FLOAT(retVal, properties, collisionDistanceMultiplier);
	SET_FLOAT(retVal, properties, dynamicFrictionForStaticShapes);
	SET_FLOAT(retVal, properties, restitutionForStaticShapes);
	SET_FLOAT(retVal, properties, motionLimitMultiplier);
	SET_INT(retVal, properties, packetSizeMultiplier);
	SET_VEC3(retVal, properties, externalAcceleration);
	SET_UINT_CONST(retVal, properties, simulationMethod, _stringToConstant);

	return retVal;
}

NxFluidEmitterDesc PhysxConfigurationFactory::createFluidEmitterDesc(const std::string& name) throw(PhysxConfigurationException)
{
	NxFluidEmitterDesc retVal;
	boost::shared_ptr<Properties> properties = getProperties(name);

	SET_INT(retVal, properties, maxParticles);
	SET_FLOAT(retVal, properties, dimensionX);
	SET_FLOAT(retVal, properties, dimensionY);
	SET_UINT_CONST(retVal, properties, type, _stringToConstant);
	SET_FLOAT(retVal, properties, rate);
	SET_FLOAT(retVal, properties, fluidVelocityMagnitude);
	SET_FLOAT(retVal, properties, particleLifetime);
	SET_UINT_CONST(retVal, properties, shape, _stringToConstant);
	SET_FLOAT(retVal, properties, repulsionCoefficient);
	SET_VEC3(retVal, properties, randomPos);
	SET_FLOAT(retVal, properties, randomAngle);

	return retVal;
}


NxBodyDesc PhysxConfigurationFactory::createBodyDesc(const std::string& name) throw(PhysxConfigurationException)
{
	NxBodyDesc retVal;
	
	// TODO

	return retVal;
}

SurfaceExtractorDesc PhysxConfigurationFactory::createSurfaceExtractorDesc(const std::string& name) throw(PhysxConfigurationException)
{
	SurfaceExtractorDesc retVal;
	boost::shared_ptr<Properties> properties = getProperties(name);

	SET_DOUBLE(retVal, properties, xMin);
	SET_DOUBLE(retVal, properties, xMax);
	SET_DOUBLE(retVal, properties, yMin);
	SET_DOUBLE(retVal, properties, yMax);
	SET_DOUBLE(retVal, properties, zMin);
	SET_DOUBLE(retVal, properties, zMax);
	
	SET_DOUBLE(retVal, properties, rc);
	SET_DOUBLE(retVal, properties, isoTreshold);

	SET_DOUBLE(retVal, properties, blockSize);
	SET_DOUBLE(retVal, properties, cubeSize);

	SET_INT(retVal, properties, maxParticles);

	SET_INT(retVal, properties, maxTrianglesPerThread);
	SET_INT(retVal, properties, maxVerticesPerThread);

	SET_INT(retVal, properties, threads);

	return retVal;
}
