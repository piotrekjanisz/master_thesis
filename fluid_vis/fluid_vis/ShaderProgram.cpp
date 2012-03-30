#pragma comment(lib, "opengl32.lib")

#include "ShaderProgram.h"
#include "ShaderProgram.h"
#include "debug_utils.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <fstream>
#include <iostream>

using namespace std;

ShaderProgram::ShaderProgram()
: _vertexProgram(0), _fragmentProgram(0), _geometryProgram(0) 
{ 
	_programId = glCreateProgram();
}

void ShaderProgram::load(const std::string& vertexSource, const std::string& fragmentSource) throw (ShaderException)
{
    loadAux(vertexSource, fragmentSource, "");
}

void ShaderProgram::load(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource) throw (ShaderException)
{
    loadAux(vertexSource, fragmentSource, geometrySource);
}

void ShaderProgram::loadAux(const std::string& vertexSoruce, const std::string& fragmentSource, const std::string& geometrySource) throw (ShaderException)
{
	_vertexSource = vertexSoruce;
	_fragmentSource = fragmentSource;
	_geometrySource = geometrySource;

    if (!vertexSoruce.empty()) {
        _vertexProgram = loadShader(vertexSoruce, GL_VERTEX_SHADER);
    }

    if (!fragmentSource.empty()) {
        _fragmentProgram = loadShader(fragmentSource, GL_FRAGMENT_SHADER);
    }

    if (!geometrySource.empty()) {
        _geometryProgram = loadShader(geometrySource, GL_GEOMETRY_SHADER);
    }

    if (_vertexProgram)
        glAttachShader(_programId, _vertexProgram);
	
	if (_geometryProgram)
        glAttachShader(_programId, _geometryProgram);

    if (_fragmentProgram)
        glAttachShader(_programId, _fragmentProgram);

	
	glLinkProgram(_programId);

    int linked;
    glGetProgramiv(_programId, GL_LINK_STATUS, &linked);

    if (!linked) {
        int logLength;
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];
        string logStr(log);
        delete[] log;

        int charsWritten;
        glGetProgramInfoLog(_programId, logLength, &charsWritten, log);

		throw ShaderException(string("error while linking shader: \n") + logStr);
    }

	parseParameters();
}


void ShaderProgram::parseParameters()
{
	int count;
	const int NAME_BUFFER_SIZE = 256;
	char nameBuffer[NAME_BUFFER_SIZE];
	int nameLen;

	count = getActiveUniforms();
	for (int i = 0; i < count; i++) {
		ShaderParameter uniform;
		uniform.index = i;
		glGetActiveUniform(getId(), i, NAME_BUFFER_SIZE, &nameLen,  &(uniform.size), &(uniform.type), nameBuffer);
		uniform.name = std::string(nameBuffer);
		_uniforms[uniform.name] = uniform;
	}

	count = getActiveAttributes();
	for (int i = 0; i < count; i++) {
		ShaderParameter attribute;
		attribute.index = i;
		glGetActiveAttrib(getId(), i, NAME_BUFFER_SIZE, &nameLen, &(attribute.size), &(attribute.type), nameBuffer);
		attribute.name = std::string(nameBuffer);
		_attributes[attribute.name] = attribute;
	}
}


int ShaderProgram::loadShader(const std::string& sourcePath, int shaderType) throw (ShaderException)
{
    fstream sourceStream(sourcePath.c_str(), ios_base::in);
    int retVal = 0;
    if (sourceStream.is_open()) {
        std::string source((std::istreambuf_iterator<char>(sourceStream)), std::istreambuf_iterator<char>());
        retVal = glCreateShader(shaderType);
        const char* src = const_cast<char*> (source.c_str());
        glShaderSource(retVal, 1, &src, 0);
        glCompileShader(retVal);
        int compiled;
        glGetShaderiv(retVal, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            int logLength;
            glGetShaderiv(retVal, GL_INFO_LOG_LENGTH, &logLength);
            char* log = new char[logLength];
            int charsWritten;
            glGetShaderInfoLog(retVal, logLength, &charsWritten, log);
            string logStr(log);
            delete [] log;
			throw ShaderException(string("Error while compiling ") + string(shaderTypeToString(shaderType)) + string(": ") + sourcePath + string(": ") + logStr);
        }
    } else {
		throw ShaderException(string("Error while opening file: ") + sourcePath);
    }
    return retVal;
}

ShaderProgram::~ShaderProgram()
{
    if (_programId)
        glDeleteProgram(_programId);
    if (_vertexProgram)
        glDeleteShader(_vertexProgram);
    if (_fragmentProgram)
        glDeleteShader(_fragmentProgram);
    if (_geometryProgram)
        glDeleteShader(_geometryProgram);
}

void ShaderProgram::useThis() const
{
    glUseProgram(getId());
}

void ShaderProgram::useNone()
{
    glUseProgram(0);
}

int ShaderProgram::getAttribLocation(const char* name)
{
	int retVal = glGetAttribLocation(_programId, name);
	if (retVal == -1) {
		throw ShaderException(string("can't find attrib: \"") + string(name) + string("\" in ") + _vertexSource);
	}
	return retVal;
}

int ShaderProgram::getUniformLocation(const char* name)
{
	int retVal = glGetUniformLocation(_programId, name);
	if (retVal == -1) {
		throw ShaderException(string("can't find uniform: \"") + string(name) + string("\" in any of ") + getShaderSourcesString());
	}
	return retVal;
}

void ShaderProgram::bindFragDataLocation(int colorNumber, const char* name)
{
	glBindFragDataLocation(getId(), colorNumber, name);
}


void ShaderProgram::setUniform1i(const char* name, int value)
{
	CHECK_GL_CMD(int location = glGetUniformLocation(getId(), name));
	useThis();
	CHECK_GL_CMD(glUniform1i(location, value));
}

void ShaderProgram::setUniform1f(const char* name, float value)
{
	CHECK_GL_CMD(int location = glGetUniformLocation(getId(), name));
	useThis();
	CHECK_GL_CMD(glUniform1f(location, value));
}

void ShaderProgram::setUniform2f(const char* name, float v0, float v1)
{
	int location = glGetUniformLocation(getId(), name);
	useThis();
	glUniform2f(location, v0, v1);
}

void ShaderProgram::setUniform3f(const char* name, float* data)
{
	int location = glGetUniformLocation(getId(), name);
	useThis();
	glUniform3fv(location, 1, data);
}

void ShaderProgram::setUniform4f(const char* name, float* data)
{
	CHECK_GL_CMD(int location = glGetUniformLocation(getId(), name));
	useThis();
	CHECK_GL_CMD(glUniform4fv(location, 1, data));
}

void ShaderProgram::setUniformMat4f(const char* name, float* data)
{
	useThis();
	glUniformMatrix4fv(_uniforms.at(name).index, 1, GL_FALSE, data);
}

void ShaderProgram::setUniformMat3f(const char* name, float* data)
{
	useThis();
	glUniformMatrix3fv(_uniforms.at(name).index, 1, GL_FALSE, data);
}

std::string ShaderProgram::shaderTypeToString(int shaderType)
{
	switch (shaderType) {
	case GL_VERTEX_SHADER:
		return "vertex shader";
	case GL_GEOMETRY_SHADER:
		return "geometry shader";
	case GL_FRAGMENT_SHADER:
		return "fragment shader";
	default:
		return "unknown shader type";
	}
}

int ShaderProgram::getActiveUniforms()
{
	int retVal;
	glGetProgramiv(getId(), GL_ACTIVE_UNIFORMS, &retVal);
	return retVal;
}

int ShaderProgram::getActiveAttributes()
{
	int retVal;
	glGetProgramiv(getId(), GL_ACTIVE_ATTRIBUTES, &retVal);
	return retVal;
}

void ShaderProgram::printParameters()
{
	std::cout << "UNIFORMS: " << std::endl;
	for (map<string, ShaderParameter>::iterator it = _uniforms.begin(); it != _uniforms.end(); ++it) {
		std::cout << "\t" << it->first << "; size: " << it->second.size << std::endl;
	}
	std::cout << "ATTRIBUTES: " << std::endl;
	for (map<string, ShaderParameter>::iterator it = _attributes.begin(); it != _attributes.end(); ++it) {
		std::cout << "\t" << it->first << "; size: " << it->second.size << "; type: " << it->second.type << std::endl;
	}
}