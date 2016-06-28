#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <limits>
#include "Vec3.hpp"

class Light {
public:

	enum LightType
	{
		DIRECTIONAL,
		POINT
	};

	Light() {	}
	Light(
		const Vec3& v,
		enum LightType lightType,
		const Vec3& color
		) :
			color(color),
			_v(v),
			_lightType(lightType)
	{

	}
	Vec3 color;
	/// returns direction to point regardless of the type of light.
	inline Vec3 directionToPoint(const Vec3& point) const {
		if (_lightType == LightType::POINT) {
			return (_v - point).unit();
		} else if (_lightType == LightType::DIRECTIONAL) {
			return -_v;
		} else {
			return -_v;
		}
	}

	/// returns distance between point and light, directional light is assumed to be at infinity.
	inline float distanceToPoint(const Vec3& point) const {
		if (_lightType == LightType::POINT) {
			return (_v - point).length();
		} else if (_lightType == LightType::DIRECTIONAL) {
			return std::numeric_limits<float>::infinity();
		} else {
			return std::numeric_limits<float>::infinity();
		}
	}
protected:
	/// Either the position, or the direction of the light.
	Vec3 _v;

	/// specifies if light is point or directional
	LightType _lightType;
};

#endif
// LIGHT_HPP