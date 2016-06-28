#include "RayTracer.hpp"

PixelBuffer RayTracer::rayTrace(const Scene& scene)
{
	Vec3 centerOfViewport;
	if (scene.isParallel) {
		centerOfViewport = scene.camEye;
	} else {
		centerOfViewport = scene.camEye + scene.camLook.unit();
	}
	// drop the adjacent length since it is always one.
	float halfWidth = tan(scene.camFovh * M_PI / 360.f); // it should be 360 degrees since fovh should be divided by two
	float halfHeight = halfWidth * (float) scene.pixHeight / (float) scene.pixWidth;
	Vec3 leftOffset = scene.camUp.cross(scene.camLook).unit();
	Vec3 upOffset = scene.camLook.cross(leftOffset).unit();

	// get the top left corner of the viewport
	Vec3 topLeft = centerOfViewport + halfWidth * leftOffset + halfHeight * upOffset;
	Vec3 pixelRight = (1.f / (float) scene.pixWidth)  * 2 * halfWidth * -leftOffset;
	Vec3 pixelDown  = (1.f / (float) scene.pixHeight) * 2 * halfHeight * -upOffset;

	// move top left corner to center of top left pixel
	topLeft = topLeft + 0.5f * pixelRight + 0.5f * pixelDown;

	PixelBuffer buf(scene.pixWidth, scene.pixHeight);
	Vec3 pixelPos;
	Vec3 pixelColor;
	for (int i=0; i < buf.width(); i++) {
		std::cout << "\r(" << i << "/" << buf.width() << ")" << std::flush;
		for (int j=0; j < buf.height(); j++) {
			pixelPos = topLeft + i * pixelRight + j * pixelDown;
			Ray3 toPixel;
			if (scene.isParallel) {
				toPixel = Ray3(pixelPos, scene.camLook);
			} else {
				toPixel = Ray3(scene.camEye, pixelPos - scene.camEye);
			}
			pixelColor = shadeRay(toPixel, scene);
			buf.setPixel(i, j, pixelColor.x, pixelColor.y, pixelColor.z);
		}
	}
	std::cout << std::endl;
	return buf;
}

Vec3 RayTracer::shadeRay(const Ray3& ray, const Scene& scene, int depth) {
	int objectIndex = -1;
	float smallestPositiveTime = -1;
	bool hitSphere = true;
	for (unsigned int i=0; i < scene.spheres.size(); i++) {
		float t = ray.timeToIntersection(scene.spheres[i]);
		if (smallestPositiveTime < 0 || (t < smallestPositiveTime && t > 0 && !isinf(t))) {
			smallestPositiveTime = t;
			objectIndex = i;
		}
	}

	Vec3 baryCoords;
	for (unsigned int i=0; i < scene.faces.size(); i++) {
		int index0, index1, index2;
		std::tie(index0, index1, index2) = scene.faces[i].vertIndices;

		const Vec3& p0 = scene.verts[index0];
		const Vec3& p1 = scene.verts[index1];
		const Vec3& p2 = scene.verts[index2];
		Vec3 thisBaryCoords;
		float t = ray.timeToIntersection(p0, p1, p2, thisBaryCoords);
		if (smallestPositiveTime < 0 || (t < smallestPositiveTime && t > 0 && !isinf(t))) {
			smallestPositiveTime = t;
			baryCoords = thisBaryCoords;
			objectIndex = i;
			hitSphere = false;
		}

	}

	// return color of closest hit sphere
	if (smallestPositiveTime >= 0 && !isinf(smallestPositiveTime)) {

		Vec3 pointOfCollision = ray.pointAtTime(smallestPositiveTime);
		Vec3 N;
		if (hitSphere) {
			N = (pointOfCollision - scene.spheres[objectIndex].center).unit();
		} else {
			N = scene.faces[objectIndex].getNormal(baryCoords, scene.norms);
		}
		// AMBIENT
		Material m;
		int texIndex;
		if (hitSphere) {
			m = scene.materials[ scene.spheres[objectIndex].matIndex ];
			texIndex = scene.spheres[objectIndex].texIndex;
		} else {
			m = scene.materials[ scene.faces[objectIndex].matIndex ];
			texIndex = scene.faces[objectIndex].texIndex;
		}
		bool comingFromBackside = false;
		bool totalInternalRefraction = false;
		float cosTheta = N.dot(-ray.dir);
		if (cosTheta < 0) {
			comingFromBackside = true;
			cosTheta *= -1;
			float thetaCritical = asin(m.ior);
			float theta = acos(cosTheta);
			if (theta > thetaCritical) {
				totalInternalRefraction = true;
			}
		}
		float Fr;
		if (m.F0 == -1) {
			Fr = 0;
		} else {
			// schlick's approximation
			Fr = m.F0 + (1 - m.F0) * pow( (1 - cosTheta), 5 );
		}
		assert(Fr < 1.01 && Fr > -0.01);
		// std::cout << "nDotview : " << N.dot(ray.dir) << std::endl;

		float u,v;
		if (texIndex >= 0) {
			if (hitSphere) {
				float theta = atan2(N.x, N.z);
				float phi = acos(N.y);
				u = (theta + M_PI) / (2 * M_PI);
				v = phi / M_PI;
			} else {
				std::pair< float, float > uv = scene.faces[objectIndex].getTexCoords(baryCoords, scene.texCoords);
				u = uv.first;
				v = uv.second;
			}
		}

		Vec3 diffuseColor;
		if (texIndex < 0) {
			diffuseColor = m.diffuseColor;
		} else {
			diffuseColor = scene.textures[texIndex]->getColor(u, v);
		}

		Vec3 resultingColor = m.ka * diffuseColor;


		for (unsigned int i=0; i < scene.lights.size(); i++) {
			Vec3 thisLightContribution(0,0,0);
			// CHECK FOR SHADOW
			Vec3 toLight = scene.lights[i].directionToPoint(pointOfCollision); // directionToPoint returns a norm
			float distanceToLight = scene.lights[i].distanceToPoint(pointOfCollision);

			Ray3 shadowRay = Ray3(pointOfCollision, toLight);
			const float eps = 0.001f;
			shadowRay = shadowRay.rayAtTime(eps);
			float shadowFlag = 1.f;
			// smallestPositiveTime = -1;
			for (unsigned int j=0; j < scene.spheres.size(); j++) {
				float t = shadowRay.timeToIntersection(scene.spheres[j]);
				// if (smallestPositiveTime < 0 || (t < smallestPositiveTime && t > 0 && !isinf(t))) {
				// 	smallestPositiveTime = t;
				// }
				if (t > 0 && !isinf(t) && t < distanceToLight) {
					float alpha = scene.materials[ scene.spheres[j].matIndex ].opacity;
					shadowFlag *= (1-alpha);
				}
			}

			for (unsigned int j=0; j < scene.faces.size(); j++) {
				int index0, index1, index2;
				std::tie(index0, index1, index2) = scene.faces[j].vertIndices;

				const Vec3& p0 = scene.verts[index0];
				const Vec3& p1 = scene.verts[index1];
				const Vec3& p2 = scene.verts[index2];
				Vec3 dummy;
				float t = shadowRay.timeToIntersection(p0, p1, p2, dummy);
				// if (smallestPositiveTime < 0 || (t < smallestPositiveTime && t > 0 && !isinf(t))) {
				// 	smallestPositiveTime = t;
				// 	objectIndex = j;
				// }
				if (t > 0 && !isinf(t) && t < distanceToLight) {
					float alpha = scene.materials[ scene.faces[j].matIndex ].opacity;
					shadowFlag *= (1-alpha);
				}
			}

			// if (smallestPositiveTime >= 0 && smallestPositiveTime < distanceToLight && !isinf(smallestPositiveTime)) {
			// 	shadowFlag = 0.f;
			// }
			// DIFFUSE
			float NdotL = N.dot(toLight);
			if (NdotL > 0) {
				Vec3 thisDiffuse = shadowFlag * m.kd * (N.dot(toLight)) * diffuseColor;
				thisLightContribution = thisLightContribution + thisDiffuse;
			
				// SPECULAR
				Vec3 R = 2 * (toLight.dot(N)) * N - toLight; // Use the actual reflected ray.
				float RdotV = R.dot(-ray.dir.unit());
				if (RdotV > 0) {
					Vec3 thisSpecular = shadowFlag * m.ks * pow(RdotV, m.alpha) * m.specularColor;
					thisLightContribution = thisLightContribution + thisSpecular;
				}
			}
			resultingColor = resultingColor + Vec3(thisLightContribution.x * scene.lights[i].color.x,
												   thisLightContribution.y * scene.lights[i].color.y,
												   thisLightContribution.z * scene.lights[i].color.z);
		}
		const int maxDepth = 10;
		if (depth < maxDepth) {
			const float eps = 0.001f;
			if (Fr > 0) { // avoids uneeded computation 
				Vec3 reflectedRayDir = -(2 * ( (ray.dir.dot(N)) * N ) - ray.dir); // the whole thing is negative, because ray.dir is in the wrong direction
				Ray3 reflectedRay(pointOfCollision, reflectedRayDir);
				Vec3 reflectedColor = shadeRay(reflectedRay.rayAtTime(eps), scene, depth+1);
				resultingColor = resultingColor + Fr * reflectedColor;
				// std::cout << ".";
				// resultingColor = resultingColor + 0.5f * reflectedColor;
			}
			if (!totalInternalRefraction && m.opacity < 1) {
				float iorTerm;
				if (comingFromBackside) {
					// we are exitting the material (assuming we are going into air)
					iorTerm =  m.ior / 1.f;
				} else {
					// we are entering the material (assuming we are coming from air)
					iorTerm = 1.f / m.ior;

				}
				Vec3 refractedRayDir = (-N) * sqrt(1 - (iorTerm * iorTerm * (1 - cosTheta * cosTheta) )) + iorTerm * (cosTheta * N + ray.dir); // again ray.dir is backwards, so we add instead of subtract
				Ray3 refractedRay(pointOfCollision, refractedRayDir);
				Vec3 refractedColor = shadeRay(refractedRay.rayAtTime(eps), scene, depth+1);
				resultingColor = resultingColor + (1 - Fr) * (1 - m.opacity) * refractedColor;
			}
		}
		return resultingColor;
	}

	// no hit, return background color
	return scene.bkgcolor;
}

