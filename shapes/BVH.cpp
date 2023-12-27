/*
 * BVH.cpp
 *
 *
 */
#include "BVH.h"


namespace rt{

    /**
     * Factory function that returns BVH node subclass based on shapes specifications
     * Source: https://raytracing.github.io/books/RayTracingTheNextWeek.html
     *
     * @return BVH node subclass instance
     *
     */
    BVH::BVH(std::vector<Shape*> src_objects, std::size_t start, std::size_t end, double time0, double time1)
    {
        auto objects = src_objects; // Create a modifiable array of the source scene objects

        int axis = random_int(0, 2);
        auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;


        std::size_t object_span = end - start;
        
        if (object_span == 1) {          
            left = right = objects[start];
        }
        else if (object_span == 2) {
            if (comparator(objects[start], objects[start+1])) {
                left = objects[start];
                right = objects[start+1];
            }
            else {
                left = objects[start+1];
                right = objects[start];
            }
        }
        else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            std::size_t mid = start + object_span / 2;
            left = new BVH(objects, start, mid, time0, time1);
            right = new BVH(objects, mid, end, time0, time1);
        }

        aabb box_left, box_right;

        if (!left->bounding_box(time0, time1, box_left)
            || !right->bounding_box(time0, time1, box_right)
            )
            std::cerr << "No bounding box in BVH constructor.\n";

        box = surrounding_box(box_left, box_right);

    }

} //namespace rt


