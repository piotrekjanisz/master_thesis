#include "physx_utils.h"
#include <string>
using namespace std;

namespace PhysXUtils
{
	std::string getErrorString(NxSDKCreateError error)
	{
		switch (error) {
		case NXCE_NO_ERROR:
			return string("no error");
		case NXCE_PHYSX_NOT_FOUND:
			return string("Unable to find the PhysX libraries. The PhysX drivers are not installed correctly.");
		case NXCE_WRONG_VERSION:
			return string("The application supplied a version number that does not match with the libraries.");
		case NXCE_DESCRIPTOR_INVALID:
			return string("The supplied SDK descriptor is invalid.");
		case NXCE_CONNECTION_ERROR:
			return string("A PhysX card was found, but there are problems when communicating with the card.");
		case NXCE_RESET_ERROR:
			return string("A PhysX card was found, but it did not reset (or initialize) properly.");
		case NXCE_IN_USE_ERROR:
			return string("A PhysX card was found, but it is already in use by another application.");
		case NXCE_BUNDLE_ERROR:
			return string("A PhysX card was found, but there are issues with loading the firmware.");
		}
	}
};