#include "ConfigurationFactory.h"
#include "IsosurfaceParticleRenderer.h"
#include "debug_utils.h"


using namespace std;

IsosurfaceParticleRenderer::IsosurfaceParticleRenderer(AbstractScene* scene)
	: ParticleRenderer(scene)
{
}


IsosurfaceParticleRenderer::~IsosurfaceParticleRenderer(void)
{
}

const std::set<std::string>& IsosurfaceParticleRenderer::getParameters()
{
	return _parameters;
}

bool IsosurfaceParticleRenderer::changeParameter(const std::string& parameter, ParamOperation operation)
{
	return false;
}

bool IsosurfaceParticleRenderer::setup()
{
	SurfaceExtractorDesc desc;
	ConfigurationFactory configurationFactory("config");
	desc = configurationFactory.createSurfaceExtractorDesc("extractor");
	_surfaceExtractor = boost::make_shared<MtSurfaceExtractor>(desc);
	_currentOutput = 0;

	_waterShader = boost::make_shared<ShaderProgram>();
	CHECK_GL_CMD(_waterShader->load("shaders/normalization_vertex.glsl", "shaders/water_surface_fragment.glsl"));
	CHECK_GL_CMD(_waterShader->bindFragDataLocation(0, "fragColor"));

	_sceneShader = boost::make_shared<ShaderProgram>();
	CHECK_GL_CMD(_sceneShader->load("shaders/screen_quad_vertex.glsl", "shaders/copy_color_and_depth_fragment.glsl"));
	CHECK_GL_CMD(_sceneShader->bindFragDataLocation(0, "frag_color"));
	CHECK_GL_CMD(_sceneShader->setUniform1i("colorTexture", 0));
	CHECK_GL_CMD(_sceneShader->setUniform1i("depthTexture", 1));

	_screenQuad = boost::make_shared<ScreenQuad>(_sceneShader);

	_frameBuffer = boost::make_shared<FrameBuffer>();

	_isoWater = boost::make_shared<GfxObject>();
	CHECK_GL_CMD(_isoWater->addAttribute("vertex", 0, 140000, 4, GfxObject::DYNAMIC_ATTR));
	CHECK_GL_CMD(_isoWater->addAttribute("normal", 0, 140000, 3, GfxObject::DYNAMIC_ATTR));
	//CHECK_GL_CMD(_isoWater->addShader(_waterShader));

	return true;
}

void IsosurfaceParticleRenderer::resize(int width, int height)
{
}

void IsosurfaceParticleRenderer::render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData)
{
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	CHECK_GL_CMD(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
	CHECK_GL_CMD(_screenQuad->attachTexture(sceneColorTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_screenQuad->attachTexture(sceneDepthTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->render());

	
	_outputs[_currentOutput].clear();
	_surfaceExtractor->extractSurface(particleData.particles, particleData.particleCount, 4, &_outputs[_currentOutput]);
	_surfaceExtractor->waitForResults();
	CHECK_GL_CMD(_waterShader->useThis());
	CHECK_GL_CMD(_waterShader->setUniformMat4f("projectionMatrix", _scene->getProjectionMatrix()));
	CHECK_GL_CMD(_waterShader->setUniformMat4f("modelViewMatrix", _scene->getViewMatrix()));
	CHECK_GL_CMD(_waterShader->setUniformMat3f("normalMatrix", _scene->getNormalMatrix(_scene->getViewMatrix())));
	
	for (list<TriangleMesh>::iterator it = _outputs[_currentOutput].begin(); it != _outputs[_currentOutput].end(); ++it) {
		CHECK_GL_CMD(_isoWater->updateAttribute("vertex", it->vertices, it->verticesCount));
		CHECK_GL_CMD(_isoWater->updateAttribute("normal", it->normals, it->verticesCount));
		CHECK_GL_CMD(_isoWater->renderElements(_waterShader, GL_TRIANGLES, it->trianglesCount*3, it->indices));	
	}
	
}
