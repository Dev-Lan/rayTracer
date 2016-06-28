#ifndef PIXELBUFFER_HPP
#define PIXELBUFFER_HPP

#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include "Vec3.hpp"

/// For now this buffer will use a float assumed to be between 0 and 1 for each channel with no alpha.

/// This class holds the pixels as a flat array.
class PixelBuffer {
public:
	/// I want to support other file formats in the future.
	enum ImageFormat
	{
		PPM,
		PNG
	};

	/// Creates empty pixel buffer.
	PixelBuffer(int width, int height);

	/// Creates a pixel buffer from a ppm file.
	PixelBuffer(const std::string& filename);

	/// outputs pixel data to file with specified filename
	void toFile(const std::string& filename);

	/// set color of pixel at point (i,j) to color (r,g,b). color is assumed to be between 0 and 1
	void setPixel(int i, int j, float r, float g, float b);

	/// returns width of buffer
	inline int width() const { return _width; }
	/// returns height of buffer
	inline int height() const { return _height; }

	/// returns color at specific points in texture coordinates
	Vec3 getColor(float u, float v) const;

protected:
	int _width;
	int _height;
	/// flat list of pixel color information. r,g,b color channels are interleaved.
	std::vector< float > _pixels;

private:
	/// specifically output to ppm file format
	void toFilePPM(const std::string& filename);
	/// returns n between lower and upper
	float clamp(float n, float lower, float upper);
};

#endif
// PIXELBUFFER_HPP