#ifndef VEC3_HPP
#define VEC3_HPP

#include <math.h>


/// Class for 3-dimensional vectors. Does basic vector operations
class Vec3 {
public:
	float x;
	float y;
	float z;

	Vec3() :
		x(0),
		y(0),
		z(0)
	{

	}
	
	Vec3(float x, float y, float z) :
		x(x),
		y(y),
		z(z)
	{

	}

	inline Vec3 operator+(const Vec3& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	inline Vec3 operator-(const Vec3& other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	inline Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	// inline Vec3 operator*(float k, const Vec3& vec) const
	// {
	// 	return Vec3(k * vec.x, k * vec.y, k * vec.z);
	// }

	// inline Vec3 operator*(const Vec3& vec, float k) const
	// {
	// 	return Vec3(k * vec.x, k * vec.y, k * vec.z);
	// }

	inline float dot(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}	

	inline Vec3 cross(const Vec3& other) const
	{
		return Vec3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y  * other.x
			);
	}

	/// lengthSquared is useful because it is faster than length, and lengthSquared comparisons are the same as length comparisons.
	/// Furthermore generally (lengthSquared < x * x) is faster than (length < x)
	inline float lengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	inline float length() const
	{
		return sqrt(x*x + y*y + z*z);
	}

	inline Vec3 unit() const
	{
		float l = length();
		return Vec3(x / l, y / l, z / l);
	}
};

Vec3 operator*(float k, const Vec3& vec);


Vec3 operator*(const Vec3& vec, float k);

#endif
// VEC3_HPP
