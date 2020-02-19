#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>

#include <cstdio> // For debugging

#include "pseudo-entropy.h"

float get_pseudo_entropy(float& mean_out, uint8_t* data, int len, int n_loop)
{
	float pe = 0.0f;

	float mean = 0.0f;
	float mean_squared = 0.0f;

	const float inv_n_loop = 1.0f / n_loop;

	for (int it = 0; it < n_loop; ++it) {
		int p = (random() / (double)RAND_MAX) * len;
		assert(p >= 0);

		mean += data[p];
		mean_squared += data[p] * data[p];
	}

	mean *= inv_n_loop;
	mean_squared *= inv_n_loop;

	pe = logf(mean_squared - mean * mean);

	mean_out = mean;

	return pe;
}
