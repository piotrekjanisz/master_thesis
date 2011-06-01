#pragma once

#include "NonCopyable.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "data_types.h"

class ScreenQuad : public NonCopyable
{
public:
	ScreenQuad(const ShaderProgramPtr& shaderProgram);
	~ScreenQuad(void);

	void render();

	void attachTexture(const TexturePtr& texture, GLenum textureAttachement);

	ShaderProgramPtr getShaderProgram() const { return _shaderProgram; }

private:
	static float quadCoords[];

	unsigned int _vao;
	unsigned int _posVbo;
	ShaderProgramPtr _shaderProgram;
};

