/*
 * BlinnPhong.h
 *
 *
 */

#ifndef BLINNPHONG_H_
#define BLINNPHONG_H_

#include "core/Material.h"
#include "math/geometry.h"
#include "core/RayHitStructs.h"

namespace rt{

class BlinnPhong: public Material{

public:
	//
	// Constructor and destructor
	//
	BlinnPhong() {};

	virtual ~BlinnPhong() {};


	//
	// Math functions for reflections and refractions 
	//
	Vec3f getReflectedColor(Vec3f rayDirection, Hit hitPoint, Material* material, Vec3f lightIntensity, Vec3f color, Vec3f lightDirection, bool isVisible);

	float clamp(const float& lo, const float& hi, const float& v);

	Vec3f reflect(Vec3f I, Vec3f N);

	Vec3f refract(Vec3f I, Vec3f N, float ior);
};



} //namespace rt



#endif /* BLINNPHONG_H_ */
