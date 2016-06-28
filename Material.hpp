#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Vec3.hpp"

/// This class holds all the constant information of a given material.
/// At this point it could be accomplished with a struct instead, but if
/// we ever extend this it might be useful for this to be in a class.
class Material {
public:
	Material() {	}
	Material(
		const Vec3& diffuseColor,
		const Vec3& specularColor,
		float ka,
		float kd,
		float ks,
		float alpha,
		float opacity,
		float ior
		) :
			diffuseColor(diffuseColor),
			specularColor(specularColor),
			ka(ka),
			kd(kd),
			ks(ks),
			alpha(alpha),
			opacity(opacity),
			ior(ior)
	{
		float tmp = (ior - 1) / (ior + 1); // assuming air
		F0 = tmp * tmp;
		if (ior < 0) {
			F0 = -1;
		}
		// std::cout << "ior : " << ior << std::endl;
		// std::cout << "F0  : " << F0 << std::endl << std::endl;
	}
	
	Vec3 diffuseColor;
	Vec3 specularColor;
	float ka;
	float kd;
	float ks;
	float alpha; // exponent in specular component of phong equation
	float opacity; // how tranparent the material is
	float ior; // index of refraction of the material
	float F0; // minimal fresnel reflection

};

#endif
// MATERIAL_HPP