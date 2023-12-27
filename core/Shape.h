/*
 * Shape.h
 *
 */

#ifndef SHAPE_H_
#define SHAPE_H_


#include "core/RayHitStructs.h"
#include "core/Material.h"
#include "rapidjson/document.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <memory>

#include <cstddef>
#include <iostream>
#include <array>


using namespace rapidjson;

namespace rt{

    /*
     * AABB (Axis-Aligned Bounding Boxes) class declaration
     * Source: https://raytracing.github.io/books/RayTracingTheNextWeek.html
     */
    class aabb {

    public:
        aabb() {}
        aabb(const Vec3f& a, const Vec3f& b) { minimum = a; maximum = b; }

        Vec3f min() const { return minimum; }
        Vec3f max() const { return maximum; }

        bool hit(const Ray& r, double t_min, double t_max) const {
            for (int a = 0; a < 3; a++) {
                auto t0 = fmin((minimum[a] - r.origin[a]) / r.direction[a],
                    (maximum[a] - r.origin[a]) / r.direction[a]);
                auto t1 = fmax((minimum[a] - r.origin[a]) / r.direction[a],
                    (maximum[a] - r.origin[a]) / r.direction[a]);
                t_min = fmax(t0, t_min);
                t_max = fmin(t1, t_max);
                if (t_max <= t_min)
                    return false;
            }
            return true;
        }

        double area() const {
            auto a = maximum.x - minimum.x;
            auto b = maximum.y - minimum.y;
            auto c = maximum.z - minimum.z;
            return 2 * (a * b + b * c + c * a);
        }

        int longest_axis() const {
            auto a = maximum.x - minimum.x;
            auto b = maximum.y - minimum.y;
            auto c = maximum.z - minimum.z;
            if (a > b && a > c)
                return 0;
            else if (b > c)
                return 1;
            else
                return 2;
        }

    public:
        Vec3f minimum;
        Vec3f maximum;
    };


    static aabb surrounding_box(aabb box0, aabb box1) {
        Vec3f small(fmin(box0.min().x, box1.min().x),
            fmin(box0.min().y, box1.min().y),
            fmin(box0.min().z, box1.min().z));

        Vec3f big(fmax(box0.max().x, box1.max().x),
            fmax(box0.max().y, box1.max().y),
            fmax(box0.max().z, box1.max().z));

        return aabb(small, big);
    }

class Shape{
public:

	//
	// Constructors
	//
	Shape() {}
	Shape(Material* material) :material(material) {};

	//
	// Destructor (must be overriden in subclass)
	//
	virtual ~Shape() {}
	

	//
	// Shape abstract methods (to be implemented by subclasses)
	//
	virtual Hit intersect(Ray) const=0;
	virtual std::string getType() const = 0;
	virtual Vec2f getUV(const Hit hit) const = 0;
	virtual bool hit(const Ray& r, double t_min, double t_max, Hit& rec) const = 0;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;

	//
	// Getter
	//
	Material* getMaterial() const {
		return material;
	}

private:

	Material* material;


protected:

	

};



} //namespace rt


#endif /* SHAPE_H_ */
