#pragma once

namespace Filters 
{
	/**
	 * Create gaussian filter matrix
	 */
	void createGauss2D(int size, double step, double sigma, float* data);
	void createGauss1D(int size, double step, double sigma, float* data);

	void createHeavisideDistribution(double first, double last, double cutoff, int size, float* data);
};