#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <math.h>
#include <assert.h>
#include "PixelBuffer.hpp"
#include "Scene.hpp"
#include "Ray3.hpp"
#include "Vec3.hpp"
#include "Sphere.hpp"


/// main driver class of ray tracing program
class RayTracer {
public:
	RayTracer() {}

	/// This is the main function of the ray tracer.
	static PixelBuffer rayTrace(const Scene& scene);

private:

	/// Casts a single ray out and returns color as a rgb Vec3
	static Vec3 shadeRay(const Ray3& ray, const Scene& scene, int depth=0);

};

#endif
// RAYTRACER_HPP