/*
 * PointLight.h
 *
 *
 */

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "core/LightSource.h"
#include "math/geometry.h"
#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>


namespace rt{

class PointLight: public LightSource{

public:

	//
	// Constructors
	//
	PointLight() {};
	PointLight(Vec3f position) : LightSource(position) {};

	//
	// Destructor (must be overriden in subclass)
	//
	virtual ~PointLight() {};	


	/**
	 * Computes light intensity
	 *
	 * @param hitPoint ray hitpoint
	 * @param lightPos light position
	 * @return lightIntensity vector containing light intensity information
	 *
	 */
	Vec3f getLightIntensity(Vec3f hitPoint, Vec3f lightDir) const {
		return std::max(hitPoint.dotProduct(-lightDir), 0.0f);
	}

	/**
	 * Compute light information for illumination
	 *
	 * @param P ray hitpoint
	 * @param lightDir light direction to be computed 
	 * @param lightIntensity light intensity to be computed 
	 * @param distance distance from camera position to hitpoint
	 *
	 */
	void illuminate(const Vec3f& P, Vec3f& lightDir, Vec3f& lightIntensity, float& distance) const {
		lightDir = (P - position);
		float r2 = lightDir.norm();
		distance = sqrt(r2);
		lightDir.x /= distance, lightDir.y /= distance, lightDir.z /= distance;
		// avoid division by 0
		float denom = 1 / (4 * M_PI * r2);
		lightIntensity = (color * intensity) * denom;	
	}

};



} //namespace rt




#endif /* POINTLIGHT_H_ */
