/*
 * Pinhole.h
 *
 *
 */

#ifndef PINHOLE_H_
#define PINHOLE_H_

#include "core/Camera.h"
#include "math/geometry.h"
#include "core/RayHitStructs.h"

namespace rt{

class Pinhole:public Camera{
public:

	//
	// Constructors
	//
	Pinhole();
	Pinhole(int width, int height, int fov, Vec3f position, Vec3f lookat, Vec3f up);

	//
	//Destructor
	//
	~Pinhole(){};

	//
	// print function (implementing abstract function of base class)
	//
	void printCamera();

	//
	// camera ray function (implementing abstract function of base class)
	//
	Ray getCameraRay(float x, float y);

	
};



} //namespace rt




#endif /* PINHOLE_H_ */
