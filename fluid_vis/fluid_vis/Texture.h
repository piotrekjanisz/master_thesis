#pragma once

#include <GL/glew.h>
#include "NonCopyable.h"

class Texture : public NonCopyable
{
public:
	Texture(GLenum textureType);
	~Texture(void);

	unsigned int getTextureId() const { return _textureId; } 

	unsigned int getWidth() const { return _width; }
	unsigned int getHeight() const { return _height; }

	GLenum getTextureType() const { return _textureType; }

	/**
	 * Sets both MAG and MIN filter
	 */
	void setTextureFilter(GLenum filter);

	/**
	 * Sets WRAP_S, T and R
	 */ 
	void setWrappingMode(GLenum mode);

	/**
	 * filtering - both MAG and MIN filters will be set to this value
	 * wrappingMode - wrapping mode for S, T and R coordinates
	 */
	void setParameters(GLenum filtering, GLenum wrappingMode);

	/**
	 * if data != NULL loads data to texture
	 * else allocates space
	 */
	void load2DFloatDataNoMipMap(int internalFormat, int width, int height, int border, GLenum dataFormat, float* data);

	void bind()
	{
		glBindTexture(_textureType, _textureId);
	}

	void bindToTextureUnit(GLenum textureUnit) 
	{
		glActiveTexture(textureUnit);
		bind();
	}

	void getData(int level, GLenum format, GLenum type, void* data);

private:

	unsigned int _textureId;
	/**
	 * ie GL_TEXTURE_1D, GL_TEXTURE_2D ...
	 */
	GLenum _textureType;
	GLenum _textureFormat;
	int _width;
	int _height;
};

