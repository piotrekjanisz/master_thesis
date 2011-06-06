#include "Texture.h"


Texture::Texture(GLenum textureType)
	: _textureId(0), _textureType(textureType), _width(0), _height(0)
{
	glGenTextures(1, &_textureId);
}


Texture::~Texture(void)
{
	if (_textureId != 0) {
		glDeleteTextures(1, &_textureId);
	}
}


void Texture::setTextureFilter(GLenum filter)
{
	glBindTexture(_textureType, _textureId);
	glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, filter);
}


void Texture::setWrappingMode(GLenum mode)
{
	glBindTexture(_textureType, _textureId);
	glTexParameteri(_textureType, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(_textureType, GL_TEXTURE_WRAP_T, mode);
	glTexParameteri(_textureType, GL_TEXTURE_WRAP_R, mode);
}


void Texture::setParameters(GLenum filtering, GLenum wrappingMode)
{
	glBindTexture(_textureType, _textureId);
	glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(_textureType, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(_textureType, GL_TEXTURE_WRAP_T, wrappingMode);
	glTexParameteri(_textureType, GL_TEXTURE_WRAP_R, wrappingMode);
}

void Texture::load2DFloatDataNoMipMap(int internalFormat, int width, int height, int border, GLenum dataFormat, float* data)
{
	_width = width;
	_height = height;
	glBindTexture(_textureType, _textureId);
	glTexImage2D(_textureType, 0, internalFormat, width, height, border, dataFormat, GL_FLOAT, data);
}

void Texture::load1DFloatDataNoMipMap(int internalFormat, int size, int border, GLenum dataFormat, float* data)
{
	_width = size;
	_height = 1;
	glBindTexture(_textureType, _textureId);
	glTexImage1D(_textureType, 0, internalFormat, size, border, dataFormat, GL_FLOAT, data);
}

void Texture::getData(int level, GLenum format, GLenum type, void* data)
{
	glGetTexImage(_textureType, level, format, type, data);
}
