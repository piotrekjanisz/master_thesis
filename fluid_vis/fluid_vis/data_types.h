#pragma once

#include <boost/smart_ptr.hpp>

extern class FrameBuffer;
extern class GfxObject;
extern class GfxStaticObject;
extern class ShaderProgram;
extern class Texture;
extern class ScreenQuad;
extern class SurfaceExtractor;

typedef boost::shared_ptr<Texture> TexturePtr;
typedef boost::shared_ptr<ShaderProgram> ShaderProgramPtr;
typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;
typedef boost::shared_ptr<GfxObject> GfxObjectPtr;
typedef boost::shared_ptr<GfxStaticObject> GfxStaticObjectPtr;
typedef boost::shared_ptr<ScreenQuad> ScreenQuadPtr;
typedef boost::shared_ptr<SurfaceExtractor> SurfaceExtractorPtr;