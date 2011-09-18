#include "Texture.h"
#include <GL/glus.h>
#include <boost/make_shared.hpp>
#include <string>

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

void Texture::setup(int width, int height, int internalFormat, GLenum dataFormat, int border, GLenum dataType)
{
	_width = width;
	_height = height;
	_internalFormat = internalFormat;
	_dataFormat = dataFormat;
	_border = border;
	_dataType = dataType;
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
	setup(width, height, internalFormat, dataFormat, border, GL_FLOAT);
	glBindTexture(_textureType, _textureId);
	glTexImage2D(_textureType, 0, internalFormat, width, height, border, dataFormat, GL_FLOAT, data);
}

void Texture::load2DUnsignedByteDataNoMipMap(int internalFormat, int width, int height, int border, GLenum dataFormat, void* data)
{
	setup(width, height, internalFormat, dataFormat, border, GL_UNSIGNED_BYTE);	
	glBindTexture(_textureType, _textureId);
	glTexImage2D(_textureType, 0, internalFormat, width, height, border, dataFormat, GL_UNSIGNED_BYTE, data);
}

void Texture::load1DFloatDataNoMipMap(int internalFormat, int size, int border, GLenum dataFormat, float* data)
{
	setup(size, 1, internalFormat, dataFormat, border, GL_FLOAT);
	glBindTexture(_textureType, _textureId);
	glTexImage1D(_textureType, 0, internalFormat, size, border, dataFormat, GL_FLOAT, data);
}

void Texture::getData(int level, GLenum format, GLenum type, void* data)
{
	bind();
	glGetTexImage(_textureType, level, format, type, data);
}

TexturePtr Texture::create2DRGBTexture(GLenum filteringMode, GLenum wrappingMode, int width, int height, int border, float* data)
{
	TexturePtr retVal = boost::make_shared<Texture>(GL_TEXTURE_2D);
	retVal->setParameters(filteringMode, wrappingMode);
	retVal->load2DFloatDataNoMipMap(GL_RGBA8, width, height, border, GL_RGBA, data);
	return retVal;
}

TexturePtr Texture::create2DDepthTexture(GLenum filteringMode, GLenum wrappingMode, int width, int height, int border, float* data)
{
	TexturePtr retVal = boost::make_shared<Texture>(GL_TEXTURE_2D);
	retVal->setParameters(filteringMode, wrappingMode);
	retVal->load2DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, width, height, border, GL_DEPTH_COMPONENT, data);
	return retVal;
}

TexturePtr Texture::createTexture2DFromImage(GLenum filteringMode, GLenum wrappingMode, const std::string& tgaPath)
{
	TexturePtr retVal = boost::make_shared<Texture>(GL_TEXTURE_2D);
	retVal->setParameters(filteringMode, wrappingMode);
	
	GLUStgaimage tgaImage;
	glusLoadTgaImage(tgaPath.c_str(), &tgaImage);
	retVal->load2DUnsignedByteDataNoMipMap(tgaImage.format, tgaImage.width, tgaImage.height, 0, tgaImage.format, tgaImage.data);
	glusDestroyTgaImage(&tgaImage);

	return retVal;
}

TexturePtr Texture::create2DTexture(GLenum filteringMode, GLenum wrappingMode, int width, int height, int internalFormat, GLenum format, int border, float* data)
{
	TexturePtr retVal = boost::make_shared<Texture>(GL_TEXTURE_2D);
	retVal->setParameters(filteringMode, wrappingMode);
	retVal->load2DFloatDataNoMipMap(internalFormat, width, height, border, format, data);
	return retVal;
}

TexturePtr Texture::create1DDepthTexture(GLenum filteringMode, GLenum wrappingMode, int size, int border, float* data)
{
	TexturePtr retVal = boost::make_shared<Texture>(GL_TEXTURE_1D);
	retVal->setParameters(filteringMode, wrappingMode);
	retVal->load1DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, size, border, GL_DEPTH_COMPONENT, data);
	return retVal;
}

TexturePtr Texture::createCubeMap(GLenum filteringMode, GLenum wrappingMode, const std::string& pathPrefix)
{
	TexturePtr retVal = boost::make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
	retVal->setParameters(filteringMode, wrappingMode);
	
	retVal->bind();

	std::string postfixes[] = {
		"_pos_x.tga", "_neg_x.tga", 
		"_pos_y.tga", "_neg_y.tga",
		"_pos_z.tga", "_neg_z.tga"
	};
	GLenum cube[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	for (int i = 0; i < 6; i++) {
		GLUStgaimage img;
		std::string fullPath = pathPrefix + postfixes[i];
		glusLoadTgaImage(fullPath.c_str(), &img);
		glTexImage2D(cube[i], 0, img.format, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
		glusDestroyTgaImage(&img);
	}

	return retVal;
}

void Texture::resize1D(int size)
{
	glBindTexture(_textureType, _textureId);
	glTexImage1D(_textureType, 0, _internalFormat, size, _border, _dataFormat, _dataType, 0);
}

void Texture::resize2D(int width, int height)
{
	glBindTexture(_textureType, _textureId);
	glTexImage2D(_textureType, 0, _internalFormat, width, height, _border, _dataFormat, _dataType, 0);
}

void Texture::generateMipMaps()
{
	glBindTexture(_textureType, _textureId);
	glGenerateMipmap(_textureType);
}
