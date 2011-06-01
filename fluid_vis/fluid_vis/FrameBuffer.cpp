#include "FrameBuffer.h"
#include "Texture.h"


FrameBuffer::FrameBuffer(void)
	:_fboName(0) 
{
	glGenFramebuffers(1, &_fboName);
}



FrameBuffer::~FrameBuffer(void)
{
	if (_fboName > 0) {
		glDeleteFramebuffers(1, &_fboName);
	}
}

void FrameBuffer::attachRenderbuffer(GLenum format, GLenum target, int width, int height)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboName);
	unsigned int renderBufferId;
	_renderBuffers.push_back(renderBufferId);
	_renderTargets.push_back(target);
	glGenRenderbuffers(1, &renderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, target, GL_RENDERBUFFER, renderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void FrameBuffer::attachTexture2D(TexturePtr& texture, GLenum target)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboName);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, target, texture->getTextureType(), texture->getTextureId(), 0);
}

void FrameBuffer::copyRenderColorToScreen(GLenum sourceTarget, 
							int srcX0, int srcY0, int srcX1, int srcY1,
							int dstX0, int dstY0, int dstX1, int dstY1)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _fboName);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glReadBuffer(sourceTarget);
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}