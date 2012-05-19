#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include "data_types.h"
#include "NonCopyable.h"

class FrameBuffer
{
public:
	struct RenderBuffer {
		unsigned int id;
		GLenum format;
	};

public:
	FrameBuffer(void);
	~FrameBuffer(void);

	static FrameBufferPtr create()
	{
		return boost::make_shared<FrameBuffer>();
	}

	/**
	 * format - format of buffer data (ie. GL_RGBA8, GL_DEPTH_COMPONENT)
	 * target - what will be drawn to this renderbuffer (ie DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0)
	 */
	void attachRenderbuffer(GLenum format, GLenum target, int width, int height);

	void attachTexture2D(TexturePtr& texture, GLenum target);

	void detachTexture2D(GLenum target);

	void copyRenderColorToScreen(GLenum sourceTarget, 
							int srcX0, int srcY0, int srcX1, int srcY1,
							int dstX0, int dstY0, int dstX1, int dstY1);

	void bindForReading()
	{
		//glDrawBuffers(_renderTargets.size(), _renderTargets.data());
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _fboName);
	}

	void bindForDrawing() 
	{		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboName);
		glDrawBuffers(_renderTargets.size(), _renderTargets.data());
	}

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, getId());
	}

	void mapDefaultOutputs(int outputNum)
	{
		GLenum fboBufs[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5,
			GL_COLOR_ATTACHMENT6,
			GL_COLOR_ATTACHMENT7,
			GL_COLOR_ATTACHMENT8,
			GL_COLOR_ATTACHMENT9,
			GL_COLOR_ATTACHMENT10,
			GL_COLOR_ATTACHMENT11,
		};

		glDrawBuffers(outputNum, fboBufs);
	}

	static void bindDefaultForReading() 
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	static void bindDefaultForDrawing() 
	{
		//GLenum drawBuffers[] = {GL_BACK_LEFT};
		//glDrawBuffers(1, drawBuffers);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	unsigned int getId() const { return _fboName; }

	void resize(int width, int height);
private:
	unsigned int _fboName;
	std::vector<RenderBuffer> _renderBuffers;
	std::vector<unsigned int> _renderTargets;


};

