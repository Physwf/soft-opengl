#include "vector.h"

#include <cmath>

Vector multiply(const Vector& v1, const Vector& v2)
{
	return { v1.y*v2.z - v2.y*v1.z, v1.z*v2.x - v2.z*v1.x, v1.x*v2.y - v2.x*v1.y };
}

Vector sub(const Vector& v1, const Vector& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector normalize(const Vector& v)
{
	float sqrt = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return { v.x / sqrt, v.y / sqrt, v.z / sqrt };
}

