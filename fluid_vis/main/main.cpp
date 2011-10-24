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
#include <fluid_vis/Scene.h>
#include <fluid_vis/ErrorStream.h>
#include <fluid_vis/physx_utils.h>
#include <fluid_vis/Properties.h>
#include <fluid_vis/ConfigurationFactory.h>
#include <fluid_vis/MyFluid.h>
#include <fluid_vis/debug_utils.h>

using namespace std;

Scene g_scene;
static NxPhysicsSDK* gPhysicsSDK = NULL;
static NxScene* g_NxScene = NULL;
static MyFluid* gFluid = NULL;
static ConfigurationFactory configurationFactory("config");

bool g_useSurfaceExtraction = false;

bool g_gpuAccelerationPossible = false;

bool g_useGPUAcceleration = true;

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
			1.0f, 0.0f, 0.0f, 0.3f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data);
		
		float data2[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data2);
		gFluid->createEmitter(emitterDesc);
		
		float data3[] = {
			1.0f, 0.0f, 0.0f, -0.3f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data3);
		gFluid->createEmitter(emitterDesc);
		
		float data4[] = {
			1.0f, 0.0f, 0.0f, 0.3f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, -0.3f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data4);
		gFluid->createEmitter(emitterDesc);
		
		float data5[] = {
			1.0f, 0.0f, 0.0f, 0.3f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, 0.3f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data5);
		gFluid->createEmitter(emitterDesc);
		
		float data6[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, 0.3f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data6);
		gFluid->createEmitter(emitterDesc);
		
		float data7[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.3f, 
			1.0f, 1.0f, 0.0f, -0.3f, 
			0.0f, 0.0f, 0.0f, 1.0f
		};
		emitterDesc.relPose.setRowMajor44(data7);
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

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = 10.0f;
	actorDesc.globalPose.t = pos;
	g_NxScene->createActor(actorDesc)->userData = (void*)size_t(size);
}

void createCubeFromEye(float size, float velocity)
{
	NxVec3 position(g_scene.cameraFrame().position());
	NxVec3 initialVelocity((g_scene.cameraFrame().forward() * velocity));
	createCube(position, size, initialVelocity);
}

void createCubesFromEye(float size, float velocity, int count)
{
	vmml::vec3f position = g_scene.cameraFrame().position();
	vmml::vec3f xAxis = g_scene.cameraFrame().xAxis();
	for (int i = 0; i < count; i++) {
		position += xAxis * (size + 0.1);
		NxVec3 nxPosition(position);
		NxVec3 initialVelocity((g_scene.cameraFrame().forward() * velocity));
		createCube(nxPosition, size, initialVelocity);
	}
}

GLUSboolean init(GLUSvoid)
{
	glEnable(GL_CULL_FACE);
	createFluid();
	return g_scene.setup();
}

GLUSvoid reshape(GLUSuint width, GLUSuint height)
{
	g_scene.reshape(width, height);
	// http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/viewport.html
	glViewport(0, 0, width, height);
}

GLUSboolean update(GLUSfloat time)
{
	g_NxScene->simulate(1.0f / 60.0f);
	
	if (g_useSurfaceExtraction)
		g_scene.renderIsoSurface(g_NxScene);
	else 
		g_scene.render(g_NxScene);
	
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
		createCubeFromEye(1.0f, 50.0f);
		//createCubeFromEye(0.5f, 50.0f);
	} else if (key == 100) {
		createCubesFromEye(1.0f, 50.0f, 10);
	} else if (key == KEY_1) {
		g_scene.changeParticleSize(-1.0f);
	} else if (key == KEY_2) {
		g_scene.changeParticleSize(1.0f);
	} else if (key == KEY_3) {
		g_scene.changeEdgeTreshold(-1.0f);
		g_scene.changeBilateralTreshold(-0.0005);
	} else if (key == KEY_4) {
		g_scene.changeEdgeTreshold(1.0f);
		g_scene.changeBilateralTreshold(0.0005);
	} else if (key == KEY_5) {
		g_scene.changeGauss(0, -0.5);
	} else if (key == KEY_6) {
		g_scene.changeGauss(0,  0.5);
	} else if (key == KEY_7) {
		g_scene.changeGauss(-1, 0.0);
	} else if (key == KEY_8) {
		g_scene.changeGauss( 1, 0.0);
	} else if (key == '-') {
		g_scene.changeAdditionalBlurPhases(-1);
	} else if (key == '+') {
		g_scene.changeAdditionalBlurPhases(1);
	} else if (key == '[') {
		g_scene.changeDepthGauss(-1, 0);
	} else if (key == ']') {
		g_scene.changeDepthGauss(1, 0);
	} else if (key == 'o') {
		g_scene.changeDepthGauss(0, -0.5);
	} else if (key == 'p') {
		g_scene.changeDepthGauss(0, 0.5);
	} else if (key == 'k') {
		g_scene.changeParticleDepth(-0.001);
	} else if (key == 'l') {
		g_scene.changeParticleDepth(0.001);
	} else if (key == 'm') {
		g_scene.changeFilterSizeMult(-0.05);
	} else if (key == 'n') {
		g_scene.changeFilterSizeMult(0.05);
	} else if (key == '<') {
		g_scene.changeTimeStep(-1.0f);
	} else if (key == '>') {
		g_scene.changeTimeStep(1.0f);
	}
}

bool initNx()
{
	NxSDKCreateError error;
	NxPhysicsSDKDesc desc;
	desc.flags &= ~NX_SDKF_NO_HARDWARE;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, new ErrorStream(), desc, &error);
	if (gPhysicsSDK == NULL) {
		cout << "Physics SDK creation failed: " << PhysXUtils::getErrorString(error) << endl;
		return false;
	}

	NxHWVersion hwCheck = gPhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) {
		std::cout << "WARN: No PhysX hardware found. Fluid simulated on CPU" << std::endl;
		g_gpuAccelerationPossible = false;
	} else {
		std::cout << "INFO: PhysX hardware acceleration possible. Fluid simulated on GPU" << std::endl;
		g_gpuAccelerationPossible = true;
	}

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.2f); // 0.2 is the best I've tried

	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(0.0f, -9.81f, 0.0f);
	g_NxScene = gPhysicsSDK->createScene(sceneDesc);

	if (g_NxScene == NULL) {
		cout << "ERROR: unable to create PhysX scene" << endl;
		return false;
	}

	NxMaterial* defaultMaterial = g_NxScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	g_NxScene->createActor(actorDesc);

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
			g_scene.rotateX(deltaY * ROT_FACTOR);
			g_scene.rotateY(-deltaX * ROT_FACTOR);
		} else if (buttons == 2) {	// middle button
		} else if (buttons == 4) {  // right button
			g_scene.translate(deltaX * TRANS_FACTOR, deltaY * TRANS_FACTOR, 0.0f);
			//g_scene.rotateLightDir(deltaY * ROT_FACTOR, deltaX * ROT_FACTOR);
		}
		g_XPos = xPos;
		g_YPos = yPos;
	}
}

void mouseWheelFunc(unsigned int buttons, int ticks, unsigned int xPos, unsigned int yPos)
{
	const float TICK_FACTOR = 0.5f;
	g_scene.translate(0.0f, 0.0f, ticks * TICK_FACTOR);
}

int main(int argc, char** argv)
{
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

	if (!glusCreateWindow("GLUS Example Window", 640, 480, GLUS_FALSE))
	{
		cout << "Could not create window!" << endl;
		return -1;
	}

	// Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// continue only if OpenGL 3.3 is supported.
	if (!glewIsSupported("GL_VERSION_3_2"))
	{
		cout << "OpenGL 3.2 not supported." << endl;

		glusDestroyWindow();
		return -1;
	}

	glusRun();
}