#pragma once

#include <string>
#include <map>
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <typeinfo>
#include "BaseException.h"


class PropertiesException : public BaseException
{
public:
	PropertiesException(const std::string& message)
		: BaseException(message) {}
};

class ElementNotFoundException : public PropertiesException
{
public:
	ElementNotFoundException(const std::string& message)
		: PropertiesException(message) {}
};

class Properties
{
	static const boost::regex __LINE_PATTERN;
	static const boost::regex __INT_PATTERN;
	static const boost::regex __FLOAT_PATTERN;
	static const boost::regex __DOUBLE_PATTERN;
	static const boost::regex __STRING_PATTERN;
	static const boost::regex __VEC3_PATTERN;

	std::map<std::string, boost::any> _properties;
	
	void processLine(const std::string& fileName, const std::string& line, int lineNumber) throw(PropertiesException);
public:
	Properties(void);
	~Properties(void);

	void load(const std::string& file) throw(PropertiesException);

	bool hasKey(const std::string& key) { return _properties.count(key) != 0; }

	template <typename T>
	T get(const std::string& key) throw(ElementNotFoundException);

	template <typename T>
	inline bool hasKeyOfType(const std::string& key);

	void printProperties();
};

template <typename T>
T Properties::get(const std::string& key) throw(ElementNotFoundException)
{
	if (!hasKey(key))
		throw ElementNotFoundException(std::string("Element of key \"") + key + std::string("not found"));
	boost::any value = _properties[key];
	if (value.type() != typeid(T)) {
		throw ElementNotFoundException(std::string("Type doesn't match. Requested: ") + std::string(typeid(T).name()) + std::string(", got: ") + std::string(value.type().name()));
	}
	return boost::any_cast<T>(value);
}

template <typename T>
bool Properties::hasKeyOfType(const std::string& key)
{
	return hasKey(key) && _properties[key].type() == typeid(T);
}

