#pragma once

#include <set>
#include <map>
#include <string>
#include <vector>
#include <boost/smart_ptr.hpp>

enum ParamOperation
{
	INC,
	DEC
};

class Parametrized 
{
public:
	virtual const std::set<std::string>& getParameters() = 0;
	virtual bool changeParameter(const std::string& parameter, ParamOperation operation) = 0;
};


typedef Parametrized* ParametrizedPtr;

class ParameterController
{
private:
	ParametrizedPtr _parametrized;
	std::map<unsigned int, std::pair<std::string, ParamOperation>> _keyMappings;
	std::map<std::string, std::pair<unsigned int, unsigned int>> _paramKeyMappings;
	std::vector<std::pair<unsigned int, unsigned int>> _keys;

public:
	ParameterController(const std::vector<std::pair<unsigned int, unsigned int>>& keys, ParametrizedPtr parametrized);
	~ParameterController(void);

	void printMappings();
	void keyPressed(unsigned int key);
};

typedef boost::shared_ptr<ParameterController> ParameterControllerPtr;

