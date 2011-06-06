#include "filters.h"
#include "debug_utils.h"
#include <cmath>

const double PI = 3.1415926535897932;

void Filters::createGauss2D(int size, double step, double sigma, float* data)
{
	int center = size / 2;
	double doubleSigmaSquare = 2 * sigma;
	double gaussCoef = PI*doubleSigmaSquare;
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			double x = (center - j) * step;
			double y = (center - i) * step;
			data[i*size + j] = exp(-(x*x + y*y)/doubleSigmaSquare)/gaussCoef;
		}
	}
}

void Filters::createGauss1D(int size, double step, double sigma, float* data)
{
	int center = size / 2;
	double doubleSigmaSquare = 2 * sigma;
	double gaussCoef = sqrt(PI*doubleSigmaSquare);

	for (int i = 0; i < size; i++) {
		double x = (center - i) * step;
		data[i] = exp(-x*x / doubleSigmaSquare);
	}
}

void Filters::createHeavisideDistribution(double first, double last, double cutoff, int size, float* data)
{
	double step = (last - first) / (size -1);
	double val = first;

	for (int i = 0; i < size; i++) {
		data[i] = val < cutoff ? 1.0f : 0.0f;
		val += step;
	}
}