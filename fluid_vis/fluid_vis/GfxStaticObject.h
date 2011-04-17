#pragma once

#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <GL/glus.h>
#include "BaseException.h"
#include "ShaderProgram.h"
#include "ShapeFactory.h"

class GfxException : public BaseException
{
public:
	GfxException(const std::string& msg)
		: BaseException(msg) {}
};

/*
	Copying of this object should not be allowed
*/
class GfxStaticObject
{
	unsigned int _vao;
	std::vector<unsigned int> _vbos;
	int _numberIndices;
	boost::shared_ptr<ShaderProgram> _shaderProgram;
public:
	GfxStaticObject(const boost::shared_ptr<ShaderProgram>& shaderProgram);
	~GfxStaticObject(void);

	void createFromGlusShape(const GLUSshape& shape) throw(GfxException);
	void createFromShape(const ShapePtr& shape) throw(GfxException);
	void addAttribute(const std::string& name, float* data, int count, int components) throw(GfxException);
	void render();
};

