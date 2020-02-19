#ifndef PSEUDO_ENTROPY_H_
#define PSEUDO_ENTROPY_H_

#include <cstdint>

float get_pseudo_entropy(float& mean_out, uint8_t* data, int len, int n_loop = 64);

#endif // PSEUDO_ENTROPY_H_
