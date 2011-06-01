#include "ScreenQuad.h"

float ScreenQuad::quadCoords[] = {
	-1.0f, -1.0f, 
	 1.0f, -1.0f,
	-1.0f,  1.0f,
	 1.0f,  1.0f
};

ScreenQuad::ScreenQuad(const ShaderProgramPtr& shaderProgram)
	: _shaderProgram(shaderProgram)
{
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_posVbo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _posVbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), quadCoords, GL_STATIC_DRAW);
	_shaderProgram->useThis();
	int posLocation = _shaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posLocation);
}


ScreenQuad::~ScreenQuad(void)
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_posVbo);
}

void ScreenQuad::render()
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ScreenQuad::attachTexture(const TexturePtr& texture, GLenum textureAttachement)
{
	texture->bindToTextureUnit(textureAttachement);
}
