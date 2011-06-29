#pragma once

#include <string>
#include "BaseException.h"
#include "NonCopyable.h"

class ShaderException : public BaseException
{
public:
    ShaderException(const std::string& msg)
		: BaseException(msg) {}
};

class ShaderProgram : public NonCopyable
{
public:
    ShaderProgram();
    void load(const std::string& vertexSource, const std::string& fragmentSource) throw(ShaderException);
    void load(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource) throw(ShaderException);
    virtual ~ShaderProgram();

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

	void bindFragDataLocation(int colorNumber, const char* name_);

	void setUniform1i(const char* name, int value);

	void setUniform1f(const char* name, float value);

	void setUniform2f(const char* name, float v0, float v1);
private:
    void loadAux(const std::string& vertexSoruce, const std::string& fragmentSource, const std::string& geometrySource) throw(ShaderException);
    int loadShader(const std::string& source, int shaderType) throw(ShaderException);

    int _programId;
    int _vertexProgram;
    int _fragmentProgram;
    int _geometryProgram;
};

