#include "Properties.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vmmlib/vector.hpp>
#include <boost/filesystem.hpp>


using namespace std;

const boost::regex Properties::__LINE_PATTERN("^\\s*(\\w+)\\s*=\\s*((\\S+)|(\"[^\"]*\"))\\s*$");
const boost::regex Properties::__INT_PATTERN("^-?\\d+i$");
const boost::regex Properties::__FLOAT_PATTERN("^-?\\d+(.\\d+)?f$");
const boost::regex Properties::__DOUBLE_PATTERN("^-?\\d+(.\\d+)?d$");
const boost::regex Properties::__STRING_PATTERN("^\"([^\"]*)\"$");
const boost::regex Properties::__VEC3_PATTERN("^\\((-?\\d+(.\\d+)?)\\s*,\\s*(-?\\d+(.\\d+)?)\\s*,\\s*(-?\\d+(.\\d+)?)\\)$");


char* substr(const char* str, int start, int end){
        char* a = new char[1+(end-start)]; // we need a new char array for the return
        for(int i=start; i<end; i++){ // loop through the string
                a[i-start] = str[i]; // set the characters in the new char array to those from the old one compensating for the substring
        }
        a[end-start] = '\0'; // add the null character, so we can output
        return a; // return
}
 
double atod(const char* a){
        double retVal = atoi(a); // start off getting the number, assuming it is a valid string to use atoi on.
        int start = 0;
        int end = 0;
        for(int i=0; a[i] != '\0'; i++){ // loop through the string to find the positions of the decimal portion, if there is one
                if(a[i] == '.' && start == 0){
                        start = i+1; // set the start position to 1 more than the current (avoids a char as the first character - we want a digit)
                }
                else if(start != 0 &&  (a[i] < '0' || a[i] > '9')){ // make sure that start is set and that we aren't looking at digits
                        end = i; // if so, set the end location for the substring
                        break; // we don't need to continue anymore - break out of the loop
                }
        }
        if(end > start){ // avoids substring problems.
                char* decimal = substr(a, start, end); // get the string that is after the decimal point
                int dec = atoi(decimal); // make it an integer
                int power = end-start; // find the power of 10 we need to divide by
                retVal += ((double)dec)/(pow(10.0, (double)power)); // divide and add to the return value (thus making it a true double)
        }
        return retVal; // return - simple enough
}

Properties::Properties(void)
{
}

Properties::~Properties(void)
{
}

void Properties::processLine(const std::string& fileName, const std::string& line, int lineNumber) throw(PropertiesException)
{
	boost::smatch match;
	if (boost::regex_match(line, match, __LINE_PATTERN)) {
		std::string key(match[1].first, match[1].second);
		std::string valueStr(match[2].first, match[2].second);
		if (boost::regex_match(valueStr, __FLOAT_PATTERN)) {
			_properties[key] = (float)atof(valueStr.c_str());
		} else if (boost::regex_match(valueStr, __DOUBLE_PATTERN)) {
			_properties[key] = (double) atod(valueStr.c_str());
		} else if (boost::regex_match(valueStr, __INT_PATTERN)) {
			_properties[key] = (int) atoi(valueStr.c_str());
		} else if (boost::regex_match(valueStr, __STRING_PATTERN)) {
			_properties[key] = valueStr.substr(1, valueStr.size() - 2);
		} else if (boost::regex_match(valueStr, match, __VEC3_PATTERN)) {
			vmml::vec3f v;
			for (int i = 0 ; i < 3; i++) {
				std::string component(match[2*i + 1].first, match[2*i + 1].second);
				v[i] = (float)atof(component.c_str());
			}
			_properties[key] = v;
		} else {
			std::cerr << "WARN: " << fileName << ":" << lineNumber << ": wrong format: " << std::endl;
			std::cerr << "\t>>" << valueStr << "<<" << std::endl;
		}
	} else {
		std::cerr << "WARN: " << fileName << ":" << lineNumber << ": wrong format" << std::endl;
	}
}

void Properties::load(const std::string& file) throw(PropertiesException)
{
	_properties.clear();
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
		processLine(file, buffer, line);
		line++;
	}
}

void Properties::printProperties()
{
	for (std::map<std::string, boost::any>::iterator it = _properties.begin(); it != _properties.end(); ++it) {
		std::cout << it->first << "=" << it->second.type().name() << std::endl;
	}
}

