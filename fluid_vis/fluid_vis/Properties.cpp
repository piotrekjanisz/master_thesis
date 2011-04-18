#include "Properties.h"
#include <iostream>
#include <fstream>
#include <cstdlib>


const boost::regex Properties::__LINE_PATTERN("^\\s*(\\w+)\\s*=\\s*([a-zA-Z0-9_.-]+)\\s*$");
const boost::regex Properties::__INT_PATTERN("^-?\\d+i$");
const boost::regex Properties::__FLOAT_PATTERN("^-?\\d+(.\\d+)?f$");

Properties::Properties(void)
{
}

Properties::~Properties(void)
{
}

void Properties::processLine(const std::string& line, int lineNumber) throw(PropertiesException)
{
	boost::smatch match;
	if (boost::regex_match(line, match, __LINE_PATTERN)) {
		std::string key(match[1].first, match[1].second);
		std::string valueStr(match[2].first, match[2].second);
		if (boost::regex_match(valueStr, __FLOAT_PATTERN)) {
			_properties[key] = (float)atof(valueStr.c_str());
		} else if (boost::regex_match(valueStr, __INT_PATTERN)) {
			_properties[key] = (int) atoi(valueStr.c_str());
		} else {
			_properties[key] = valueStr;
		}
	} else {
		std::cerr << "WARN: line " << lineNumber << ": wrong format" << std::endl;
	}
}

void Properties::load(const std::string& file) throw(PropertiesException)
{
	std::fstream input(file.c_str(), std::fstream::in);
	if (!input.is_open()) {
		throw PropertiesException(std::string("Can't open file: ") + file);
	}

	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	int line = 1;
	while (!input.eof()) {
		input.getline(buffer, BUFFER_SIZE);
		if (input.gcount() == 0)
			break;
		processLine(buffer, line);
		line++;
	}
}

void Properties::printProperties()
{
	for (std::map<std::string, boost::any>::iterator it = _properties.begin(); it != _properties.end(); ++it) {
		std::cout << it->first << "=" << it->second.type().name() << std::endl;
	}
}

