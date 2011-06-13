#pragma once

#include <GL/glew.h>
#include "NonCopyable.h"
#include "data_types.h"

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

	void load1DFloatDataNoMipMap(int internalFormat, int size, int border, GLenum dataFormat, float* data);

	void load2DUnsignedByteDataNoMipMap(int internalFormat, int width, int height, int border, GLenum dataFormat, void* data);

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


	// some factory methods
	static TexturePtr create2DRGBTexture(GLenum filteringMode, GLenum wrappingMode, int width, int height, int border = 0, float* data = 0);
	static TexturePtr create2DDepthTexture(GLenum filteringMode, GLenum wrappingMode, int width, int height, int border = 0, float* data = 0);
	static TexturePtr createTexture2DFromImage(GLenum filteringMode, GLenum wrappingMode, const std::string& tgaPath);
	static TexturePtr create1DDepthTexture(GLenum filteringMode, GLenum wrappingMode, int size, int border, float* data);
	static TexturePtr create2DTexture(GLenum filteringMode, GLenum wrappingMode, int width, int height, int internalFormat, GLenum format, int border = 0, float* data = 0);

	/**
	 * pathPrefix - method will try to load images from following files:
	 * patPrefix + "_neg_x.tga", patPrefix + "_pos_x.tga",
	 * patPrefix + "_neg_y.tga", patPrefix + "_pos_y.tga",
	 * patPrefix + "_neg_z.tga", patPrefix + "_pos_z.tga",
	 */
	static TexturePtr createCubeMap(GLenum filteringMode, GLenum wrappingMode, const std::string& pathPrefix);

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

