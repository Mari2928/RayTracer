/*
 * ThinLens.cpp
 *
 *
 */
#include <iostream>


#include "ThinLens.h"




namespace rt{

	//
	// ThinLens constructor (example)
	//
	ThinLens::ThinLens(int width, int height, int fov, Vec3f position, Vec3f lookat, Vec3f up):Camera(width, height, fov, position, lookat, up){

		//to fill

	}

	/**
	 * Prints camera data
	 * this function implements the abstract function of the base Camera class
	 */
	void ThinLens::printCamera(){
		printf("I am a thin lens camera! \n");
		printf("width: %dpx, height: %dpx, fov:%d \n", width, height, fov);
	}

	/**
	 * Generate camera ray
	 * this function implements the abstract function of the base Camera class
	 */
	Ray ThinLens::getCameraRay(float x, float y)  {
		Ray ray;
		return ray;
	}

} //namespace rt
