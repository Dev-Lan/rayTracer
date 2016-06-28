#ifndef RAY3_HPP
#define RAY3_HPP

#include <limits>
#include <math.h>
#include "Sphere.hpp"
#include "Vec3.hpp"

/// 3D ray class. Math for ray object intersection is done here.
class Ray3
{
public:
	Vec3 origin;
	Vec3 dir;
	Ray3() {}
	Ray3(const Vec3& origin, const Vec3& dir) :
		origin(origin),
		dir(dir.unit())
		{

		}

	/// Ray sphere intersection. Math referenced from:
	/// http://www.ccs.neu.edu/home/fell/CSU540/programs/RayTracingFormulas.htm
	inline float timeToIntersection(const Sphere& sphere) const
	{
		float a,b,c;
		a = dir.dot(dir);
		b = 2 * dir.dot( origin - sphere.center );
		c = sphere.center.dot( sphere.center ) +
			origin.dot(origin) -
			2 * sphere.center.dot(origin) - 
			sphere.radius * sphere.radius;
		float discr = b * b - 4 * a * c;
		if (discr < 0) {
			// No coliision
			return std::numeric_limits<float>::infinity();
		} else if (discr == 0) {
			// Grazing hit
			return (-b - sqrt(discr)) / (2*a);
		} else {
			// double hit, have to find the closest positive intersection
			float t = (-b - sqrt(discr)) / (2*a);
			if (t > 0) {
				return t;
			} else {
				t = (-b + sqrt(discr)) / (2*a);
				if (t > 0) {
					return t;
				} else {
					// only hits in negative time, ie, no hit.
					return std::numeric_limits<float>::infinity();
				}
			}
		}
	}

	// /// Ray triangle intersection. Either returns time to intersection, or zero if there
	// /// is no intersection. It also loads the barycentric coordinates onto baryCoords.
	inline float timeToIntersection(const Vec3& p0, const Vec3& p1, const Vec3& p2, Vec3& baryCoords) const
	{
		// std::cout << p0.x <<","<< p1.x <<","<<p2.x << std::endl;
		// std::cout << p0.y <<","<< p1.y <<","<<p2.y << std::endl;
		// std::cout << p0.z <<","<< p1.z <<","<<p2.z << std::endl;
		baryCoords = fastRayTriangleIntersection(p0, p1, p2);
		// std::cout << "bary ("<<baryCoords.x<<","<<baryCoords.y<<","<<baryCoords.z<< ")"<<std::endl;
		if (baryCoords.z >= 0 && baryCoords.y >= 0 && baryCoords.y <= 1 && baryCoords.x >= 0 && baryCoords.x <= 1 - baryCoords.y) {
			// find t
			Vec3 e1 = p1 - p0;
			Vec3 e2 = p2 - p0;
			Vec3 N = e1.cross(e2);
			float d = -(N.dot(p0));
			return -(origin.dot(N) + d) / (dir.dot(N));
		} else {
			// No collision, return infinity.
			return std::numeric_limits<float>::infinity();
		}
	}

	/// Efficient Ray triangle intersection that returns barycentric coordinates using
	/// the alphabet soup method described in our textbook.
	inline Vec3 fastRayTriangleIntersection(const Vec3& p0, const Vec3& p1, const Vec3& p2) const
	{
		// I could rearrange these for more efficiency, but nah.
		float a = p0.x - p1.x;
		float b = p0.y - p1.y;
		float c = p0.z - p1.z;
		float d = p0.x - p2.x;
		float e = p0.y - p2.y;
		float f = p0.z - p2.z;
		float g = dir.x;
		float h = dir.y;
		float i = dir.z;
		float j = p0.x - origin.x;
		float k = p0.y - origin.y;
		float l = p0.z - origin.z;
		float m = e*i - h*f;
		float n = g*f - d*i;
		float o = d*h - e*g;
		float p = a*k - j*b;
		float q = j*c - a*l;
		float r = b*l - k*c;
		float s = a*m + b*n + c*o;
		// barycentric coordinates
		float B = (j*m + k*n + l*o) / s;
		float Y = (i*p + h*q + g*r) / s;
		float T = -(f*p + e*q + d*r) / s;
		return Vec3(B,Y,T);
	}



	/// Finds point from origin of ray after traveling for time t.
	inline Vec3 pointAtTime(float t) const
	{
		return origin + dir*t;
	}

	/// Finds ray from origin of ray after traveling for time t.
	inline Ray3 rayAtTime(float t) const
	{
		return Ray3(origin + dir*t, dir);
	}
};

#endif
// RAY3_HPP