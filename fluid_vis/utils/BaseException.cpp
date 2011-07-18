#include "BaseException.h"

using namespace Utils;

BaseException::BaseException(const std::string& msg)
	: _msg(msg)
{
}


BaseException::~BaseException(void)
{
}
