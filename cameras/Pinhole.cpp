/*
 * Pinhole.cpp
 *
 *
 */
#include <iostream>
#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>

#include "Pinhole.h"



namespace rt{

	//
	// Pinhole constructor (example)
	//
	Pinhole::Pinhole(int width, int height, int fov, Vec3f position, Vec3f lookat, Vec3f up)
		:Camera(width, height, fov, position, lookat, up){}	


	/**
	 * Prints camera data
	 * this function implements the abstract function of the base Camera class
	 */
	void Pinhole::printCamera(){
		printf("I am a pinhole camera! \n");
		printf("width: %dpx, height: %dpx, fov:%d \n", width, height, fov);
	}

	/**
	 * Generate camera ray
	 * this function implements the abstract function of the base Camera class
	 */
	Ray Pinhole::getCameraRay(float x, float y) {
		Ray ray;	

		float aspectRatio = float(width) / float(height);
		auto theta = fov * M_PI / 180;
		auto h = tan(theta / 2);
		float focalLength = width / (2 * tan(theta/2));

		Vec3f vup(0, -1, 0);
		Vec3f camDirection(lookat - position);
		camDirection = camDirection.normalize();
		Vec3f right = camDirection.crossProduct(vup.normalize());
		Vec3f up = right.crossProduct(camDirection);		

		Vec3f image_point = x * -right + y * up + position + camDirection;
		ray.direction = image_point - position;
		ray.direction.normalize();
		ray.origin = position;

		return ray;
	}

} //namespace rt

