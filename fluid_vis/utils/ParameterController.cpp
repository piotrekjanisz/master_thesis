#include "ParameterController.h"

using namespace std;

typedef std::set<string>::iterator string_set_it;
typedef std::map<std::string, std::pair<unsigned int, unsigned int>>::iterator param_mappings_iterator;

ParameterController::ParameterController(const vector<pair<unsigned int, unsigned int>>& keys, ParametrizedPtr parametrized)
	: _keys(keys), _parametrized(parametrized)
{
	for (string_set_it it = _parametrized->getParameters().begin(); it != _parametrized->getParameters().end(); ++it) {
		std::pair<unsigned int, unsigned int> keyPair = _keys.back(); 
		_keys.pop_back();

		_paramKeyMappings[*it] = keyPair;
		
		_keyMappings[keyPair.first] = pair<string, ParamOperation>(*it, ParamOperation::DEC);
		_keyMappings[keyPair.second] = pair<string, ParamOperation>(*it, ParamOperation::INC);
	}
}


ParameterController::~ParameterController(void)
{
}

void ParameterController::printMappings()
{
	for (param_mappings_iterator it = _paramKeyMappings.begin(); it != _paramKeyMappings.end(); ++it) {
		cout << it->first << ": " << endl << "\t" << (char)it->second.first << " " << (char)it->second.second << endl;
	}
}

void ParameterController::keyPressed(unsigned int key)
{
	if (_keyMappings.count(key) > 0) {
		_parametrized->changeParameter(_keyMappings[key].first, _keyMappings[key].second);
	}
}
