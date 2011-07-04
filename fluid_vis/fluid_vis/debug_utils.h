/**
 * Debugging utities
 * 
 * If DEBUG macro is defined code inside DEBUG_CODE and DEBUG_COUT will be exectued
 * otherwise code from those macros will be discarded
 *
 * Sometimes it's usefull to disable/enable debugging code only in one source file.
 * To do that just add #undef DEBUG or #define DEBUG before including this file.
 * 
 */

#ifndef DEBUG_UTILS
#define DEBUG_UTILS

#include <iostream>
#include <GL/glew.h>
#include <string>

namespace DebugUtils
{
	template <typename T> 
	void printArray(T* array, int size, int componentNum);
	std::string glErrorToString(GLenum error);
};

template <typename T> 
void DebugUtils::printArray(T* array, int size, int componentNum)
{
	for (int i = 0; i < size; i++) {
		std::cout << array[i*componentNum + 0];
		for (int j = 1; j < componentNum; j++)
			std::cout << ", " << array[i*componentNum + j];
		std::cout << std::endl;
	}
}

#endif



#ifdef DEBUG
#define DEBUG_CODE(code) (code)
#define DEBUG_COUT(code) (std::cout code)
#define CHECK_GL_CMD(cmd) \
	while (glGetError() != GL_NO_ERROR) { } \
	cmd; \
	{ \
		GLenum error = glGetError(); \
		if (error != GL_NO_ERROR) { \
			std::cerr << "ERROR: "  << std::endl; \
			std::cerr << "\tLOCATION : " << __FILE__ << ":" << __LINE__ << std::endl; \
			std::cerr << "\tCOMMAND  : \"" << #cmd << "\"" << std::endl; \
			std::cerr << "\tCODE     : " << DebugUtils::glErrorToString(error) << std::endl; \
		} \
	}
#define DEBUG_PRINT_VAR(var) \
	std::cout << #var << ": " << var << std::endl;

#else
#define DEBUG_CODE(code)
#define DEBUG_COUT(code)
#define CHECK_GL_CMD(cmd) cmd
#define DEBUG_PRINT_VAR(var)
#endif



