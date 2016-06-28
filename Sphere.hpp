#ifndef SPHERE_HPP
#define SPHERE_HPP

/// Class for representing spheres.
/// Points to material index.
class Sphere
{
public:
	Vec3 center;
	float radius;
	int matIndex; // index of sphere material information
	int texIndex; // index of sphere texture information
	Sphere() {}
	Sphere(const Vec3& center, float radius, int materialIndex, int textureIndex):
		center(center),
		radius(radius),
		matIndex(materialIndex),
		texIndex(textureIndex)
		{

		}
};

#endif
// SPHERE_HPP