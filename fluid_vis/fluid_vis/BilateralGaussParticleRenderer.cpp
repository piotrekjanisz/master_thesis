#include "BilateralGaussParticleRenderer.h"
#include "debug_utils.h"
#include "filters.h"

BilateralGaussParticleRenderer::BilateralGaussParticleRenderer(AbstractScene* scene)
	: ScreenSpaceParticleRenderer(scene),
	 _filterSizeMult(0.5f),
	 _edgeTreshold(0.003f),
	 _bilateralGaussSigma(10.0f)
{
}


BilateralGaussParticleRenderer::~BilateralGaussParticleRenderer(void)
{
}

void BilateralGaussParticleRenderer::resize(int width, int height)
{
	ScreenSpaceParticleRenderer::resize(width, height);

	_smoothAuxTexture->resize2D(width, height);
	_frameBuffer->resize(width, height);
}

bool BilateralGaussParticleRenderer::setup()
{
	if (!ScreenSpaceParticleRenderer::setup())
		return false;

	try {
		_bilateralGaussSmoothShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_bilateralGaussSmoothShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->load("shaders/screen_quad_vertex.glsl", "shaders/separable_bilateral_gauss_pointsize_fragment.glsl"));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("gaussianDist", 0));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("spatialDist", 1));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("linearDepth", 2));

		_screenQuad = boost::make_shared<ScreenQuad>(_bilateralGaussSmoothShader);

		_frameBuffer = boost::make_shared<FrameBuffer>();
		// TODO is it necessary
		CHECK_GL_CMD(_frameBuffer->attachRenderbuffer(GL_RGBA, GL_COLOR_ATTACHMENT0, 640, 480));

		_smoothAuxTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED);

		float gaussData[64000];
		const int GAUSS_SIZE = 21;
		const int HEAVISIDE_SIZE = 5000;
		Filters::createHeavisideDistribution(0.0, 1.0, _edgeTreshold, HEAVISIDE_SIZE, gaussData);
		_spatialDistTexture = Texture::create1DDepthTexture(GL_NEAREST, GL_CLAMP_TO_EDGE, HEAVISIDE_SIZE, 0, gaussData);

		int filtersNum = Filters::createGauss1DArrayAsc(50, 0.2, gaussData);
		_maxFilter = filtersNum - 1;
		DEBUG_PRINT_VAR(_maxFilter);
		CHECK_GL_CMD(_gaussDistributionsArrayTexture = Texture::create1DDepthTextureArray(GL_LINEAR, GL_CLAMP_TO_EDGE, 50, filtersNum, gaussData));
	} catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
		fgetc(stdin);
		return false;
	}

	return true;
}

void BilateralGaussParticleRenderer::smoothWaterTexture()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->getId()));
	CHECK_GL_CMD(_frameBuffer->attachTexture2D(_smoothAuxTexture, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	CHECK_GL_CMD(_screenQuad->attachTexture(_gaussDistributionsArrayTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_screenQuad->attachTexture(_spatialDistTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE2));

	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / _scene->getWidth(), 0.0f));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1f("farDist", _scene->getZFar()));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1i("maxPointSize", _maxParticleSize));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1f("filterSizeMult", _filterSizeMult));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1i("maxFilter", _maxFilter));
	CHECK_GL_CMD(_screenQuad->render());
	
	CHECK_GL_CMD(_frameBuffer->attachTexture2D(_waterLinDetphTexture, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	
	CHECK_GL_CMD(_screenQuad->attachTexture(_smoothAuxTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 0.0f, 1.0f / _scene->getHeight()));
	CHECK_GL_CMD(_screenQuad->render());
}
