
CFLAGS=-std=c++0x

all: run

run: main.o RayTracer.o PixelBuffer.o Scene.o
	g++ main.o RayTracer.o PixelBuffer.o Vec3.o Scene.o -o run $(CFLAGS)

main.o: main.cpp
	g++ -c main.cpp $(CFLAGS)

RayTracer.o: RayTracer.cpp Ray3.hpp Vec3.o Sphere.hpp PixelBuffer.o
	g++ -c RayTracer.cpp $(CFLAGS)

Vec3.o:
	g++ -c Vec3.cpp $(CFLAGS)

PixelBuffer.o: PixelBuffer.cpp Vec3.o
	g++ -c PixelBuffer.cpp $(CFLAGS)

Scene.o: Scene.cpp Vec3.o Sphere.hpp PixelBuffer.o TriFace.hpp
	g++ -c Scene.cpp $(CFLAGS)

clean:
	rm *o run