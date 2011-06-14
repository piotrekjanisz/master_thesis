#pragma once

namespace Filters 
{
	/**
	 * Create gaussian filter matrix
	 */
	void createGauss2D(int size, double step, double sigma, float* data);
	void createGauss1D(int size, double step, double sigma, float* data);
	void normalize(float* data, int rows, int cols);

	void createHeavisideDistribution(double first, double last, double cutoff, int size, float* data);
};