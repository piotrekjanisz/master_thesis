#include "ConfigurationFactory.h"

#include <iostream>
#include <boost/make_shared.hpp>
#include <vmmlib\vmmlib.hpp>

using namespace std;
using namespace boost::filesystem;

ConfigurationFactory::ConfigurationFactory(const std::string& configurationRoot) throw(Utils::ConfigurationFactoryException)
	:BaseConfigurationFactory(configurationRoot)
{
	_stringToConstant["NX_FE_CONSTANT_FLOW_RATE"] = NX_FE_CONSTANT_FLOW_RATE;
	_stringToConstant["NX_FE_CONSTANT_PRESSURE"] = NX_FE_CONSTANT_PRESSURE;
	_stringToConstant["NX_FE_RECTANGULAR"] = NX_FE_RECTANGULAR;
	_stringToConstant["NX_FE_ELLIPSE"] = NX_FE_ELLIPSE;
	_stringToConstant["NX_F_SPH"] = NX_F_SPH;
	_stringToConstant["NX_F_MIXED_MODE"] = NX_F_MIXED_MODE;
	_stringToConstant["NX_F_NO_PARTICLE_INTERACTION"] = NX_F_NO_PARTICLE_INTERACTION;
}

ConfigurationFactory::~ConfigurationFactory()
{
}
