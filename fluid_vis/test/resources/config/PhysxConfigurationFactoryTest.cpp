#include <fluid_vis/PhysxConfigurationFactory.h>
#include <boost/test/unit_test.hpp>
#include <NxPhysics.h>

using namespace std;

struct PhysxConfigurationFactoryTestFixture
{
	PhysxConfigurationFactoryTestFixture()
		: mConfigRoot("test_config"), mFactory(mConfigRoot)
	{
	}

	~PhysxConfigurationFactoryTestFixture()
	{
	}

	string mConfigRoot;
	PhysxConfigurationFactory mFactory;

	static const float FLOAT_PRECISION;
};

const float PhysxConfigurationFactoryTestFixture::FLOAT_PRECISION = 0.0000001f;

BOOST_FIXTURE_TEST_SUITE(PhysxConfigurationFactoryTest, PhysxConfigurationFactoryTestFixture);

BOOST_AUTO_TEST_CASE(Init)
{
	BOOST_REQUIRE_NO_THROW((PhysxConfigurationFactory(mConfigRoot)));
	BOOST_REQUIRE_THROW(PhysxConfigurationFactory("foo/bar/bleh"), PhysxConfigurationException);
}

BOOST_AUTO_TEST_CASE(Creation)
{
	BOOST_CHECK_NO_THROW(mFactory.createFluidDesc("water1"));
	BOOST_CHECK_THROW(mFactory.createFluidDesc("water1.cfg"), PhysxConfigurationException);
	BOOST_CHECK_THROW(mFactory.createFluidDesc("test_cofnig/water1.cfg"), PhysxConfigurationException);
	
	BOOST_CHECK_NO_THROW(mFactory.createFluidEmitterDesc("emitter1"));
	BOOST_CHECK_THROW(mFactory.createFluidEmitterDesc("emitter1.cfg"), PhysxConfigurationException);
	BOOST_CHECK_THROW(mFactory.createFluidEmitterDesc("test_cofnig/emitter1.cfg"), PhysxConfigurationException);
}

BOOST_AUTO_TEST_CASE(CreateFluid)
{
	NxFluidDesc fluidDesc = mFactory.createFluidDesc("water1");
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.kernelRadiusMultiplier, 2.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.restParticlesPerMeter, 10.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.motionLimitMultiplier, 3.0f, FLOAT_PRECISION);
	BOOST_CHECK_EQUAL(fluidDesc.packetSizeMultiplier, 8);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.collisionDistanceMultiplier, 0.1, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.stiffness, 50.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.viscosity, 40.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.restDensity, 1000.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.damping, 0.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.restitutionForStaticShapes, 0.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(fluidDesc.dynamicFrictionForStaticShapes, 0.05f, FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(CreateFluidEmitter)
{
	NxFluidEmitterDesc emitterDesc = mFactory.createFluidEmitterDesc("emitter1");
	BOOST_CHECK_EQUAL(emitterDesc.maxParticles, 0);
	BOOST_CHECK_CLOSE_FRACTION(emitterDesc.dimensionX, 0.3f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(emitterDesc.dimensionY, 0.3f, FLOAT_PRECISION);
	BOOST_CHECK_EQUAL(emitterDesc.type, NX_FE_CONSTANT_FLOW_RATE);
	BOOST_CHECK_CLOSE_FRACTION(emitterDesc.rate, 300.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(emitterDesc.fluidVelocityMagnitude, 10.0f, FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(emitterDesc.particleLifetime, 5.0f, FLOAT_PRECISION);
	BOOST_CHECK_EQUAL(emitterDesc.shape, NX_FE_RECTANGULAR);
	BOOST_CHECK_CLOSE_FRACTION(emitterDesc.repulsionCoefficient, 0.02f, FLOAT_PRECISION);
}

BOOST_AUTO_TEST_SUITE_END();