#include "BaseConfigurationFactory.h"
#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>

using namespace Utils;
using namespace boost::filesystem;

BaseConfigurationFactory::BaseConfigurationFactory(const std::string& configurationRoot)
	: _configurationRoot(configurationRoot)
{
	_configurationRoot = absolute(_configurationRoot).normalize();
	if (!exists(_configurationRoot)) {
		throw ConfigurationFactoryException(_configurationRoot.string() + " doesn't exist");
	} else if (!is_directory(_configurationRoot)) {
		throw ConfigurationFactoryException(_configurationRoot.string() + " is not a directory");
	}
}

BaseConfigurationFactory::~BaseConfigurationFactory()
{
}

std::string BaseConfigurationFactory::getConfigFilePath(const std::string& name) throw(ConfigurationFactoryException)
{
	path configPath = _configurationRoot;
	configPath /= (name + ".cfg");
	configPath = absolute(configPath).normalize();

	if (!exists(configPath)) {
		throw ConfigurationFactoryException(configPath.string() + " doesn't exist");
	}

	return configPath.string();
}

boost::shared_ptr<Properties> BaseConfigurationFactory::getProperties(const std::string& name) throw(ConfigurationFactoryException)
{
	if (_descriptions.count(name) == 0) {
		boost::shared_ptr<Properties> prop = boost::make_shared<Properties>();

		try {
			prop->load(getConfigFilePath(name));
		} catch (PropertiesException& ex) {
			throw ConfigurationFactoryException(ex.what());
		}
		_descriptions[name] = prop;
	}

	return _descriptions[name];
}