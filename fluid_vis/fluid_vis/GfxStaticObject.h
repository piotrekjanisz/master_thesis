#pragma once

#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <GL/glus.h>
#include <GL/glew.h>
#include "BaseException.h"
#include "ShaderProgram.h"
#include "ShapeFactory.h"
#include "data_types.h"
#include "NonCopyable.h"

class GfxException : public BaseException
{
public:
	GfxException(const std::string& msg)
		: BaseException(msg) {}
};

/*
	Copying of this object should not be allowed
*/
class GfxStaticObject : public NonCopyable
{
	unsigned int _vao;
	std::vector<unsigned int> _vbos;
	int _numberIndices;
	int _numberVertices;
	ShaderProgramPtr _shaderProgram;
	GLenum _primitiveType;

public:
	GfxStaticObject(const ShaderProgramPtr& shaderProgram);
	~GfxStaticObject(void);

	void createFromGlusShape(const GLUSshape& shape) throw(GfxException);
	void createFromShape(const ShapePtr& shape) throw(GfxException);
	void createFromScreenQuad(const ShapePtr& quad) throw(GfxException);
	void addAttribute(const std::string& name, float* data, int count, int components) throw(GfxException);
	void addTexture(const std::string& name, int textureUnit) throw (GfxException);
	void render();
	void renderArrays(GLenum mode); 

	const ShaderProgramPtr& getShader() const { return _shaderProgram; }
	ShaderProgramPtr& getShader() { return _shaderProgram; }
};

