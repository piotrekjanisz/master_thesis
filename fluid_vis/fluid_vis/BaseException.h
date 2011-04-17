#pragma once

#include <exception>
#include <string>

class BaseException : public std::exception
{
	std::string _msg;
public:
	BaseException(const std::string& msg);
	virtual ~BaseException(void);

	virtual const char* what() const throw() { return _msg.c_str(); }
};
