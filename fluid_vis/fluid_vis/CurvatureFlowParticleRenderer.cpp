#include "CurvatureFlowParticleRenderer.h"
#include "debug_utils.h"
#include "filters.h"
#include <vmmlib\vmmlib.hpp>
#include <iostream>

const std::string CurvatureFlowParticleRenderer::PARAM_BLUR_ITERATION_COUNT("blur iteration count");
const std::string CurvatureFlowParticleRenderer::PARAM_TIME_STEP("time step");
const std::string CurvatureFlowParticleRenderer::PARAM_EDGE_TRESHOLD("edge trehold");

CurvatureFlowParticleRenderer::CurvatureFlowParticleRenderer(AbstractScene* scene)
	: ScreenSpaceParticleRenderer(scene), 
	_blurIterationCount(50),
	_timeStep(-0.00000001f),
	_edgeTreshold(0.003f)
{
	_parameterNames.insert(PARAM_BLUR_ITERATION_COUNT);
	_parameterNames.insert(PARAM_EDGE_TRESHOLD);
	_parameterNames.insert(PARAM_TIME_STEP);

	_blurIterationCount = _description.blurIterations;
	_timeStep = _description.timeStep;
	_edgeTreshold = _description.edgeTreshold;
}

#define PRINT_PARAM(param) std::cout << parameter << ": " << param << std::endl;

bool CurvatureFlowParticleRenderer::changeParameter(const std::string& parameter, ParamOperation operation)
{
	int sign = operation == ParamOperation::INC ? 1 : -1;

	if (parameter == PARAM_BLUR_ITERATION_COUNT) {
		_blurIterationCount += sign * 1;
		PRINT_PARAM(_blurIterationCount);
	} else if (parameter == PARAM_EDGE_TRESHOLD) {
		_edgeTreshold += sign * 0.0001;
		PRINT_PARAM(_edgeTreshold);
	} else if (parameter == PARAM_TIME_STEP) {
		_timeStep += sign * (-0.0000000001f);
		PRINT_PARAM(_timeStep);
	} else {
		return ScreenSpaceParticleRenderer::changeParameter(parameter, operation);
	}

	return true;
}


CurvatureFlowParticleRenderer::~CurvatureFlowParticleRenderer(void)
{
}

bool CurvatureFlowParticleRenderer::setup()
{
	if (! ScreenSpaceParticleRenderer::setup())
		return false;

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
		_edgeDetectionShader = ShaderProgram::create();
		CHECK_GL_CMD(_edgeDetectionShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_edgeDetectionShader->load("shaders/screen_quad_vertex.glsl", "shaders/edge_detection_fragment.glsl"));
		CHECK_GL_CMD(_edgeDetectionShader->setUniform1f("treshold", 0.01f));

		_curvatureFlowShader = ShaderProgram::create();
		CHECK_GL_CMD(_curvatureFlowShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_curvatureFlowShader->load("shaders/screen_quad_vertex.glsl", "shaders/curvature_flow_fragment.glsl"));
		CHECK_GL_CMD(_curvatureFlowShader->setUniform1i("depthTexture", 0));
		CHECK_GL_CMD(_curvatureFlowShader->setUniform1i("edgeTexture", 1));

		_derivativeShader = ShaderProgram::create();
		CHECK_GL_CMD(_derivativeShader->bindFragDataLocation(0, "dx"));
		CHECK_GL_CMD(_derivativeShader->bindFragDataLocation(1, "dy"));
		CHECK_GL_CMD(_derivativeShader->load("shaders/screen_quad_vertex.glsl", "shaders/derivative_fragment.glsl"));
		CHECK_GL_CMD(_derivativeShader->setUniform1i("input_image", 0));

		_curvatureFlowShader2 = ShaderProgram::create();
		CHECK_GL_CMD(_curvatureFlowShader2->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_curvatureFlowShader2->load("shaders/screen_quad_vertex.glsl", "shaders/curvature_flow_fragment.glsl"));
		CHECK_GL_CMD(_curvatureFlowShader2->setUniform1i("depthTexture", 0));
		CHECK_GL_CMD(_curvatureFlowShader2->setUniform1i("edgeTexture", 1));
		CHECK_GL_CMD(_curvatureFlowShader2->setUniform1i("dxTexture", 2));
		CHECK_GL_CMD(_curvatureFlowShader2->setUniform1i("dyTexture", 3));

	} catch (const BaseException& ex) {
		std::cout << ex.what() << std::endl;
		fgetc(stdin);
		return false;
	}

	return true;
}


bool CurvatureFlowParticleRenderer::setupObjects()
{
	_edgeQuad = ScreenQuad::create(_edgeDetectionShader);
	_curvatureFlowQuad = ScreenQuad::create(_curvatureFlowShader);

	_curvatureFlowQuad2 = ScreenQuad::create(_curvatureFlowShader2);
	_derivativeQuad = ScreenQuad::create(_derivativeShader);

	return true;
}


bool CurvatureFlowParticleRenderer::setupFramebuffers()
{
	_smoothFB = FrameBuffer::create();

	return true;
}

bool CurvatureFlowParticleRenderer::setupTextures()
{
	CHECK_GL_CMD(_smoothedTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_edgeTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));

	CHECK_GL_CMD(_dxTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_dyTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	return true;
}


void CurvatureFlowParticleRenderer::resize(int width, int height)
{
	ScreenSpaceParticleRenderer::resize(width, height);

	// resize textures
	CHECK_GL_CMD(_smoothedTexture->resize2D(width, height));
	CHECK_GL_CMD(_edgeTexture->resize2D(width, height));

	// resize Frame Buffers
	CHECK_GL_CMD(_smoothFB->resize(width, height));
}

void CurvatureFlowParticleRenderer::smoothWaterTexture()
{
	// smooth water
	CHECK_GL_CMD(_smoothFB->bind());
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
	glClear(GL_DEPTH_BUFFER_BIT);

	CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
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

	CHECK_GL_CMD(_smoothFB->attachTexture2D(_waterLinDetphTexture, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT);
	CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_curvatureFlowQuad->render());

	for (int i = 0; i <	_blurIterationCount; i++)
	{
		CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
		glClear(GL_DEPTH_BUFFER_BIT);
		CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
		CHECK_GL_CMD(_curvatureFlowQuad->render());

		CHECK_GL_CMD(_smoothFB->attachTexture2D(_waterLinDetphTexture, GL_COLOR_ATTACHMENT0));
		glClear(GL_DEPTH_BUFFER_BIT);
		CHECK_GL_CMD(_curvatureFlowQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
		CHECK_GL_CMD(_curvatureFlowQuad->render());
	}
}

//void CurvatureFlowParticleRenderer::smoothWaterTexture()
//{
//	// smooth water
//	CHECK_GL_CMD(_smoothFB->bind());
//	CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(1));
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_edgeTexture, GL_COLOR_ATTACHMENT0));
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	CHECK_GL_CMD(_edgeQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / _scene->getWidth(), 1.0 / _scene->getHeight()));
//	CHECK_GL_CMD(_edgeQuad->getShaderProgram()->setUniform1f("treshold", _edgeTreshold));
//	CHECK_GL_CMD(_edgeQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
//	CHECK_GL_CMD(_edgeQuad->render());
//	
//	float Cx = 2.0 / _scene->getCtgFovX();
//	float Cy = 2.0 / _scene->getCtgFovY();
//
//	// ----------------------------------------------
//	CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(2));
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_dxTexture, GL_COLOR_ATTACHMENT0));
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_dyTexture, GL_COLOR_ATTACHMENT1));
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	CHECK_GL_CMD(_derivativeQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
//	CHECK_GL_CMD(_derivativeQuad->render());
//	
//	CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(1));
//	CHECK_GL_CMD(_smoothFB->detachTexture2D(GL_COLOR_ATTACHMENT1));
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
//	glClearColor(1.0, 1.0, 1.0, 1.0);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
//	CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_edgeTexture, GL_TEXTURE1));
//	CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_dxTexture, GL_TEXTURE2));
//	CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_dyTexture, GL_TEXTURE3));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform1f("Cx", Cx));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform1f("Cy", Cy));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform1f("Cx_square", Cx*Cx));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform1f("Cy_square", Cy*Cy));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform2f("coordStep", 1.0 / _scene->getWidth(), 1.0 / _scene->getHeight()));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform2f("coordStepInv", _scene->getWidth(), _scene->getHeight()));
//	CHECK_GL_CMD(_curvatureFlowQuad2->getShaderProgram()->setUniform1f("timeStep", _timeStep));
//	CHECK_GL_CMD(_curvatureFlowQuad2->render());
//
//	CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(2));
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_dxTexture, GL_COLOR_ATTACHMENT0));
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_dyTexture, GL_COLOR_ATTACHMENT1));
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	CHECK_GL_CMD(_derivativeQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
//	CHECK_GL_CMD(_derivativeQuad->render());
//
//	CHECK_GL_CMD(_smoothFB->detachTexture2D(GL_COLOR_ATTACHMENT1));
//	CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(1));	
//	CHECK_GL_CMD(_smoothFB->attachTexture2D(_waterLinDetphTexture, GL_COLOR_ATTACHMENT0));
//	glClearColor(1.0, 1.0, 1.0, 1.0);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_smoothedTexture, GL_TEXTURE0));
//	CHECK_GL_CMD(_curvatureFlowQuad2->render());
//	// ---------------------------
//
//	for (int i = 0; i <	_blurIterationCount; i++)
//	{
//		CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(2));
//		CHECK_GL_CMD(_smoothFB->attachTexture2D(_dxTexture, GL_COLOR_ATTACHMENT0));
//		CHECK_GL_CMD(_smoothFB->attachTexture2D(_dyTexture, GL_COLOR_ATTACHMENT1));
//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//		CHECK_GL_CMD(_derivativeQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
//		CHECK_GL_CMD(_derivativeQuad->render());
//
//		CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(1));
//		CHECK_GL_CMD(_smoothFB->detachTexture2D(GL_COLOR_ATTACHMENT1));
//		CHECK_GL_CMD(_smoothFB->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
//		glClearColor(1.0, 1.0, 1.0, 1.0);
//		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//		CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
//		CHECK_GL_CMD(_curvatureFlowQuad2->render());
//
//		CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(2));
//		CHECK_GL_CMD(_smoothFB->attachTexture2D(_dxTexture, GL_COLOR_ATTACHMENT0));
//		CHECK_GL_CMD(_smoothFB->attachTexture2D(_dyTexture, GL_COLOR_ATTACHMENT1));
//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//		CHECK_GL_CMD(_derivativeQuad->attachTexture(_waterLinDetphTexture, GL_TEXTURE0));
//		CHECK_GL_CMD(_derivativeQuad->render());
//
//		CHECK_GL_CMD(_smoothFB->mapDefaultOutputs(1));
//		CHECK_GL_CMD(_smoothFB->detachTexture2D(GL_COLOR_ATTACHMENT1));
//		CHECK_GL_CMD(_smoothFB->attachTexture2D(_waterLinDetphTexture, GL_COLOR_ATTACHMENT0));
//		glClearColor(1.0, 1.0, 1.0, 1.0);
//		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//		CHECK_GL_CMD(_curvatureFlowQuad2->attachTexture(_smoothedTexture, GL_TEXTURE0));
//		CHECK_GL_CMD(_curvatureFlowQuad2->render());
//	}
//}

const std::set<std::string>& CurvatureFlowParticleRenderer::getParameters()
{
	return _parameterNames;
}