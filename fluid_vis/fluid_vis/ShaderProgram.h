#pragma once

#include <string>
#include <map>
#include <GL/glew.h>
#include "BaseException.h"
#include "NonCopyable.h"
#include "data_types.h"

class ShaderException : public BaseException
{
public:
    ShaderException(const std::string& msg)
		: BaseException(msg) {}
};

struct ShaderParameter
{
	int size;
	int index;
	std::string name;
	GLenum type;
};

class ShaderProgram : public NonCopyable
{
public:
    ShaderProgram();
    void load(const std::string& vertexSource, const std::string& fragmentSource) throw(ShaderException);
    void load(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource) throw(ShaderException);
    virtual ~ShaderProgram();

	static ShaderProgramPtr create()
	{
		return boost::make_shared<ShaderProgram>();
	}

    /**
     * @return opengl shader program id
     */
    int getId() const { return _programId; }

    /**
     * Binds this program
     */
    void useThis() const;

    /**
     * Unbinds any bind program
     */
    static void useNone();

    int getAttribLocation(const char* name);

    int getUniformLocation(const char* name);

	int getActiveUniforms();

	int getActiveAttributes();

	void bindFragDataLocation(int colorNumber, const char* name_);

	void setUniform1i(const char* name, int value);

	void setUniform1f(const char* name, float value);

	void setUniform2f(const char* name, float v0, float v1);

	void setUniform3f(const char* name, float* data);

	void setUniform4f(const char* name, float* data);

	void setUniformMat4f(const char* name, float* data);

	void setUniformMat3f(const char* name, float* data);

	void printParameters();

private:
    void loadAux(const std::string& vertexSoruce, const std::string& fragmentSource, const std::string& geometrySource) throw(ShaderException);
    int loadShader(const std::string& source, int shaderType) throw(ShaderException);
	
	std::string getShaderSourcesString()
	{
		return _vertexSource + ", " + _fragmentSource + ", " + _geometrySource; 
	}

	std::string shaderTypeToString(int shaderType);

	void parseParameters();

	std::map<std::string, ShaderParameter> _attributes;
	std::map<std::string, ShaderParameter> _uniforms;

    int _programId;
    int _vertexProgram;
    int _fragmentProgram;
    int _geometryProgram;

	std::string _vertexSource;
	std::string _fragmentSource;
	std::string _geometrySource;
};

