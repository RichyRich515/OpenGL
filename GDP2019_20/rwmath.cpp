#include "rwmath.hpp"
#include <utility>

float rw::rand_in_range(float min, float max)
{
	return rand() / (float)RAND_MAX * (max - min + 1.0f) + min;
}