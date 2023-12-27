/*
 * BVH.h
 *
 *
 */

#ifndef BVH_H_
#define BVH_H_

#include "core/Shape.h"
#include "core/RayHitStructs.h"
#include "core/Material.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <memory>

#include <cstddef>
#include <iostream>
#include <array>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

namespace rt{

class BVH :public Shape {

public:

    //
    // Constructors
    //
    BVH() {};

    BVH(std::vector<Shape*> src_objects,
        std::size_t start, std::size_t end,
        double time0, double time1);

    virtual ~BVH() {};

    Hit intersect(Ray ray) const {
        Hit h;
        return h;
    }

    Vec2f getUV(const Hit hit) const {
        return Vec2f(0,0);
    }

    std::string getType() const {
        return std::string("BVH");
    }

    /**
     * Test the intersection
     *
     * @param r ray
     * @param t_min min distance
     * @param t_max max distance
     * @param rec hit records
     *
     * @return true if ray hits the shape, false otherwise
     */
    bool hit(
        const Ray& r, double t_min, double t_max, Hit& rec) const {
        if (!box.hit(r, t_min, t_max))
            return false;             

        bool hit_left = left->hit(r, t_min, t_max, rec);
        bool hit_right = right->hit(r, t_min, hit_left ? rec.distance : t_max, rec);

        return hit_left || hit_right;
    }

    //
    // Helper functions for bounding boxes comparison/computation
    //
    bool bounding_box(double time0, double time1, aabb& output_box) const {
        output_box = box;
        return true;
    }    

    double random_double() const {
        // Returns a random real in [0,1).
        return rand() / (RAND_MAX + 1.0);
    }

    double random_double(double min, double max) const {
        // Returns a random real in [min,max).
        return min + (max - min) * random_double();
    }

    int random_int(int min, int max) const {
        // Returns a random integer in [min,max].
        return static_cast<int>(random_double(min, max + 1));
    }

    
    
public:
    Shape* left;
    Shape* right;
    aabb box;
    Material* left_material;
    Material* right_material;
};

//
// Helper functions for bounding boxes comparison/computation
//
static inline bool box_compare(const Shape* a, const Shape* b, int axis) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BVH constructor.\n";

    return box_a.min()[axis] < box_b.min()[axis];
}

static bool box_x_compare(const Shape* a, const Shape* b)  {
    return box_compare(a, b, 0);
}

static bool box_y_compare(const Shape* a, const Shape* b)  {
    return box_compare(a, b, 1);
}

static bool box_z_compare(const Shape* a, const Shape* b)  {
    return box_compare(a, b, 2);
}


} //namespace rt



#endif /* BVH_H_ */
