#pragma once

#include "Properties.h"
#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <vmmlib\vmmlib.hpp>
#include <iostream>

namespace Utils 
{


/**
 *    Macros for shortening the code in "create.." methods
 */

#define INT_MEMBER(memberName) \
	if (properties->hasKeyOfType<int>(#memberName)) \
		retVal.memberName = properties->get<int>(#memberName)

#define FLOAT_MEMBER(memberName) \
	if (properties->hasKeyOfType<float>(#memberName)) \
		retVal.memberName = properties->get<float>(#memberName)

#define DOUBLE_MEMBER(memberName) \
	if (properties->hasKeyOfType<double>(#memberName)) \
		retVal.memberName = properties->get<double>(#memberName)

#define VEC3_MEMBER(memberName) \
	if (properties->hasKeyOfType<vmml::vec3f>(#memberName)) \
		retVal.memberName = properties->get<vmml::vec3f>(#memberName)

#define NXVEC3_MEMBER(memberName) \
	if (properties->hasKeyOfType<vmml::vec3f>(#memberName)) \
		retVal.memberName = NxVec3(properties->get<vmml::vec3f>(#memberName))

#define ENUM_MEMBER(memberName) \
	if (properties->hasKeyOfType<std::string>(#memberName)) { \
		std::string str = properties->get<std::string>(#memberName); \
		if (_stringToConstant.count(str) == 0) { \
			std::cerr << "WARNING: unrecognized value of " #memberName << str << std::endl; \
		} else { \
			retVal.memberName = _stringToConstant[str]; \
		} \
	}

#define AUTO_GET_START(structName) \
	structName create ## structName(const std::string& resName) \
	{ \
		structName retVal; \
		PropertiesPtr properties = getProperties(resName);

#define AUTO_GET_START_NAME(structName, methodName) \
	structName methodName(const std::string& resName) \
	{ \
		structName retVal; \
		PropertiesPtr properties = getProperties(resName);

#define AUTO_GET_END() return retVal; }

class ConfigurationFactoryException : public BaseException
{
public:
	ConfigurationFactoryException(const std::string& msg)
		: BaseException(msg) {}
};

class BaseConfigurationFactory
{
public:
	typedef boost::shared_ptr<Properties> PropertiesPtr;

private:
	boost::filesystem::path _configurationRoot;
	std::map<std::string, PropertiesPtr > _descriptions;

protected:
	PropertiesPtr getProperties(const std::string& name) throw(ConfigurationFactoryException);
	std::string getConfigFilePath(const std::string& name) throw(ConfigurationFactoryException);\
	std::map<std::string, unsigned int> _stringToConstant;

public:
	BaseConfigurationFactory(const std::string& configurationRoot) throw(ConfigurationFactoryException);
	~BaseConfigurationFactory();
}; // class BaseConfigurationFactory


}; // namespace Utils

