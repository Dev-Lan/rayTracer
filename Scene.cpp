#include "Scene.hpp"

Scene::Scene(const std::string& filename) {
	std::ifstream inFile;
	inFile.open(filename.c_str());
	if (!inFile) {
		std::cerr << "ERROR : Failed to find file (" << filename << ")." << std::endl;
		exit(1);
	}
	isParallel = false;
	std::cout << "Initializing scene.";
	int i = 0;
	std::string line;
	while (std::getline(inFile, line)) {
		std::vector< std::string > tokens;
		std::string buf;
		std::stringstream ss(line);
		while (ss >> buf) {
			tokens.push_back(buf);
		}
		if (!tokens.empty()) {
			consumeLine(tokens);
		}
		i++;
		if (i % 1000 == 0) {
			std::cout <<".";
		}
	}
	std::cout << std::endl;
}

void Scene::consumeLine(const std::vector< std::string >& line) {
	if (line[0].compare(std::string("eye")) == 0) {
		checkLength(std::string("eye"), line, 3);
		try {
			camEye = Vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for ey. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("viewdir")) == 0) {
		checkLength(std::string("viewdir"), line, 3);
		try {
			camLook = Vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for viewdir. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("updir")) == 0) {
		checkLength(std::string("updir"), line, 3);
		try {
			camUp = Vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for updir. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("fovh")) == 0) {
		checkLength(std::string("fovh"), line, 1);
		try {
			camFovh = std::stof(line[1]);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for fovh. Exiting early." << std::endl;
			exit(1);
		}
		checkMin(std::string("fovh"), camFovh, 0);
		checkMax(std::string("fovh"), camFovh, 180);
	} else if (line[0].compare(std::string("imsize")) == 0) {
		checkLength(std::string("imsize"), line, 2);
		try {
			pixWidth = std::stoi(line[1]);
			pixHeight = std::stoi(line[2]);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for imsize. Exiting early." << std::endl;
			exit(1);
		}
		checkMin(std::string("imsize"), pixWidth, 0);
		checkMin(std::string("imsize"), pixHeight, 0);
	} else if (line[0].compare(std::string("bkgcolor")) == 0) {
		checkLength(std::string("bkgcolor"), line, 3);
		try {
			bkgcolor = Vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for bkgcolor. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("mtlcolor")) == 0) {
		checkLength(std::string("mtlcolor"), line, 12);
		try {
			Vec3 diffuseColor(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
			Vec3 specularColor(std::stof(line[4]), std::stof(line[5]), std::stof(line[6]));
			float ka = std::stof(line[7]);
			float kd = std::stof(line[8]);
			float ks =std::stof(line[9]);
			float alpha = std::stof(line[10]);
			float opacity = std::stof(line[11]);
			float ior = std::stof(line[12]);
			Material newMaterial(diffuseColor, specularColor, ka, kd, ks, alpha, opacity, ior);
			materials.push_back(newMaterial);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for mtlcolor number. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("sphere")) == 0) {
		if (materials.empty()) {
			std::cerr << "ERROR : Specify mtlcolor before listings sphere in input. Exiting early." << std::endl;
			exit(1);
		}
		checkLength(std::string("sphere"), line, 4);
		try {
			Vec3 center(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
			float radius = std::stof(line[4]);
			Sphere s(center, radius, materials.size()-1, textures.size()-1);
			spheres.push_back(s);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for sphere. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("light")) == 0) {
		checkLength(std::string("light"), line, 7);
		try {
			Vec3 v(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
			int lightFlag = std::stoi(line[4]);
			Vec3 color(std::stof(line[5]), std::stof(line[6]), std::stof(line[7]));
			Light::LightType lightType;
			if (lightFlag == 0) {
				lightType = Light::LightType::DIRECTIONAL;
			} else if (lightFlag == 1) {
				lightType = Light::LightType::POINT;
			} else {
				std::cerr << "ERROR : Invalid input; light type must be either 0 or 1. Exiting early." <<std::endl;
				exit(1);
			}
			Light l(v, lightType, color);
			lights.push_back(l);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for light. Exiting early." << std::endl;
			exit(1);
		}

	} else if (line[0].compare(std::string("texture")) == 0) {
		checkLength(std::string("texture"), line, 1);
		try {
			PixelBuffer* tex = new PixelBuffer(line[1]);
			textures.push_back(tex);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for texture. Exiting Early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("v")) == 0) {
		checkLength(std::string("v"), line, 3);
		try {
			verts.push_back( Vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3])) );
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for v. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("vt")) == 0) {
		checkLength(std::string("vt"), line, 2);
		try {
			float u = std::stof(line[1]);
			float v = std::stof(line[2]);
			texCoords.push_back(std::make_pair(u,v));
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for vt. Exiting early." << std::endl;
		}
	} else if (line[0].compare(std::string("vn")) == 0) {
		checkLength(std::string("vn"), line, 3);
		try {
			norms.push_back( Vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3])).unit() );
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in values for vn. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("f")) == 0) {
		checkLength(std::string("f"), line, 3);
		int i1, i2, i3;
		int t1, t2, t3;
		int n1, n2, n3;
		loadFaceChunk(line[1], i1, t1, n1);
		loadFaceChunk(line[2], i2, t2, n2);
		loadFaceChunk(line[3], i3, t3, n3);
		// std::cout << "NORM : " << n1 << n2 << n3 << std::endl;
		TriFace tri(
			std::make_tuple(i1, i2, i3),
			std::make_tuple(t1, t2, t3),
			std::make_tuple(n1, n2, n3),
			materials.size()-1,
			textures.size()-1,
			verts
			);
		faces.push_back(tri);
	} else if (line[0].compare(std::string("parallel")) == 0) {
		isParallel = true;
	} else if (line[0].compare(std::string("viewdist")) == 0) {
		checkLength(std::string("viewdist"), line, 1);
		try {
			viewDist = 	std::stof(line[1]);
		} catch (...) {
			std::cerr << "ERROR : Invalid input; failed to read in value for viewdist. Exiting early." << std::endl;
			exit(1);
		}
	} else if (line[0].compare(std::string("#")) == 0) {
		// Do nothing, it's a comment!
	} else {
		std::cerr << "ERROR : Unknown value in input (" << line[0] << ". Exiting early." << std::endl;
		exit(1);
	}
}

void Scene::loadFaceChunk(const std::string& chunk, int& i, int& t, int& n) {
	if (chunk.find("/") == std::string::npos) {
		i = std::stoi(chunk)-1;
		t = -1;
		n = -1;
	} else {
		std::stringstream ss(chunk);
		std::string subChunk;

		std::getline(ss, subChunk, '/');
		i = std::stoi(subChunk)-1;
		

		std::getline(ss, subChunk, '/');
		if (subChunk.empty()) {
			t = -1;
		} else {
			t = std::stoi(subChunk)-1;
		}
    	if (ss.rdbuf()->in_avail() > 0) {
			std::getline(ss, subChunk, '/');
			n = std::stoi(subChunk)-1;
		} else {
			n = -1;
		}
	}
}


void Scene::checkLength(const std::string& id, const std::vector< std::string >& line, unsigned int N) {
	if (line.size()-1 != N) {
		std::cerr << "ERROR : Invalid input; " << id << " requires " << N << " values. Instead " << line.size() - 1  << " were given. Exiting early." << std::endl;
		exit(1);
	}
}

void Scene::checkMin(const std::string& id, float value, float min) {
	if (value <= min) {
		std::cerr << "ERROR : Invalid input; " << id << " requires values above " << min << ", " << value << " was given. Exiting early." << std::endl;
		exit(1);
	}
}

void Scene::checkMax(const std::string& id, float value, float max) {
	if (value >= max) {
		std::cerr << "ERROR : Invalid input; " << id << " requires values below " << max << ", " << value << " was given. Exiting early." << std::endl;
		exit(1);
	}
}

void Scene::checkVec(const std::string& id, const Vec3& v) {
	if (v.lengthSquared() == 0) {
		std::cerr << "ERROR : Invalid input; " << id << " requires a non-zero vector. exiting early." << std::endl;
		exit(1); 
	}
}

