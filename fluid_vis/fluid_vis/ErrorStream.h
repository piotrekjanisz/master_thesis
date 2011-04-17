#pragma once
#include <NxUserOutputStream.h>

class ErrorStream :
	public NxUserOutputStream
{
public:
	ErrorStream(void);
	virtual ~ErrorStream(void);

	virtual void reportError(NxErrorCode code, const char *message, const char *file, int line);
	virtual NxAssertResponse reportAssertViolation (const char *message, const char *file, int line); 

	virtual void print (const char *message);
};

