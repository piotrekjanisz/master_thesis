#include "Scene.h"
#include "debug_utils.h"
#include "filters.h"
#include <utils/utils.h>
#include <surface_extraction\data_types.h>
#include <surface_extraction\Block.h>
#include "ConfigurationFactory.h"

#include <GL/glus.h>
#include <GL/glew.h>
#include <iostream>
#include <boost/make_shared.hpp>
#include <list>

using namespace std;

Scene::Scene()
	: _particleSize(100.0f), 
	_bilateralTreshold(0.002), 
	_bilateralGaussSigma(10.0), 
	_bilateralGaussSize(21), 
	_depthGaussSize(21),
	_depthGaussSigma(40.0),
	_additionalBlurPhases(0),
	_particleDepth(0.05f),
	_filterSizeMult(0.5f),
	_lightDirection(-1.0, -1.0, -1.0, 0.0)
{
	// don't put any opengl command here!!!
}

void Scene::reshape(int width, int height)
{
	AbstractScene::reshape(width, height);
	CHECK_GL_CMD(_sceneTexture->resize2D(width, height));
	CHECK_GL_CMD(_sceneDepthTexture->resize2D(width, height));
	CHECK_GL_CMD(_zTexture->resize2D(width, height));
	CHECK_GL_CMD(_screenQuadTexture->resize2D(width, height));
	CHECK_GL_CMD(_depthTexture->resize2D(width, height));
	CHECK_GL_CMD(_waterDepthTexture->resize2D(width, height));
	CHECK_GL_CMD(_smoothedTexture->resize2D(width, height));
	CHECK_GL_CMD(_smoothedTexture2->resize2D(width, height));
	CHECK_GL_CMD(_edgeTexture->resize2D(width, height));

	CHECK_GL_CMD(_sceneFrameBuffer->resize(width, height));
	CHECK_GL_CMD(_waterFrameBuffer->resize(width, height));
	CHECK_GL_CMD(_waterDepthFrameBuffer->resize(width, height));
	CHECK_GL_CMD(_smoothFrameBuffer->resize(width, height));
}

void Scene::changeBilateralTreshold(double change)
{
	_bilateralTreshold += change;
	_bilateralTreshold = Utils::clamp(_bilateralTreshold, 0.0, 1.0);

	Filters::createHeavisideDistribution(0.0, 1.0, _bilateralTreshold, 5000, _debugData);
	_spatialDistTexture->load1DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, 5000, 0, GL_DEPTH_COMPONENT, _debugData);

	DEBUG_PRINT_VAR(_bilateralTreshold);
}

void Scene::changeGauss(int sizeChange, double sigmaChange)
{
	_bilateralGaussSize = Utils::clamp<int>(_bilateralGaussSize + 2*sizeChange, 3, 101);
	_bilateralGaussSigma = Utils::clamp<double>(_bilateralGaussSigma + sigmaChange, 0.1, 1000.0);
	DEBUG_PRINT_VAR(_bilateralGaussSize);
	DEBUG_PRINT_VAR(_bilateralGaussSigma);

	Filters::createGauss1D(_bilateralGaussSize, 1.0, _bilateralGaussSigma, _debugData);
	//Filters::normalize(_debugData, 1, _bilateralGaussSize);
	CHECK_GL_CMD(_gaussDist1DBilateralTexture->load1DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, _bilateralGaussSize, 0, GL_DEPTH_COMPONENT, _debugData));
	DEBUG_CODE(Utils::printArray(_debugData, _bilateralGaussSize, 1));
}

void Scene::changeDepthGauss(int sizeChange, double sigmaChange)
{
	_depthGaussSize = Utils::clamp(_depthGaussSize + 2*sizeChange, 3, 101);
	_depthGaussSigma = Utils::clamp(_depthGaussSigma + sigmaChange, 0.1, 1000.0);
	DEBUG_PRINT_VAR(_depthGaussSize);
	DEBUG_PRINT_VAR(_depthGaussSigma);

	Filters::createGauss1D(_depthGaussSize, 1.0, _depthGaussSigma, _debugData);
	Filters::normalize(_debugData, 1, _depthGaussSize);
	CHECK_GL_CMD(_gaussDist1DTexture->load1DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, _depthGaussSize, 0, GL_DEPTH_COMPONENT, _debugData));
	DEBUG_CODE(Utils::printArray(_debugData, _depthGaussSize, 1));
}

void Scene::changeAdditionalBlurPhases(int change)
{
	_additionalBlurPhases = Utils::clamp<int>(_additionalBlurPhases + change, 0, 10);
	DEBUG_PRINT_VAR(_additionalBlurPhases);
}

void Scene::changeParticleDepth(float change)
{
	_particleDepth = Utils::clamp(_particleDepth + change, 0.001f, 0.5f);
	DEBUG_PRINT_VAR(_particleDepth);
}

void Scene::changeFilterSizeMult(float change)
{
	_filterSizeMult = Utils::clamp(_filterSizeMult + change, 0.3f, 0.8f);
	DEBUG_PRINT_VAR(_filterSizeMult);
}

void Scene::rotateLightDir(float xrot, float yrot)
{
	vmml::vec3f lightDirection(_lightDirection.x(), _lightDirection.y(), _lightDirection.z());
	lightDirection = lightDirection.rotate(xrot, vmml::vec3f(1.0f, 0.0f, 0.0f));
	lightDirection = lightDirection.rotate(yrot, vmml::vec3f(0.0f, 1.0f, 0.0f));
	_lightDirection.x() = lightDirection.x();
	_lightDirection.y() = lightDirection.y();
	_lightDirection.z() = lightDirection.z();

	DEBUG_PRINT_VAR(_lightDirection);
}

int _ntriag = 0;
int _nvert = 0;


bool Scene::setupTextures()
{
	CHECK_GL_CMD(_sceneTexture = Texture::create2DRGBTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_sceneDepthTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_zTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_screenQuadTexture = Texture::create2DRGBTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_depthTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_waterDepthTexture = Texture::create2DRGBTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_smoothedTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_smoothedTexture2 = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_edgeTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	CHECK_GL_CMD(_floorTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_REPEAT, "textures/floor2.tga"));
	CHECK_GL_CMD(_floorNormalMapTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_REPEAT, "textures/floor2_normalmap.tga"));
	CHECK_GL_CMD(_boxTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_CLAMP_TO_EDGE, "textures/box.tga"));
	CHECK_GL_CMD(_skyBoxTexture = Texture::createCubeMap(GL_LINEAR, GL_CLAMP_TO_EDGE, "textures/sk3"));

	// filters
	const int GAUSS_SIZE = 21;
	const int HEAVISIDE_SIZE = 5000;
	float* gaussData = 0;
	Utils::ArrayHandle<float> handle(gaussData = new float[64000]);
	
	Filters::createGauss2D(GAUSS_SIZE, 1.0, 0.5, gaussData);
	_gaussDistTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, GAUSS_SIZE, GAUSS_SIZE, 0, gaussData);
	
	// gauss for smoothing water surface
	Filters::createGauss1D(GAUSS_SIZE, 1, _bilateralGaussSigma, gaussData);
	//Filters::normalize(gaussData, 1, _bilateralGaussSize);
	_gaussDist1DBilateralTexture = Texture::create1DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, _bilateralGaussSize, 0, gaussData);
	
	int filtersNum = Filters::createGauss1DArrayAsc(50, 0.2, gaussData);
	_maxFilter = filtersNum - 1;
	CHECK_GL_CMD(_gaussDistributionsArrayTexture = Texture::create1DDepthTextureArray(GL_LINEAR, GL_CLAMP_TO_EDGE, 50, filtersNum, gaussData));

	// texture with bilateral treshold
	Filters::createHeavisideDistribution(0.0, 1.0, _bilateralTreshold, HEAVISIDE_SIZE, gaussData);
	_spatialDistTexture = Texture::create1DDepthTexture(GL_NEAREST, GL_CLAMP_TO_EDGE, HEAVISIDE_SIZE, 0, gaussData);
	
	// gauss for smoothing water depth
	Filters::createGauss1D(_depthGaussSize, 1.0, _depthGaussSigma, gaussData);
	Filters::normalize(gaussData, 1, _depthGaussSize);
	_gaussDist1DTexture = Texture::create1DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, _depthGaussSize, 0, gaussData);

	return true;
}


bool Scene::setupFramebuffers()
{
	_sceneFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneTexture, GL_COLOR_ATTACHMENT0));
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneDepthTexture, GL_DEPTH_ATTACHMENT));

	_waterFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_waterFrameBuffer->attachTexture2D(_depthTexture, GL_DEPTH_ATTACHMENT));
	CHECK_GL_CMD(_waterFrameBuffer->attachTexture2D(_zTexture, GL_COLOR_ATTACHMENT0));

	_waterDepthFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_waterDepthFrameBuffer->attachTexture2D(_waterDepthTexture, GL_COLOR_ATTACHMENT0));

	_smoothFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_smoothFrameBuffer->attachRenderbuffer(GL_RGBA, GL_COLOR_ATTACHMENT0, 640, 480));
	CHECK_GL_CMD(_smoothFrameBuffer->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
	return true;
}


bool Scene::setupShaders()
{
	try {
		_shaderProgram = boost::make_shared<ShaderProgram>();
		_shaderProgram->load("shaders/vertex.glsl", "shaders/fragment.glsl");
		CHECK_GL_CMD(_shaderProgram->bindFragDataLocation(0, "fragColor"));
		CHECK_GL_CMD(_shaderProgram->setUniform1i("tex", 0));
		
		_normalMapShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_normalMapShader->load("shaders/normal_map_vertex.glsl", "shaders/normal_map_fragment.glsl"));
		CHECK_GL_CMD(_normalMapShader->setUniform1i("tex", 0));
		CHECK_GL_CMD(_normalMapShader->setUniform1i("normal_map", 1));
		_normalMapShader->printParameters();

		_isoSurfaceProgram = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_isoSurfaceProgram->load("shaders/normalization_vertex.glsl", "shaders/water_surface_fragment.glsl"));
		CHECK_GL_CMD(_isoSurfaceProgram->bindFragDataLocation(0, "fragColor"));
		CHECK_GL_CMD(_isoWaterProjectionLocation = _isoSurfaceProgram->getUniformLocation("projectionMatrix"));
		CHECK_GL_CMD(_isoWaterModelViewLocation = _isoSurfaceProgram->getUniformLocation("modelViewMatrix"));
		CHECK_GL_CMD(_isoWaterNormalLocation = _isoSurfaceProgram->getUniformLocation("normalMatrix"));

		ShaderProgramPtr screenQuadShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(screenQuadShader->load("shaders/screen_quad_vertex.glsl", "shaders/separable_bilateral_gauss_fragment.glsl"));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("inputImage", 0));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("gaussianDist", 1));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("spatialDist", 2));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("linearDepth", 3));
		CHECK_GL_CMD(screenQuadShader->bindFragDataLocation(0, "frag_color"));
		//_screenQuad = boost::make_shared<ScreenQuad>(screenQuadShader);

		_bilateralGaussSmoothShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(screenQuadShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->load("shaders/screen_quad_vertex.glsl", "shaders/separable_bilateral_gauss_pointsize_fragment.glsl"));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("inputImage", 0));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("gaussianDist", 1));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("spatialDist", 2));
		CHECK_GL_CMD(_bilateralGaussSmoothShader->setUniform1i("linearDepth", 3));
		_screenQuad = boost::make_shared<ScreenQuad>(_bilateralGaussSmoothShader);

		_edgeDetectionShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_edgeDetectionShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_edgeDetectionShader->load("shaders/screen_quad_vertex.glsl", "shaders/edge_detection_fragment.glsl"));
		CHECK_GL_CMD(_edgeDetectionShader->setUniform1f("treshold", 0.015f));
		_edgeQuad = boost::make_shared<ScreenQuad>(_edgeDetectionShader);

		_finalShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_finalShader->load("shaders/screen_quad_vertex.glsl", "shaders/final_stage_fragment.glsl"));
		CHECK_GL_CMD(_finalShader->setUniform1i("depthTexture", 0));
		CHECK_GL_CMD(_finalShader->setUniform1i("sceneDepthTexture", 1));
		CHECK_GL_CMD(_finalShader->setUniform1i("sceneTexture", 2));
		CHECK_GL_CMD(_finalShader->setUniform1i("waterDepthTexture", 3));
		CHECK_GL_CMD(_inverseProjectionLocation = _finalShader->getUniformLocation("inverseProjection"));
		CHECK_GL_CMD(_finalShader->bindFragDataLocation(0, "frag_color"));
		_finalQuad = boost::make_shared<ScreenQuad>(_finalShader);

		_skyBoxShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_skyBoxShader->load("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl"));
		CHECK_GL_CMD(_skyBoxShader->setUniform1i("cubeMap", 0));
		CHECK_GL_CMD(_skyBoxShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_skyBoxProjectionLocation = _skyBoxShader->getUniformLocation("projectionMatrix"));
		CHECK_GL_CMD(_skyBoxModelViewLocation = _skyBoxShader->getUniformLocation("modelViewMatrix"));

		_waterShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_waterShader->load("shaders/water_shader_vertex.glsl", "shaders/water_shader_fragment.glsl"));
		CHECK_GL_CMD(_waterProjectionLocation = _waterShader->getUniformLocation("projectionMatrix"));
		CHECK_GL_CMD(_waterModelViewLocation = _waterShader->getUniformLocation("modelViewMatrix"));
		CHECK_GL_CMD(_waterShader->bindFragDataLocation(0, "fragColor"));

		_waterDepthShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_waterDepthShader->load("shaders/water_depth_vertex.glsl", "shaders/water_depth_fragment.glsl"));
		CHECK_GL_CMD(_waterDepthProjectionLocation = _waterDepthShader->getUniformLocation("projectionMatrix"));
		CHECK_GL_CMD(_waterDepthModelViewLocation = _waterDepthShader->getUniformLocation("modelViewMatrix"));
		CHECK_GL_CMD(_waterDepthShader->bindFragDataLocation(0, "fragColor"));

		_gaussianBlurShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_gaussianBlurShader->load("shaders/screen_quad_vertex.glsl", "shaders/gaussian_blur_fragment.glsl"));
		CHECK_GL_CMD(_gaussianBlurShader->setUniform1i("inputImage", 0));
		CHECK_GL_CMD(_gaussianBlurShader->setUniform1i("gaussianDist", 1));
		CHECK_GL_CMD(_gaussianBlurShader->bindFragDataLocation(0, "frag_color"));
		_blurQuad = boost::make_shared<ScreenQuad>(_gaussianBlurShader);

		_grayscaleTextureShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_grayscaleTextureShader->load("shaders/screen_quad_vertex.glsl", "shaders/show_texture_grayscale_fragment.glsl"));
		CHECK_GL_CMD(_grayscaleTextureShader->setUniform1i("texture1", 0));
		CHECK_GL_CMD(_grayscaleTextureShader->bindFragDataLocation(0, "frag_color"));
		_grayscaleIntermediateQuad = boost::make_shared<ScreenQuad>(_grayscaleTextureShader);
	} catch (const BaseException& ex) {
		cout << ex.what() << endl;
		fgetc(stdin);
		return false;
	}
	return true;
}


bool Scene::setupObjects()
{
	_water = boost::make_shared<GfxObject>();
	CHECK_GL_CMD(_water->addAttribute("vertex", 0, 70000, 4, GfxObject::DYNAMIC_ATTR));
	CHECK_GL_CMD(_water->addShader(_waterShader));
	CHECK_GL_CMD(_water->addShader(_waterDepthShader));

	_isoWater = boost::make_shared<GfxObject>();
	CHECK_GL_CMD(_isoWater->addAttribute("vertex", 0, 140000, 4, GfxObject::DYNAMIC_ATTR));
	CHECK_GL_CMD(_isoWater->addAttribute("normal", 0, 140000, 3, GfxObject::DYNAMIC_ATTR));
	CHECK_GL_CMD(_water->addShader(_isoSurfaceProgram));

	try {
		_box = boost::make_shared<GfxStaticObject>(_shaderProgram);
		_plane = boost::make_shared<GfxStaticObject>(_normalMapShader);
		_skyBox = boost::make_shared<GfxStaticObject>(_skyBoxShader);
		ShapePtr plane = ShapeFactory().createPlane(100.0f, 100.0);
		ShapePtr box = ShapeFactory().createBox(0.5f);
		ShapePtr skyBox = ShapeFactory().createSkyBox(50.0f);
		CHECK_GL_CMD(_box->createFromShape(box));
		CHECK_GL_CMD(_plane->createFromShape(plane));
		CHECK_GL_CMD(_skyBox->createFromShape(skyBox));
	} catch(const GfxException& ex) {
		cout << ex.what() << endl;
		fgetc(stdin);
		return false;
	}

	return true;
}


bool Scene::setup()
{
	if (!AbstractScene::setup()) {
		return false;
	}

	SurfaceExtractorDesc desc;
	ConfigurationFactory configurationFactory("config");
	desc = configurationFactory.createSurfaceExtractorDesc("extractor");

	_currentOutput = 0;

	_surfaceExtractor = boost::make_shared<SurfaceExtractor>(desc);
	_mtSurfaceExtractor = boost::make_shared<MtSurfaceExtractor>(desc);
	_block = boost::make_shared<Block>(3000, desc.xMin, desc.xMax, desc.yMin, desc.yMax, desc.zMin, desc.zMax, desc.rc, desc.cubeSize);

	_debugData = new float[640*480*4];
	
	translate(0.0f, 1.5f, -0.5f);
	rotateX(-0.4f);

	translate(0.0f, 0.0f, -5.0f);

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

void Scene::render()
{
	setupMatrixes();
	_shaderProgram->useThis();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(_projectionLocation, 1, GL_FALSE, _projectionMatrix);
	glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, _viewMatrix);
	glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(_viewMatrix));
	glUniform4fv(_colorLocation, 1, vmml::vec4f(0.9f, 0.9f, 0.9f, 1.0f));
	_box->render();
	glUniform4fv(_colorLocation, 1, vmml::vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	_plane->render();
}

void Scene::renderIsoSurface(NxScene* physicsScene)
{
	setupMatrixes();
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CHECK_GL_CMD(_skyBoxShader->useThis());
	CHECK_GL_CMD(glUniformMatrix4fv(_skyBoxProjectionLocation, 1, GL_FALSE, _projectionMatrix));
	CHECK_GL_CMD(glUniformMatrix4fv(_skyBoxModelViewLocation, 1, GL_FALSE, _viewMatrix));
	CHECK_GL_CMD(_skyBoxTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_skyBox->render());

	_shaderProgram->useThis();

	CHECK_GL_CMD(glUniformMatrix4fv(_projectionLocation, 1, GL_FALSE, _projectionMatrix));
	CHECK_GL_CMD(glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, _viewMatrix));
	CHECK_GL_CMD(glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(_viewMatrix)));
	CHECK_GL_CMD(_floorTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_plane->render());

	int nbActors = physicsScene->getNbActors();
	NxActor** actors = physicsScene->getActors();

	// render boxes
	_boxTexture->bindToTextureUnit(GL_TEXTURE0);
	vmml::mat4f modelMatrix;
	vmml::mat4f modelViewMatrix;
	while(nbActors--) {
		NxActor* actor = *actors++;
		if (!actor->userData) 
			continue;

		actor->getGlobalPose().getColumnMajor44(modelMatrix);
		modelViewMatrix = _viewMatrix * modelMatrix;
		glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, modelViewMatrix);
		glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(modelViewMatrix));
		_box->render();
	}

	NxFluid** fluids = physicsScene->getFluids();
	int nbFluids = physicsScene->getNbFluids();

	for(int i = 0; i < nbFluids; i++) {
		NxFluid* fluid = fluids[i];
		MyFluid* myFluid = (MyFluid*)fluid->userData;
		if (myFluid) {
			_outputs[_currentOutput].clear();
			list<TriangleMesh> meshes;
			_mtSurfaceExtractor->waitForResults();
			float* positions = myFluid->getPositionsAsync();
			int particleCount = myFluid->getParticlesCountAsync();
			_mtSurfaceExtractor->extractSurface(positions, particleCount, 4, &_outputs[_currentOutput]);
			_currentOutput = (_currentOutput + 1) % 2;
			CHECK_GL_CMD(_isoSurfaceProgram->useThis());
			CHECK_GL_CMD(glUniformMatrix4fv(_isoWaterProjectionLocation, 1, GL_FALSE, _projectionMatrix));
			CHECK_GL_CMD(glUniformMatrix4fv(_isoWaterModelViewLocation, 1, GL_FALSE, _viewMatrix));
			CHECK_GL_CMD(glUniformMatrix3fv(_isoWaterNormalLocation, 1, GL_FALSE, getNormalMatrix(_viewMatrix)));
			for (list<TriangleMesh>::iterator it = _outputs[_currentOutput].begin(); it != _outputs[_currentOutput].end(); ++it) {
				CHECK_GL_CMD(_isoWater->updateAttribute("vertex", it->vertices, it->verticesCount));
				CHECK_GL_CMD(_isoWater->updateAttribute("normal", it->normals, it->verticesCount));
				CHECK_GL_CMD(_isoWater->renderElements(_isoSurfaceProgram, GL_TRIANGLES, it->trianglesCount*3, it->indices));			
			}
		}
	}

	computeFrameRate();
}

void Scene::render(NxScene* physicsScene)
{
	setupMatrixes();

	vmml::vec4f lightDirectionEyeSpace = getLightInEyeSpace();

	// render scene
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _sceneFrameBuffer->getId()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// render skybox
	CHECK_GL_CMD(_skyBoxShader->useThis());
	CHECK_GL_CMD(_skyBoxShader->setUniformMat4f("projectionMatrix", _projectionMatrix));
	CHECK_GL_CMD(_skyBoxShader->setUniformMat4f("modelViewMatrix", _viewMatrix));
	CHECK_GL_CMD(_skyBoxTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_skyBox->render());

	/*
	_shaderProgram->useThis();
	CHECK_GL_CMD(_shaderProgram->setUniformMat4f("projectionMatrix", _projectionMatrix));
	CHECK_GL_CMD(_shaderProgram->setUniformMat4f("modelViewMatrix", _viewMatrix));
	CHECK_GL_CMD(_shaderProgram->setUniformMat3f("normalMatrix", getNormalMatrix(_viewMatrix)));
	CHECK_GL_CMD(_shaderProgram->setUniform4f("lightDirection", lightDirectionEyeSpace));
	CHECK_GL_CMD(_floorTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_plane->render());
	*/
	
	CHECK_GL_CMD(_plane->getShader()->setUniformMat4f("modelViewProjectionMatrix", _modelViewProjectionMatrix));
	CHECK_GL_CMD(_plane->getShader()->setUniform3f("lightDirection", _lightDirection));
	CHECK_GL_CMD(_floorTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_floorNormalMapTexture->bindToTextureUnit(GL_TEXTURE1));
	CHECK_GL_CMD(_plane->render());
	

	int nbActors = physicsScene->getNbActors();
	NxActor** actors = physicsScene->getActors();

	_shaderProgram->useThis();
	CHECK_GL_CMD(_shaderProgram->setUniformMat4f("projectionMatrix", _projectionMatrix));
	CHECK_GL_CMD(_shaderProgram->setUniform4f("lightDirection", lightDirectionEyeSpace));
	// render boxes
	_boxTexture->bindToTextureUnit(GL_TEXTURE0);
	vmml::mat4f modelMatrix;
	vmml::mat4f modelViewMatrix;
	while(nbActors--) {
		NxActor* actor = *actors++;
		if (!actor->userData) 
			continue;

		actor->getGlobalPose().getColumnMajor44(modelMatrix);
		modelViewMatrix = _viewMatrix * modelMatrix;
		CHECK_GL_CMD(_box->getShader()->setUniformMat4f("modelViewMatrix", modelViewMatrix));
		CHECK_GL_CMD(_box->getShader()->setUniformMat3f("normalMatrix", getNormalMatrix(modelViewMatrix)));
		CHECK_GL_CMD(_box->render());
	}

	// render fluids into depth buffer
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _waterFrameBuffer->getId()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	NxFluid** fluids = physicsScene->getFluids();
	int nbFluids = physicsScene->getNbFluids();

	for(int i = 0; i < nbFluids; i++) {
		NxFluid* fluid = fluids[i];
		MyFluid* myFluid = (MyFluid*)fluid->userData;
		if (myFluid) {
			_particleCount = myFluid->getParticlesCount();
			_waterShader->useThis();
			/*
			const int PART_COUNT = 2;
			float particles[] = {
				0.0f, 5.0f, 0.0f, 1.0f,
				0.0f, 5.2f, 0.0f, 1.0f,
			};
			CHECK_GL_CMD(glUniformMatrix4fv(_waterProjectionLocation, 1, GL_FALSE, _projectionMatrix));
			CHECK_GL_CMD(glUniformMatrix4fv(_waterModelViewLocation, 1, GL_FALSE, _viewMatrix));
			CHECK_GL_CMD(_waterShader->setUniform1f("pointSize", _particleSize));
			CHECK_GL_CMD(_water->updateAttribute("vertex", particles, PART_COUNT));
			CHECK_GL_CMD(_water->render(PART_COUNT, GL_POINTS, _waterShader));
			*/
			
			CHECK_GL_CMD(glUniformMatrix4fv(_waterProjectionLocation, 1, GL_FALSE, _projectionMatrix));
			CHECK_GL_CMD(glUniformMatrix4fv(_waterModelViewLocation, 1, GL_FALSE, _viewMatrix));
			CHECK_GL_CMD(_waterShader->setUniform1f("pointSize", _particleSize));
			CHECK_GL_CMD(_water->updateAttribute("vertex", myFluid->getPositions(), myFluid->getParticlesCount()));
			CHECK_GL_CMD(_water->render(myFluid->getParticlesCount(), GL_POINTS, _waterShader));
			

			CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _waterDepthFrameBuffer->getId()));
			CHECK_GL_CMD(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			CHECK_GL_CMD(glEnable(GL_BLEND));
			CHECK_GL_CMD(glBlendEquation(GL_FUNC_ADD));
			CHECK_GL_CMD(glBlendFunc(GL_ONE, GL_ONE));
			CHECK_GL_CMD(glDisable(GL_DEPTH_TEST));

			_waterDepthShader->useThis();
			CHECK_GL_CMD(glUniformMatrix4fv(_waterDepthProjectionLocation, 1, GL_FALSE, _projectionMatrix));
			CHECK_GL_CMD(glUniformMatrix4fv(_waterDepthModelViewLocation, 1, GL_FALSE, _viewMatrix));
			CHECK_GL_CMD(_waterDepthShader->setUniform1f("pointSize", _particleSize));
			CHECK_GL_CMD(_waterDepthShader->setUniform1f("particleDepth", _particleDepth));
			//CHECK_GL_CMD(_water->render(PART_COUNT, GL_POINTS, _waterDepthShader));
			CHECK_GL_CMD(_water->render(myFluid->getParticlesCount(), GL_POINTS, _waterDepthShader));

			CHECK_GL_CMD(glEnable(GL_DEPTH_TEST));
			CHECK_GL_CMD(glDisable(GL_BLEND));

			CHECK_GL_CMD(glClear(GL_DEPTH_BUFFER_BIT));
			CHECK_GL_CMD(_blurQuad->attachTexture(_waterDepthTexture, GL_TEXTURE0));
			CHECK_GL_CMD(_blurQuad->attachTexture(_gaussDist1DTexture, GL_TEXTURE1));
			CHECK_GL_CMD(_blurQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 0.0f));
			CHECK_GL_CMD(_blurQuad->render());
			//CHECK_GL_CMD(glClear(GL_DEPTH_BUFFER_BIT));
			CHECK_GL_CMD(_blurQuad->getShaderProgram()->setUniform2f("coordStep", 0.0f, 1.0f / getHeight()));
			CHECK_GL_CMD(_blurQuad->render());
		}
	}

	// blur phase
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _smoothFrameBuffer->getId()));
	CHECK_GL_CMD(_smoothFrameBuffer->attachTexture2D(_smoothedTexture, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_screenQuad->attachTexture(_depthTexture, GL_TEXTURE0));
	//CHECK_GL_CMD(_screenQuad->attachTexture(_gaussDist1DBilateralTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->attachTexture(_gaussDistributionsArrayTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->attachTexture(_spatialDistTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_screenQuad->attachTexture(_zTexture, GL_TEXTURE3));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 0.0f));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1f("farDist", getZFar()));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1i("maxPointSize", _particleSize));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1f("filterSizeMult", _filterSizeMult));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1i("maxFilter", _maxFilter));
	CHECK_GL_CMD(_screenQuad->render());

	CHECK_GL_CMD(_smoothFrameBuffer->attachTexture2D(_smoothedTexture2, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	
	CHECK_GL_CMD(_screenQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 0.0f, 1.0f / getHeight()));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform1f("farDist", getZFar()));
	CHECK_GL_CMD(_screenQuad->render());


	/*
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _smoothFrameBuffer->getId()));
	CHECK_GL_CMD(_smoothFrameBuffer->attachTexture2D(_edgeTexture, GL_COLOR_ATTACHMENT0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_edgeQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 1.0 / getHeight()));
	CHECK_GL_CMD(_edgeQuad->attachTexture(_zTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_edgeQuad->render());
	*/


	// put water together with rest of the scene
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	/*
	CHECK_GL_CMD(_grayscaleIntermediateQuad->attachTexture(_smoothedTexture2, GL_TEXTURE0));
	CHECK_GL_CMD(_grayscaleIntermediateQuad->render());
	*/
	
	CHECK_GL_CMD(_finalQuad->attachTexture(_smoothedTexture2, GL_TEXTURE0));
	//CHECK_GL_CMD(_finalQuad->attachTexture(_depthTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_finalQuad->attachTexture(_sceneDepthTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_finalQuad->attachTexture(_sceneTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_finalQuad->attachTexture(_waterDepthTexture, GL_TEXTURE3));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform4f("lightDirection", lightDirectionEyeSpace));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 1.0f / getHeight()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->useThis());
	CHECK_GL_CMD(glUniformMatrix4fv(_inverseProjectionLocation, 1, GL_FALSE, _inverseProjectionMatrix));
	CHECK_GL_CMD(_finalQuad->render());

	computeFrameRate();
}

void Scene::displayAdditionalStats()
{
	std::cout << "PARTICLES: " << _particleCount << std::endl;
}


