#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "RayTracer.hpp"
#include "PixelBuffer.hpp"
#include "Scene.hpp"

void drawGridGradientCircle(PixelBuffer& buf) {
	float midX = buf.width() / 2.f;
	float midY = buf.height() / 2.f;
	float maxDistSquared = midX * midX + midY * midY;
	for (int i=0; i < buf.width(); i++) {
		for (int j=0; j < buf.height(); j++) {
			float distX = (midX - i);
			float distY = (midY - j);
			float distSquared = distX * distX + distY * distY;
			float t = 1 - distSquared / maxDistSquared;
			if ( t < 0.90) {
				if ( (i + j) % 2 == 0 ) {
					buf.setPixel(i,j, t * .75, t * .75, t * .75);
				} else {
					buf.setPixel(i,j, t * .25, t * .25, t * .25);
				}
			} else {
				if (fmod(t * 1000.f, 1.f) < .5) {
					buf.setPixel(i,j, 0.8f,0.3f,0.1f);
				} else {
					buf.setPixel(i,j, 0.9,.51,.22);
				}
			}
		}
	}
}

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		std::cerr << "ERROR : Please specify a input file." << std::endl;
		exit(1);
	}
	std::string filename = argv[1];
	Scene scene(filename);
	std::cout << filename << std::endl;
	PixelBuffer buf = RayTracer::rayTrace(scene);

	// file splitting code referenced form
	// http://www.cplusplus.com/reference/string/string/find_last_of/


	int index = filename.find_last_of("/\\");
	std::string path = filename.substr(0,index);
	std::string name = filename.substr(index+1);
	std::stringstream ss;
	ss << name;
	std::string namebase;
	std::getline(ss, namebase, '.');

	buf.toFile(namebase);
	return 0;
}