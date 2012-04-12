#include "CurvatureFlowParticleRenderer.h"
#include "debug_utils.h"
#include "filters.h"
#include <vmmlib\vmmlib.hpp>
#include <iostream>

const std::string CurvatureFlowParticleRenderer::PARAM_PARTICLE_SIZE("particle size");
const std::string CurvatureFlowParticleRenderer::PARAM_THICKNESS_GAUSS_SIZE("thickness gauss size");
const std::string CurvatureFlowParticleRenderer::PARAM_THICKNESS_GAUSS_SIGMA("thickness gauss sigma");
const std::string CurvatureFlowParticleRenderer::PARAM_BLUR_ITERATION_COUNT("blur iteration count");
const std::string CurvatureFlowParticleRenderer::PARAM_PARTICLE_THICKNESS("particle thickness");
const std::string CurvatureFlowParticleRenderer::PARAM_TIME_STEP("time step");
const std::string CurvatureFlowParticleRenderer::PARAM_EDGE_TRESHOLD("edge trehold");
const std::string CurvatureFlowParticleRenderer::PARAM_THICKNESS_SIZE("thickness texture size");


CurvatureFlowParticleRenderer::CurvatureFlowParticleRenderer(AbstractScene* scene)
	: ParticleRenderer(scene), 
	_particleSize(150.0f), 
	_thicknessGaussSize(21),
	_thicknessGaussSigma(40.0),
	_blurIterationCount(30),
	_particleThickness(0.05f),
	_timeStep(-0.00000005f),
	_edgeTreshold(0.003f),
	_thicknessSize(0.5f)
{
	_parameterNames.insert(PARAM_PARTICLE_SIZE);
	_parameterNames.insert(PARAM_THICKNESS_GAUSS_SIZE);
	_parameterNames.insert(PARAM_THICKNESS_GAUSS_SIGMA);
	_parameterNames.insert(PARAM_BLUR_ITERATION_COUNT);
	_parameterNames.insert(PARAM_PARTICLE_THICKNESS);
	_parameterNames.insert(PARAM_EDGE_TRESHOLD);
	_parameterNames.insert(PARAM_TIME_STEP);
	_parameterNames.insert(PARAM_THICKNESS_SIZE);
}

#define PRINT_PARAM(param) std::cout << parameter << ": " << param << std::endl;

bool CurvatureFlowParticleRenderer::changeParameter(const std::string& parameter, ParamOperation operation)
{
	int sign = operation == ParamOperation::INC ? 1 : -1;

	if (parameter == PARAM_PARTICLE_SIZE) {
		_particleSize += sign * 1;
		PRINT_PARAM(_particleSize);
	} else if (parameter == PARAM_THICKNESS_GAUSS_SIZE) {
		_thicknessGaussSize += sign * 2;
		PRINT_PARAM(_thicknessGaussSigma);
		setupThicknessGauss();
	} else if (parameter == PARAM_THICKNESS_GAUSS_SIGMA) {
		_thicknessGaussSigma += sign * 0.01;
		PRINT_PARAM(_thicknessGaussSigma);
	} else if (parameter == PARAM_BLUR_ITERATION_COUNT) {
		_blurIterationCount += sign * 1;
		PRINT_PARAM(_blurIterationCount);
	} else if (parameter == PARAM_PARTICLE_THICKNESS) {
		_particleThickness += sign * 0.001;
		PRINT_PARAM(_particleThickness);
	} else if (parameter == PARAM_EDGE_TRESHOLD) {
		_edgeTreshold += sign * 0.0001;
		PRINT_PARAM(_edgeTreshold);
	} else if (parameter == PARAM_TIME_STEP) {
		_timeStep += sign * (-0.000000001f);
		PRINT_PARAM(_timeStep);
	} else if (parameter == PARAM_THICKNESS_SIZE) {
		_thicknessSize += sign * (0.05);
		resize(_scene->getWidth(), _scene->getHeight());
		PRINT_PARAM(_thicknessSize);
	} else {
		return false;
	}

	return true;
}


CurvatureFlowParticleRenderer::~CurvatureFlowParticleRenderer(void)
{
}

bool CurvatureFlowParticleRenderer::setup()
{
	if (! setupTextures())
		return false;

	if (! setupFramebuffers())
		return false;
	
	if (! setupShaders())
		return false;

	if (! setupObjects())
		return false;

	return true;
}

bool CurvatureFlowParticleRenderer::setupShaders()
{
	try {
		_waterThicknessShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_waterThicknessShader->load("shaders/water_depth_vertex.glsl", "shaders/water_depth_fragment.glsl"));
		CHECK_GL_CMD(_waterThicknessShader->bindFragDataLocation(0, "fragColor"));

		_waterShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_waterShader->load("shaders/water_shader_vertex.glsl", "shaders/water_shader_fragment.glsl"));
		CHECK_GL_CMD(_waterShader->bindFragDataLocation(0, "fragColor"));

		_gaussianBlurShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_gaussianBlurShader->load("shaders/screen_quad_vertex.glsl", "shaders/gaussian_blur_fragment.glsl"));
		CHECK_GL_CMD(_gaussianBlurShader->setUniform1i("inputImage", 0));
		CHECK_GL_CMD(_gaussianBlurShader->setUniform1i("gaussianDist", 1));
		CHECK_GL_CMD(_gaussianBlurShader->bindFragDataLocation(0, "frag_color"));

		_finalShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_finalShader->load("shaders/screen_quad_vertex.glsl", "shaders/final_stage_fragment2.glsl"));
		CHECK_GL_CMD(_finalShader->setUniform1i("depthTexture", 0));
		CHECK_GL_CMD(_finalShader->setUniform1i("sceneDepthTexture", 1));
		CHECK_GL_CMD(_finalShader->setUniform1i("sceneTexture", 2));
		CHECK_GL_CMD(_finalShader->setUniform1i("waterDepthTexture", 3));
		CHECK_GL_CMD(_finalShader->setUniform1i("wTexture", 4));
		CHECK_GL_CMD(_finalShader->bindFragDataLocation(0, "frag_color"));
		
		_edgeDetectionShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_edgeDetectionShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_edgeDetectionShader->load("shaders/screen_quad_vertex.glsl", "shaders/edge_detection_fragment.glsl"));
		CHECK_GL_CMD(_edgeDetectionShader->setUniform1f("treshold", 0.01f));

		_curvatureFlowShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_curvatureFlowShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_curvatureFlowShader->load("shaders/screen_quad_vertex.glsl", "shaders/curvature_flow_fragment.glsl"));
		CHECK_GL_CMD(_curvatureFlowShader->setUniform1i("depthTexture", 0));
		CHECK_GL_CMD(_curvatureFlowShader->setUniform1i("edgeTexture", 1));

		_copyTextureToDepthShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_copyTextureToDepthShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_copyTextureToDepthShader->load("shaders/screen_quad_vertex.glsl", "shaders/copy_texture_to_depth_fragment.glsl"));
		CHECK_GL_CMD(_copyTextureToDepthShader->setUniform1i("texture1", 0));

		_grayscaleTextureShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_grayscaleTextureShader->load("shaders/screen_quad_vertex.glsl", "shaders/show_texture_grayscale_fragment.glsl"));
		CHECK_GL_CMD(_grayscaleTextureShader->setUniform1i("texture1", 0));
		CHECK_GL_CMD(_grayscaleTextureShader->bindFragDataLocation(0, "frag_color"));
		_grayscaleIntermediateQuad = boost::make_shared<ScreenQuad>(_grayscaleTextureShader);
	} catch (const BaseException& ex) {
		std::cout << ex.what() << std::endl;
		fgetc(stdin);
		return false;
	}

	return true;
}


bool CurvatureFlowParticleRenderer::setupObjects()
{
	_water = boost::make_shared<GfxObject>();
	CHECK_GL_CMD(_water->addAttribute("vertex", 0, 70000, 4, GfxObject::DYNAMIC_ATTR));
	CHECK_GL_CMD(_water->addAttribute("density", 0, 70000, 1, GfxObject::DYNAMIC_ATTR));
	CHECK_GL_CMD(_water->addShader(_waterShader));
	CHECK_GL_CMD(_water->addShader(_waterThicknessShader));

	_blurQuad = boost::make_shared<ScreenQuad>(_gaussianBlurShader);
	_finalQuad = boost::make_shared<ScreenQuad>(_finalShader);
	_edgeQuad = boost::make_shared<ScreenQuad>(_edgeDetectionShader);
	_curvatureFlowQuad = boost::make_shared<ScreenQuad>(_curvatureFlowShader);
	_fillDepthBufferQuad = boost::make_shared<ScreenQuad>(_copyTextureToDepthShader);

	return true;
}


bool CurvatureFlowParticleRenderer::setupFramebuffers()
{
	_waterFB = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_waterFB->attachTexture2D(_waterDepthTexture, GL_DEPTH_ATTACHMENT));
	CHECK_GL_CMD(_waterFB->attachTexture2D(_waterLinDetphTexture, GL_COLOR_ATTACHMENT0));

	_waterThicknessFB = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_waterThicknessFB->attachTexture2D(_waterThicknessTexture, GL_COLOR_ATTACHMENT0));

	_smoothFB = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_smoothFB->attachRenderbuffer(GL_RGBA, GL_COLOR_ATTACHMENT0, 640, 480));
	CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));

	return true;
}

void CurvatureFlowParticleRenderer::setupThicknessGauss()
{
	float gaussData[64000];

	Filters::createGauss1D(_thicknessGaussSize, 1.0, _thicknessGaussSigma, gaussData);
	Filters::normalize(gaussData, 1, _thicknessGaussSize);
	CHECK_GL_CMD(_gaussDist1DTexture->load1DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, _thicknessGaussSize, 0, GL_DEPTH_COMPONENT, gaussData));
}

bool CurvatureFlowParticleRenderer::setupTextures()
{
	CHECK_GL_CMD(_waterLinDetphTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_waterDepthTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_waterThicknessTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_smoothedTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_smoothedTexture2 = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_edgeTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_thicknessAuxTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));

	float gaussData[64000];

	Filters::createGauss1D(_thicknessGaussSize, 1.0, _thicknessGaussSigma, gaussData);
	Filters::normalize(gaussData, 1, _thicknessGaussSize);
	CHECK_GL_CMD(_gaussDist1DTexture = Texture::create1DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, _thicknessGaussSize, 0, gaussData));

	return true;
}


void CurvatureFlowParticleRenderer::resize(int width, int height)
{
	// resize textures
	CHECK_GL_CMD(_waterLinDetphTexture->resize2D(width, height));
	CHECK_GL_CMD(_waterDepthTexture->resize2D(width, height));
	CHECK_GL_CMD(_waterThicknessTexture->resize2D((int)(width * _thicknessSize), (int)(height * _thicknessSize)));
	CHECK_GL_CMD(_thicknessAuxTexture->resize2D((int)(width * _thicknessSize), (int)(height * _thicknessSize)));
	CHECK_GL_CMD(_smoothedTexture->resize2D(width, height));
	CHECK_GL_CMD(_smoothedTexture2->resize2D(width, height));
	CHECK_GL_CMD(_edgeTexture->resize2D(width, height));

	// resize Frame Buffers
	CHECK_GL_CMD(_waterFB->resize(width, height));
	CHECK_GL_CMD(_waterThicknessFB->resize((int)(width * _thicknessSize), (int)(height * _thicknessSize)));
	CHECK_GL_CMD(_smoothFB->resize(width, height));
}

void CurvatureFlowParticleRenderer::render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData)
{
	CHECK_GL_CMD(_water->updateAttribute("vertex", particleData.particles, particleData.particleCount));
	CHECK_GL_CMD(_water->updateAttribute("density", particleData.particleDensity, particleData.particleCount));

	// render fluid thickness
	glViewport(0, 0, (int)(_scene->getWidth() * _thicknessSize), (int)(_scene->getHeight() * _thicknessSize));

	// scale scene depth texture to thickness texture size
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _waterThicknessFB->getId()));
	CHECK_GL_CMD(_waterThicknessFB->attachTexture2D(_thicknessAuxTexture, GL_DEPTH_ATTACHMENT));
	glClear(GL_DEPTH_BUFFER_BIT);
	_fillDepthBufferQuad->getShaderProgram()->useThis();
	CHECK_GL_CMD(_fillDepthBufferQuad->attachTexture(sceneDepthTexture, GL_TEXTURE0));
	_fillDepthBufferQuad->render();

	// render thickness
	CHECK_GL_CMD(glEnable(GL_BLEND));
	CHECK_GL_CMD(glBlendEquation(GL_FUNC_ADD));
	CHECK_GL_CMD(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE));
	CHECK_GL_CMD(glDepthMask(false));

	_waterThicknessShader->useThis();
	CHECK_GL_CMD(_waterThicknessShader->setUniformMat4f("projectionMatrix", _scene->getProjectionMatrix()));
	CHECK_GL_CMD(_waterThicknessShader->setUniformMat4f("modelViewMatrix", _scene->getViewMatrix()));
	CHECK_GL_CMD(_waterThicknessShader->setUniform1f("pointSize", _particleSize));
	CHECK_GL_CMD(_waterThicknessShader->setUniform1f("particleDepth", _particleThickness));
	CHECK_GL_CMD(_water->render(particleData.particleCount, GL_POINTS, _waterThicknessShader));

	CHECK_GL_CMD(_waterThicknessFB->detachTexture2D(GL_DEPTH_ATTACHMENT));
	CHECK_GL_CMD(glDepthMask(true));
	CHECK_GL_CMD(glDisable(GL_BLEND));

	// blurr thickness texture 
	CHECK_GL_CMD(glClear(GL_DEPTH_BUFFER_BIT));
	CHECK_GL_CMD(_blurQuad->attachTexture(_waterThicknessTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_blurQuad->attachTexture(_gaussDist1DTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_blurQuad->getShaderProgram()->setUniform2f("coordStep", _thicknessSize / _scene->getWidth(), 0.0f));
	CHECK_GL_CMD(_blurQuad->render());
	CHECK_GL_CMD(_blurQuad->getShaderProgram()->setUniform2f("coordStep", 0.0f, _thicknessSize / _scene->getHeight()));
	CHECK_GL_CMD(_blurQuad->render());
	
	// render fluid into depth texture
	glViewport(0, 0, _scene->getWidth(), _scene->getHeight());
	_waterShader->useThis();	
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _waterFB->getId()));
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	CHECK_GL_CMD(_waterShader->setUniformMat4f("projectionMatrix", _scene->getProjectionMatrix()));
	CHECK_GL_CMD(_waterShader->setUniformMat4f("modelViewMatrix", _scene->getViewMatrix()));
	CHECK_GL_CMD(_waterShader->setUniform1f("pointSize", _particleSize));
	CHECK_GL_CMD(_water->render(particleData.particleCount, GL_POINTS, _waterShader));
	
	// smooth water
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _smoothFB->getId()));
	CHECK_GL_CMD(_smoothFB->attachTexture2D(_edgeTexture, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_edgeQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / _scene->getWidth(), 1.0 / _scene->getHeight()));
	CHECK_GL_CMD(_edgeQuad->getShaderProgram()->setUniform1f("treshold", _edgeTreshold));
	CHECK_GL_CMD(_edgeQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_edgeQuad->render());
	
	float Cx = 2.0 / _scene->getCtgFovX();
	float Cy = 2.0 / _scene->getCtgFovY();

	CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_edgeTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform1f("Cx", Cx));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform1f("Cy", Cy));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform1f("Cx_square", Cx*Cx));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform1f("Cy_square", Cy*Cy));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform2f("coordStep", 1.0 / _scene->getWidth(), 1.0 / _scene->getHeight()));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform2f("coordStepInv", _scene->getWidth(), _scene->getHeight()));
	CHECK_GL_CMD(_curvatureFlowQuad->getShaderProgram()->setUniform1f("timeStep", _timeStep));
	CHECK_GL_CMD(_curvatureFlowQuad->render());

	CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture2, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_curvatureFlowQuad->render());

	for (int i = 0; i <	_blurIterationCount; i++)
	{
		CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_smoothedTexture2, GL_TEXTURE0));
		CHECK_GL_CMD(_curvatureFlowQuad->render());

		CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture2, GL_COLOR_ATTACHMENT0));
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
		CHECK_GL_CMD(_curvatureFlowQuad->render());
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// put water together with rest of the scene
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	//CHECK_GL_CMD(_grayscaleIntermediateQuad->attachTexture(_thicknessAuxTexture, GL_TEXTURE0));
	//CHECK_GL_CMD(_grayscaleIntermediateQuad->render());
	
	CHECK_GL_CMD(_finalQuad->attachTexture(_waterDepthTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_finalQuad->attachTexture(sceneDepthTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_finalQuad->attachTexture(sceneColorTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_finalQuad->attachTexture(_waterThicknessTexture, GL_TEXTURE3));
	CHECK_GL_CMD(_finalQuad->attachTexture(_smoothedTexture2, GL_TEXTURE4));

	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform1f("ctg_fov_x", _scene->getCtgFovX()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform1f("ctg_fov_y", _scene->getCtgFovY()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform1f("far", _scene->getZFar()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform4f("lightDirection", _scene->getLightInEyeSpace()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / _scene->getWidth(), 1.0f / _scene->getHeight()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->useThis());
	CHECK_GL_CMD(_finalQuad->render());
	
}


const std::set<std::string>& CurvatureFlowParticleRenderer::getParameters()
{
	return _parameterNames;
}