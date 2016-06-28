#ifndef TRIFACE_HPP
#define TRIFACE_HPP
#include <tuple>
#include "Vec3.hpp"

/// Class for representing triangle Faces.
class TriFace
{
public:
	std::tuple< int, int, int > vertIndices;
	std::tuple< int, int, int > texCoordIndices;
	std::tuple< int, int, int > normIndices;
	Vec3 N;
	int matIndex; // index of TriFace material information
	int texIndex; // index of TriFace texture information
	TriFace() {}
	TriFace(
		const std::tuple<int, int, int>& vertIndices,
		const std::tuple<int, int, int>& texCoordIndices,
		const std::tuple<int, int, int>& normIndices,
		int materialIndex,
		int textureIndex,
		const std::vector< Vec3 >& verts
		) :
			vertIndices(vertIndices),
			texCoordIndices(texCoordIndices),
			normIndices(normIndices),
			matIndex(materialIndex),
			texIndex(textureIndex)
		{
			if (std::get<0>(normIndices) == -1) {
				// there are no explicit normals specified in the input.
				// calculate a flat normal for the whole face.
				int i0, i1, i2;
				std::tie(i0,i1,i2) = vertIndices;
				Vec3 e1 = verts[i1] - verts[i0];
				Vec3 e2 = verts[i2] - verts[i0];
				N = e1.cross(e2).unit();
			} else {
				N = Vec3(0,0,0);
			}
			if (std::get<0>(texCoordIndices) == -1) {
				texIndex = -1;
			}
		}
	inline Vec3 getNormal(const Vec3& baryCoords, const std::vector< Vec3 >& norms) const
	{
		if (std::get<0>(normIndices) == -1) {
			return N;
		} else {
			float alpha = 1 - (baryCoords.x + baryCoords.y);
			float beta = baryCoords.x;
			float gamma = baryCoords.y;
			const Vec3& n0 = norms[std::get<0>(normIndices)];
			const Vec3& n1 = norms[std::get<1>(normIndices)];
			const Vec3& n2 = norms[std::get<2>(normIndices)];
			return (alpha*n0 + beta*n1 + gamma*n2).unit();
		}
	}

	inline std::pair<float, float> getTexCoords(const Vec3& baryCoords, const std::vector< std::pair<float, float> >& texCoords) const
	{
		if (std::get<0>(texCoordIndices) == -1) {
			return std::make_pair(0, 0);
		} else {
			float alpha = 1 - (baryCoords.x + baryCoords.y);
			float beta = baryCoords.x;
			float gamma = baryCoords.y;
			const std::pair<float, float>& uv0 = texCoords[std::get<0>(texCoordIndices)];
			const std::pair<float, float>& uv1 = texCoords[std::get<1>(texCoordIndices)];
			const std::pair<float, float>& uv2 = texCoords[std::get<2>(texCoordIndices)];
			float u = alpha * uv0.first + beta * uv1.first + gamma * uv2.first;
			float v = alpha * uv0.second + beta * uv1.second + gamma * uv2.second;
			return std::make_pair(u,v);
		}
	}
};

#endif
// TRIFACE_HPP