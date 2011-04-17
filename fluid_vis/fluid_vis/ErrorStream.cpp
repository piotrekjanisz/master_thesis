#include "ErrorStream.h"
#include <iostream>

using namespace std;

ErrorStream::ErrorStream(void)
{
}


ErrorStream::~ErrorStream(void)
{
}

void ErrorStream::reportError(NxErrorCode code, const char *message, const char *file, int line)
{
	cerr << "ERROR:" << endl;
	cerr << "\tcode: " << code << endl;
	cerr << "\tmsg:  " << message << endl;
	cerr << "\tfile: " << file << endl;
	cerr << "\tline: " << line << endl;
}

NxAssertResponse ErrorStream::reportAssertViolation(const char *message, const char *file, int line)
{
	cerr << "ASSERTION VIOLATION: " << file << ", " << line << endl;
	return NX_AR_CONTINUE;
}

void ErrorStream::print(const char *message)
{
	cerr << "ERROR: " << message << endl;
}
