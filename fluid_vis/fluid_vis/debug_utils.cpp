#include "debug_utils.h"

std::string getFrameBufferStatusString()
{
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (fboStatus) {
	case GL_FRAMEBUFFER_UNDEFINED:
		return std::string("GL_FRAMEBUFFER_UNDEFINED");
	case GL_FRAMEBUFFER_COMPLETE:
		return std::string("GL_FRAMEBUFFER_COMPLETE");
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return std::string("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return std::string("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return std::string("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return std::string("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
	case GL_FRAMEBUFFER_UNSUPPORTED:
		return std::string("GL_FRAMEBUFFER_UNSUPPORTED");
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		return std::string("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		return std::string("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
	default:
		return std::string("unknown FBO error");
	}
}

std::string DebugUtils::glErrorToString(GLenum error)
{
	switch (error) {
	case GL_NO_ERROR:
		return std::string("GL_NO_ERROR");
	case GL_INVALID_ENUM:
		return std::string("GL_INVALID_ENUM");
	case GL_INVALID_VALUE:
		return std::string("GL_INVALID_VALUE");
	case GL_INVALID_OPERATION:
		return std::string("GL_INVALID_OPERATION");
	case GL_STACK_OVERFLOW:
		return std::string("GL_STACK_OVERFLOW");
	case GL_STACK_UNDERFLOW:
		return std::string("GL_STACK_UNDERFLOW");
	case GL_OUT_OF_MEMORY:
		return std::string("GL_OUT_OF_MEMORY");
	case GL_TABLE_TOO_LARGE:
		return std::string("GL_TABLE_TOO_LARGE");
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return std::string("GL_INVALID_FRAMEBUFFER_OPERATION: ") + getFrameBufferStatusString();
	default:
		DEBUG_COUT(<< error << std::endl);
		return std::string("unknown error");
	}
}