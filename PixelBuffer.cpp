#include "PixelBuffer.hpp"

PixelBuffer::PixelBuffer(int width, int height) :
		_width(width),
		_height(height)
{
	_pixels = std::vector<float>(width*height*3);
}

PixelBuffer::PixelBuffer(const std::string& filename) {
	std::ifstream inFile;
	inFile.open(filename.c_str());
	if (!inFile) {
		std::cerr << "ERROR : Failed to find file (" << filename << "). Exiting." << std::endl;
		exit(1);
	}
	// hardcode to consume some input lines
	// this is likely to break on bad input, but #yolo
	std::string s;
	inFile >> s; // consume p3
		// std::cout << s << std::endl;
	inFile >> s;
		// std::cout << s << std::endl;
	_width = std::stoi(s);
	inFile >> s;
		// std::cout << s << std::endl;
	_height = std::stoi(s);
	inFile >> s;
		// std::cout << s << std::endl;
	_pixels = std::vector<float>(_width * _height * 3);
	for (int i=0; i < 3  * _width * _height; i++) {
		inFile >> s;
		// std::cout << s << std::endl;
		_pixels[i] = std::stof(s) / 255.f;
	}
}


void PixelBuffer::setPixel(int i, int j, float r, float g, float b) {
	int index = 3 * (j * _width + i);
	_pixels[ index ]     = r;
	_pixels[ index + 1 ] = g;
	_pixels[ index + 2 ] = b;
}

Vec3 PixelBuffer::getColor(float u, float v) const {
	int y = _width - 1;
	int x = (int) (u * y);
	// do positive modulo so you can get texture wrapping outside [0, 1]
	int i = (x % y + y) % y;
	y = _height - 1;
	x = (int) (v * y);
	int j = (x % y + y) % y;
	int index = 3 * (j * _width + i);
	return Vec3(_pixels[index],
				_pixels[index+1],
				_pixels[index+2]);
}


void PixelBuffer::toFile(const std::string& filename) {
	// if (imageFormat == ImageFormat::PPM) {
		toFilePPM(filename);
	// }
	/// other formats added here.
}


void PixelBuffer::toFilePPM(const std::string& filename) {
	std::ofstream out;
	std::string filenameOut = filename + ".ppm";
	out.open(filenameOut.c_str());
	out << "P3" << std::endl;
	out << _width << " " << _height << std::endl;
	out << "255" << std::endl;
	for (unsigned int i=0; i < _pixels.size(); i++) {
		if ( i % 12 == 0) {
			out << "\n";
		}
		int c = clamp(_pixels[i] * 255, 0, 255);
		out << c << " ";
	}
	out << std::endl;
	out.close();
}


float PixelBuffer::clamp(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}