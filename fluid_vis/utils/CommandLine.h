#pragma once

#include <map>
#include <string>

class CommandLine
{
	std::map<std::string, std::string> _args;

public:
	CommandLine();
	void init(int argc, char** argv);
};

