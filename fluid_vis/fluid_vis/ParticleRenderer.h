#pragma once

#include "Texture.h"
#include "AbstractScene.h"
#include <utils/ParameterController.h>

#include <vector>
#include <string>
#include <set>
#include <list>
#include <boost/timer.hpp>

struct ParticleData
{
	int particleCount;
	float* particles;
	float* particleDensity;
};

#define MAX_SIZE 3

class ParticleRenderer : public Parametrized
{
private:
	boost::timer _timer;
	unsigned int _renderingTime;
	double _renderingTimeTotal;
	unsigned int _query;
	unsigned int _queries[2];
	std::list<double> _times;

protected:
	AbstractScene* _scene;

	void startTimeQuery()
	{
#ifdef QUERY_TIME
		glGenQueries(1, &_query);
		glBeginQuery(GL_TIME_ELAPSED, _query);
#endif
	}

	void endTimeQuery(double additionalTime = 0.0)
	{
#ifdef QUERY_TIME
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectuiv(_query, GL_QUERY_RESULT, &_renderingTime);
		glDeleteQueries(1, &_query);
		_renderingTimeTotal = additionalTime + ((double)_renderingTime / 1000000000.0);
		_times.push_front(_renderingTimeTotal);
		if (_times.size() > MAX_SIZE)
			_times.pop_back();
#endif
	}

public:
	ParticleRenderer(AbstractScene* scene) : _scene(scene){};
	virtual ~ParticleRenderer(void) {};

	virtual bool setup() = 0;
	virtual void resize(int width, int height) = 0;
	virtual void render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData) = 0;

	double getRenderingTime()
	{
		double renderingTime = 0.0;
		for(std::list<double>::iterator it = _times.begin(); it != _times.end(); ++it)
			renderingTime += *it;
		return renderingTime / (double)(_times.size() > 0 ? _times.size() : 1);
	}
};

