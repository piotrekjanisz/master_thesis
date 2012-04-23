#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "GLUS.lib")

#ifdef DEBUG_BUILD
#pragma comment(lib, "boost_regex-vc100-mt-gd-1_47")
#else
#pragma comment(lib, "boost_regex-vc100-mt-1_47")
#endif


#include <NxCooking.h>
#include <NxPhysics.h>
#include <GL/glew.h>
#include <GL/glus.h>
#include <iostream>
#include <boost/make_shared.hpp>
//#include <fluid_vis/Scene.h>
#include <fluid_vis/Scene2.h>
#include <fluid_vis/ErrorStream.h>
#include <fluid_vis/physx_utils.h>
#include <fluid_vis/Properties.h>
#include <fluid_vis/ConfigurationFactory.h>
#include <fluid_vis/MyFluid.h>
#include <fluid_vis/debug_utils.h>
#include <fluid_vis/CurvatureFlowParticleRenderer.h>
#include <fluid_vis/ScreenSpaceParticleRenderer.h>
#include <fluid_vis/BilateralGaussParticleRenderer.h>
#include <fluid_vis/IsosurfaceParticleRenderer.h>

using namespace std;


enum RenderingMode {
	BILATERAL_GAUSS,
	CURVATURE_FLOW,
	ISOSURFACE_EXTRACTION
};

//Scene g_scene;
Scene2 g_scene2;

static NxPhysicsSDK* gPhysicsSDK = NULL;
static NxScene* g_NxScene = NULL;
static MyFluid* gFluid = NULL;
static ConfigurationFactory configurationFactory("config");

ParameterControllerPtr g_renderingParameterController;

RenderingMode g_renderingMode;

bool g_gpuAccelerationPossible = false;

bool g_useGPUAcceleration = true;

int g_screenWidth;
int g_screenHeight;

int g_maxParticles;

void createFluid() 
{	
	try {
		NxFluidDesc fluidDesc = configurationFactory.createFluidDesc("water1");
		if (g_gpuAccelerationPossible && g_useGPUAcceleration) {
			fluidDesc.flags |= NX_FF_HARDWARE;
		} else {
			fluidDesc.flags &= ~NX_FF_HARDWARE;
		}

		gFluid = new MyFluid(g_NxScene, fluidDesc, NxVec3(1.0f, 0.0f, 0.0f), 100.0f);

		NxFluidEmitterDesc emitterDesc = configurationFactory.createFluidEmitterDesc("emitter1");

		float data[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 5.0f, 
			0.0f, 0.0f, 1.0f, -3.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data);
		gFluid->createEmitter(emitterDesc);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	} catch (...) {
		std::cerr << "Strange error during fluid creation" << std::endl;
	}
}

void createCube(const NxVec3& pos, float size = 1.0f, const NxVec3& initialVelocity = NxVec3(0.0f, 0.0f, 0.0f))
{
	if (g_NxScene == NULL)
		return;
	
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping = 0.5f;
	bodyDesc.linearVelocity = initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3(size, size, size);
	boxDesc.skinWidth = 0.05f;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = 10.0f;
	actorDesc.globalPose.t = pos;
	g_NxScene->createActor(actorDesc)->userData = (void*)1;
}

void createStaticBox(const NxVec3& pos, const NxVec3& dim)
{
	NxBoxShapeDesc boxShape;
	boxShape.dimensions = dim;
	boxShape.skinWidth = 0.05f;

	NxActorDesc boxActor;
	boxActor.shapes.pushBack(&boxShape);
	boxActor.body = NULL;
	boxActor.globalPose.t = pos;

	g_NxScene->createActor(boxActor)->userData = (void*)0;
}

void createCubeFromEye(float size, float velocity)
{
	NxVec3 position(g_scene2.cameraFrame().position());
	NxVec3 initialVelocity((g_scene2.cameraFrame().forward() * velocity));
	createCube(position, size, initialVelocity);
}

void createCubesFromEye(float size, float velocity, int count)
{
	vmml::vec3f position = g_scene2.cameraFrame().position();
	vmml::vec3f xAxis = g_scene2.cameraFrame().xAxis();
	for (int i = 0; i < count; i++) {
		position += xAxis * (size + 0.1);
		NxVec3 nxPosition(position);
		NxVec3 initialVelocity((g_scene2.cameraFrame().forward() * velocity));
		createCube(nxPosition, size, initialVelocity);
	}
}

void initKeyController()
{
	vector<pair<unsigned int, unsigned int>> keys;
	keys.push_back(make_pair('1', '2'));
	keys.push_back(make_pair('3', '4'));
	keys.push_back(make_pair('5', '6'));
	keys.push_back(make_pair('7', '8'));
	keys.push_back(make_pair('9', '0'));
	keys.push_back(make_pair('[', ']'));
	keys.push_back(make_pair(';', '\''));
	keys.push_back(make_pair('.', '/'));
	keys.push_back(make_pair('o', 'p'));
	keys.push_back(make_pair('k', 'l'));
	keys.push_back(make_pair('-', '+'));
	keys.push_back(make_pair('n', 'm'));
	keys.push_back(make_pair('v', 'b'));
	keys.push_back(make_pair('<', '>'));
	keys.push_back(make_pair('(', ')'));

	g_renderingParameterController = boost::make_shared<ParameterController>(keys, (ParametrizedPtr)&g_scene2);
}

GLUSboolean init(GLUSvoid)
{
	glEnable(GL_CULL_FACE);
	createFluid();

	boost::shared_ptr<ParticleRenderer> renderer;
	switch (g_renderingMode) {
	case BILATERAL_GAUSS:
		renderer = boost::shared_ptr<ParticleRenderer>(new BilateralGaussParticleRenderer(&g_scene2));
		break;
	case CURVATURE_FLOW:
		renderer = boost::shared_ptr<ParticleRenderer>(new CurvatureFlowParticleRenderer(&g_scene2));
		break;
	case ISOSURFACE_EXTRACTION:
		renderer = boost::shared_ptr<ParticleRenderer>(new IsosurfaceParticleRenderer(&g_scene2));
		break;
	}

	g_scene2.setParticleRenderer(renderer);
	g_scene2.setParticlesToShutDown(g_maxParticles);

	initKeyController();
	return g_scene2.setup();
}

GLUSvoid reshape(GLUSuint width, GLUSuint height)
{
	g_scene2.reshape(width, height);
	// http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/viewport.html
	glViewport(0, 0, width, height);
}

GLUSboolean update(GLUSfloat time)
{
	g_NxScene->simulate(1.0f / 60.0f);
	
	g_scene2.render(g_NxScene);
	glFlush();

	g_NxScene->flushStream();
	g_NxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	
	return GLUS_TRUE;
}

GLUSvoid terminate(GLUSvoid)
{
}


void keyFunc(GLUSboolean pressed, GLUSuint key)
{
	const GLUSuint KEY_F = 102;
	const GLUSuint KEY_1 = 49;
	const GLUSuint KEY_2 = 50;
	const GLUSuint KEY_3 = 51;
	const GLUSuint KEY_4 = 52;
	const GLUSuint KEY_5 = 53;
	const GLUSuint KEY_6 = 54;
	const GLUSuint KEY_7 = 55;
	const GLUSuint KEY_8 = 56;

	if (key == KEY_F) {
		createCubeFromEye(0.5f, 50.0f);
	} else if (key == 100) {
		createCubesFromEye(1.0f, 50.0f, 10);
	} else if (key == '`') {
		g_renderingParameterController->printMappings();
	} else {
		g_renderingParameterController->keyPressed(key);
	}
}

bool initNx()
{
	NxSDKCreateError error;
	NxPhysicsSDKDesc desc;
	desc.gpuHeapSize = 128;
	desc.flags &= ~NX_SDKF_NO_HARDWARE;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, new ErrorStream(), desc, &error);
	if (gPhysicsSDK == NULL) {
		std::cerr << "Physics SDK creation failed: " << PhysXUtils::getErrorString(error) << endl;
		return false;
	}

	NxHWVersion hwCheck = gPhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) {
		std::cerr << "WARN: No PhysX hardware found. Fluid simulated on CPU" << std::endl;
		g_gpuAccelerationPossible = false;
	} else {
		std::cerr << "INFO: PhysX hardware acceleration possible. Fluid simulated on GPU" << std::endl;
		g_gpuAccelerationPossible = true;
	}
	
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.2f); // 0.2 is the best I've tried

	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(0.0f, -9.81f, 0.0f);
	g_NxScene = gPhysicsSDK->createScene(sceneDesc);

	if (g_NxScene == NULL) {
		std::cerr << "ERROR: unable to create PhysX scene" << endl;
		return false;
	}

	NxMaterial* defaultMaterial = g_NxScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	NxPlaneShapeDesc planeDesc;
	planeDesc.skinWidth = 0.05f;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	g_NxScene->createActor(actorDesc);

	// create basin for water
	createStaticBox(NxVec3(2.0f, 1.0f, 0.0f), NxVec3(0.2f, 1.0f, 2.0f));
	createStaticBox(NxVec3(-2.0f, 1.0f, 0.0f), NxVec3(0.2f, 1.0f, 2.0f));
	createStaticBox(NxVec3(0.0f, 1.0f, 2.0f), NxVec3(2.0f, 1.0f, 0.2f));
	createStaticBox(NxVec3(0.0f, 1.0f, -2.0f), NxVec3(2.0f, 1.0f, 0.2f));

	// create drains
	NxPlaneShapeDesc drainDesc;
	drainDesc.shapeFlags |= NX_SF_FLUID_DRAIN;
	drainDesc.normal = NxVec3(1.0, 0.0, 0.0);
	drainDesc.d = -4;

	NxActorDesc drainActor;
	drainActor.shapes.push_back(&drainDesc);
	g_NxScene->createActor(drainActor);

	drainDesc.normal = NxVec3(-1.0, 0.0, 0.0);
	g_NxScene->createActor(drainActor);
	
	drainDesc.normal = NxVec3(0.0, 0.0, 1.0);
	g_NxScene->createActor(drainActor);

	drainDesc.normal = NxVec3(0.0, 0.0, -1.0);
	g_NxScene->createActor(drainActor);

	drainDesc.d = -9;
	drainDesc.normal = NxVec3(0.0, -1.0, 0.0);
	g_NxScene->createActor(drainActor);

	return true;
}

void releaseNx()
{
	if(gPhysicsSDK != NULL)
	{
		if(g_NxScene != NULL) gPhysicsSDK->releaseScene(*g_NxScene);
		g_NxScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}
}

unsigned int g_XPos;
unsigned int g_YPos;

void mouseFunc(GLUSboolean pressed, unsigned int buttons, unsigned int xPos, unsigned int yPos)
{
	if (pressed) {
		g_XPos = xPos;
		g_YPos = yPos;
	}
}

void mouseMoveFunc(unsigned int buttons, unsigned int xPos, unsigned int yPos)
{
	float const TRANS_FACTOR = 0.01f;
	float const ROT_FACTOR = 0.005f;
	if (buttons) {
		int deltaX = (int)xPos - (int)g_XPos;
		int deltaY = (int)yPos - (int)g_YPos;

		if (buttons == 1) {			// left button
			g_scene2.rotateX(deltaY * ROT_FACTOR);
			g_scene2.rotateY(-deltaX * ROT_FACTOR);
		} else if (buttons == 2) {	// middle button
		} else if (buttons == 4) {  // right button
			g_scene2.translate(deltaX * TRANS_FACTOR, deltaY * TRANS_FACTOR, 0.0f);
			//g_scene.rotateLightDir(deltaY * ROT_FACTOR, deltaX * ROT_FACTOR);
		}
		g_XPos = xPos;
		g_YPos = yPos;
	}
}

void mouseWheelFunc(unsigned int buttons, int ticks, unsigned int xPos, unsigned int yPos)
{
	const float TICK_FACTOR = 0.5f;
	g_scene2.translate(0.0f, 0.0f, ticks * TICK_FACTOR);
}

void printUsage()
{
	std::cerr << "main.exe [bilateral_gauss | curvature_flow | isosurface] <screen_width> <screen_height> <max_particles>" << std::endl;
}

void parseCommandLine(int argc, char** argv)
{
	if (argc < 5) {
		printUsage();
		exit(1);
	}

	char* renderingMode = argv[1];
	if (strcmp(renderingMode, "bilateral_gauss") == 0) {
		g_renderingMode = RenderingMode::BILATERAL_GAUSS;
	} else if (strcmp(renderingMode, "curvature_flow") == 0) {
		g_renderingMode = RenderingMode::CURVATURE_FLOW;
	} else if (strcmp(renderingMode, "isosurface") == 0) {
		g_renderingMode = RenderingMode::ISOSURFACE_EXTRACTION;
	} else {
		std::cerr << "WARNING: rendering mode \"" << renderingMode << "\" not recognized" << std::endl;
		std::cerr << "Using \"bilateral_gauss\"" << std::endl;
		g_renderingMode = RenderingMode::BILATERAL_GAUSS;
	}

	g_screenWidth = atoi(argv[2]);
	g_screenHeight = atoi(argv[3]);

	g_maxParticles = atoi(argv[4]);
}

int main(int argc, char** argv)
{
	parseCommandLine(argc, argv);

	try {
		atexit(releaseNx);

		initNx();

		glusInitFunc(init);

		glusReshapeFunc(reshape);

		glusUpdateFunc(update);

		glusTerminateFunc(terminate);

		glusKeyFunc(keyFunc);

		glusMouseMoveFunc(mouseMoveFunc);

		glusMouseFunc(mouseFunc);

		glusMouseWheelFunc(mouseWheelFunc);

		glusPrepareContext(3, 2, GLUS_FORWARD_COMPATIBLE_BIT);

		if (!glusCreateWindow("GLUS Example Window", g_screenWidth, g_screenHeight, GLUS_FALSE))
		{
			std::cerr << "Could not create window!" << endl;
			return -1;
		}

		// Init GLEW
		glewExperimental = GL_TRUE;
		glewInit();

		// continue only if OpenGL 3.3 is supported.
		if (!glewIsSupported("GL_VERSION_3_2"))
		{
			std::cerr << "OpenGL 3.2 not supported." << endl;

			glusDestroyWindow();
			return -1;
		}

		glusRun();
	} catch (const std::exception& ex) {
		std::cerr << "EXCEPTION: " << ex.what() << std::endl;
		fgetc(stdin);
		return 1;
	} catch (const std::string& ex) {
		std::cerr << "EXCEPTION: " << ex << std::endl;
		fgetc(stdin);
	} catch (const char* ex) {
		std::cerr << "EXCEPTION: " << ex << std::endl;
		fgetc(stdin);
	} catch (...) {
		std::cerr << "UNDEFINED EXCEPTION" << std::endl;
		fgetc(stdin);
	}
}