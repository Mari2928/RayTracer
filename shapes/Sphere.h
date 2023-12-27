/*
 * Sphere.h
 *
 *
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "math/geometry.h"
#include "core/RayHitStructs.h"
#include "core/Shape.h"
#include "BVH.h"
#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>

#include <atomic>

namespace rt{

class Sphere:public Shape{

public:	

	//
	// Constructors and destructor
	//
	Sphere() {}
	Sphere(Vec3f center, float radius, std::string type, Material* material) :center(center), radius(radius), type(type), material(material), Shape(material){};

	virtual ~Sphere() {}

	//
	// Functions that need to be implemented, since Sphere is a subclass of Shape
	//		

	/**
	 * Computes whether a ray hit the specific instance of a sphere shape and returns the hit data
	 *
	 * @param ray cast ray to check for intersection with shape
	 *
	 * @return hit struct containing intersection information
	 *
	 */
	Hit intersect(Ray ray) const {		

		Hit h;

		// subtract sphere center
		Vec3f origin = ray.origin - center;	

		// compute sphere intersection equation
		float a = ray.direction.dotProduct(ray.direction);
		float b = 2.0f * origin.dotProduct(ray.direction);
		float c = origin.dotProduct(origin) - radius * radius;
		float discriminant = b * b - 4.0f * a * c;		

		// ray hits the sphere: compute hit elements 
		if (discriminant < 0.0f) {
			h.hittable = false;
			return h;
		}
		//if (discriminant >= 0.0f) {
			// solve for t and take a closest t of solution to get a hit point
			h.distance = (-b - std::sqrt(discriminant)) / (2.0f * a);

			// store records
			Vec3f hitPoint(origin + ray.direction * h.distance);
			hitPoint = hitPoint.normalize();
			h.point = hitPoint;
			Vec3f outward_normal = (h.point - center) * (1 / radius);
			h.set_face_normal(ray, outward_normal);
			h.shape = std::string("shape");
			h.hittable = true;	
			h.normal = (hitPoint - center).normalize();

		return h;
	}

	/**
	 * Computes U and V vectors to map texture image pixels on sphere.
	 * Source: https://www.youtube.com/watch?v=d35gDi98gEY, explains UV space for texture mapping.	 *
	 * @param pos the hit position (x and y coordinates)
	 *
	 * @return pos the mapped hit postion
	 *
	 */
	Vec2f getUV(const Hit hit) const {
		Vec3f d = (center - hit.point).normalize();

		float u = 0.5f - atan2(d.x, d.z) / (2.0f * M_PI);
		float v = 0.5f + asin(d.y) / M_PI;

		return Vec2f(u, v);
	}

	std::string getType() const {
		return this->type;
	}

	//
	// Intersection test function for BVH version: returns true if ray hits any object, false otherwise
	//
	bool hit(
	    const Ray& r, double t_min, double t_max, Hit& rec) const {
	    Vec3f oc = r.origin - center;
	        
	    auto a = r.direction.length() * r.direction.length(); 
	    auto half_b = oc.dotProduct(r.direction);
	    auto c = (oc.length() * oc.length()) - radius * radius; 

	    auto discriminant = half_b * half_b - a * c;
		if (discriminant < 0) {
			rec.hittable = false;
			return false;
		}
	    auto sqrtd = sqrt(discriminant);
	
	    // Find the nearest root that lies in the acceptable range.
	    auto root = (-half_b - sqrtd) / a;
	    if (root < t_min || t_max < root) {
	        root = (-half_b + sqrtd) / a;
			if (root < t_min || t_max < root) {
				rec.hittable = false;
				return false;
			}	            
	    }

		// store records
	    rec.distance = root;  
		Vec3f hitPoint(oc + r.direction * root);
		hitPoint = hitPoint.normalize();
		rec.point = hitPoint; //oc + rec.distance * r.direction; 
	    Vec3f outward_normal = (rec.point - center) * (1 / radius);
	    rec.set_face_normal(r, outward_normal);
		rec.shape = std::string("shape");
		rec.hittable = true;
		rec.normal = (rec.point - center).normalize();
		rec.material = material;
		rec.uv = getUV(rec);

	    return true;
	}
	
	//
	// Compute bounding box for sphere
	//
	bool bounding_box(double time0, double time1, aabb& output_box) const {
	    output_box = aabb(
	        center - Vec3f(radius, radius, radius),
	        center + Vec3f(radius, radius, radius));
	    return true;
	}

	
	

private:
	std::string type;

	Vec3f center;
	float radius;
	Material* material;
	Vec2f uv;
};



} //namespace rt




#endif /* SPHERE_H_ */
