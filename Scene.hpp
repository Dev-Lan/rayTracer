#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <tuple>
#include "Vec3.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "PixelBuffer.hpp"
#include "TriFace.hpp"


/// This class contains the relevant information for the scene.
/// ie camera position, objects in scene, background color, etc.
class Scene
{
public:
	Scene() {	}
	Scene(const std::string& filename);
	Vec3 camEye;
	Vec3 camLook;
	Vec3 camUp;
	float camFovh;
	int pixWidth;
	int pixHeight;
	Vec3 bkgcolor;
	std::vector< Material > materials;
	std::vector< Sphere > spheres;
	std::vector< Vec3 > verts;
	std::vector< std::pair<float, float> > texCoords;
	std::vector< Vec3 > norms;
	std::vector< TriFace > faces;
	std::vector< Light > lights;
	std::vector< PixelBuffer* > textures;
	bool isParallel;
	float viewDist;
protected:
	/// Helper function that assigns one of the values specified in the init file.
	void consumeLine(const std::vector< std::string >& line);
	/// Helper function for dealing with the slashes in the face input.
	void loadFaceChunk(const std::string& chunk, int& i, int& t, int& n);
	/// Another helper function to do a little sanity checking.
	void checkLength(const std::string& id, const std::vector< std::string >& line, unsigned int N);
	/// helper function for checking bounds of values
	void checkMin(const std::string& id, float value, float min);
	/// helper function for checking bounds of values
	void checkMax(const std::string& id, float value, float max);
	/// helper function for checking invalid Vec3s
	void checkVec(const std::string& id, const Vec3& v);

};

#endif
// SCENE_HPP