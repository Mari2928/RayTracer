/*
 * RayHitStructs.h
 */

#ifndef CORE_RAYHITSTRUCTS_H_
#define CORE_RAYHITSTRUCTS_H_

#include "math/geometry.h"
#include "Material.h"

namespace rt{

/*
 * Ray structure definition
 */
enum RayType {PRIMARY, SECONDARY, SHADOW};

struct Ray{

	RayType raytype;

	//---------- Ray variables ------
	Vec3f direction;
	Vec3f origin;
	
};


struct Hit{

	Vec3f point; //point where ray hits a shape

	//----------Hit variables ------
	float distance;
	bool hittable;
	Vec3f normal;
	std::string shape;
	bool front_face;
	Material* material;
	Vec2f uv;
	uint32_t triIndex;


	inline void set_face_normal(const Ray& r, const Vec3f& outward_normal) {
		front_face = r.direction.dotProduct(outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

}


#endif /* CORE_RAYHITSTRUCTS_H_ */
