#include "PhysxConfigurationFactory.h"

#include <iostream>
#include <boost/make_shared.hpp>
#include <vmmlib\vmmlib.hpp>

using namespace std;
using namespace boost::filesystem;


PhysxConfigurationFactory::PhysxConfigurationFactory(const std::string& configurationRoot) throw(PhysxConfigurationException)
	: _configurationRoot(configurationRoot) 
{
	_configurationRoot = absolute(_configurationRoot).normalize();
	if (!exists(_configurationRoot)) {
		throw PhysxConfigurationException(_configurationRoot.string() + " doesn't exist");
	} else if (!is_directory(_configurationRoot)) {
		throw PhysxConfigurationException(_configurationRoot.string() + " is not a directory");
	}
}


PhysxConfigurationFactory::~PhysxConfigurationFactory(void)
{
}

string PhysxConfigurationFactory::getConfigFilePath(const string name) throw(PhysxConfigurationException)
{
	path configPath = _configurationRoot;
	configPath /= name;
	configPath = absolute(configPath.replace_extension(".cfg")).normalize();

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

	if (properties->hasKeyOfType<int>("maxParticles"))
		retVal.maxParticles = properties->get<float>("maxParticles");

	if (properties->hasKeyOfType<float>("kernelRadiusMultiplier"))
		retVal.kernelRadiusMultiplier = properties->get<float>("kernelRadiusMultiplier");

	if (properties->hasKeyOfType<float>("restParticlesPerMeter"))
		retVal.restParticlesPerMeter = properties->get<float>("restParticlesPerMeter");

	if (properties->hasKeyOfType<float>("restDensity"))
		retVal.restDensity = properties->get<float>("restDensity");

	if (properties->hasKeyOfType<float>("viscosity"))
		retVal.viscosity = properties->get<float>("viscosity");

	if (properties->hasKeyOfType<float>("stiffness"))
		retVal.stiffness = properties->get<float>("stiffness");

	if (properties->hasKeyOfType<float>("damping"))
		retVal.damping = properties->get<float>("damping");

	if (properties->hasKeyOfType<float>("surfaceTension"))
		retVal.surfaceTension = properties->get<float>("surfaceTension");

	if (properties->hasKeyOfType<float>("collisionDistanceMultiplier"))
		retVal.collisionDistanceMultiplier = properties->get<float>("collisionDistanceMultiplier");

	if (properties->hasKeyOfType<float>("dynamicFrictionForStaticShapes"))
		retVal.dynamicFrictionForStaticShapes = properties->get<float>("dynamicFrictionForStaticShapes");

	if (properties->hasKeyOfType<float>("restitutionForStaticShapes"))
		retVal.restitutionForStaticShapes = properties->get<float>("restitutionForStaticShapes");

	if (properties->hasKeyOfType<float>("motionLimitMultiplier"))
		retVal.motionLimitMultiplier = properties->get<float>("motionLimitMultiplier");

	if (properties->hasKeyOfType<int>("packetSizeMultiplier"))
		retVal.packetSizeMultiplier = properties->get<int>("packetSizeMultiplier");

	if (properties->hasKeyOfType<vmml::vec3f>("externalAcceleration"))
		retVal.externalAcceleration = NxVec3(properties->get<vmml::vec3f>("externalAcceleration").array);

	return retVal;
}


NxBodyDesc PhysxConfigurationFactory::createBodyDesc(const std::string& name) throw(PhysxConfigurationException)
{
	NxBodyDesc retVal;
	
	// TODO

	return retVal;
}
