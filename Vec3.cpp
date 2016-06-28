#include "Vec3.hpp"

Vec3 operator*(float k, const Vec3& vec)
{
	return Vec3(k * vec.x, k * vec.y, k * vec.z);
}

Vec3 operator*(const Vec3& vec, float k)
{
	return Vec3(k * vec.x, k * vec.y, k * vec.z);
}