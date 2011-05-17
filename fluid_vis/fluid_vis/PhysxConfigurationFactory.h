#pragma once

#include "BaseException.h"
#include "Properties.h"
#include <NxPhysics.h>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

class PhysxConfigurationException : public BaseException
{
public:
	PhysxConfigurationException(const std::string& msg)
		: BaseException(msg) {}
};

class PhysxConfigurationFactory
{
	boost::filesystem::path _configurationRoot;
	std::map<std::string, boost::shared_ptr<Properties> > _descriptions;
	std::map<std::string, unsigned int> _stringToConstant;

	void initStringToConstant();

	std::string getConfigFilePath(const std::string name) throw(PhysxConfigurationException);
	boost::shared_ptr<Properties> getProperties(const std::string& name) throw(PhysxConfigurationException);

public:
	PhysxConfigurationFactory(const std::string& configurationRoot) throw(PhysxConfigurationException);
	~PhysxConfigurationFactory();

	NxFluidDesc createFluidDesc(const std::string& name) throw(PhysxConfigurationException);
	NxFluidEmitterDesc createFluidEmitterDesc(const std::string& name) throw(PhysxConfigurationException);
	NxBodyDesc createBodyDesc(const std::string& name) throw(PhysxConfigurationException);
};

