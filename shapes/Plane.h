/*
 * Plane.h
 *
 *
 */

#ifndef PLANE_H_
#define PLANE_H_

#include "math/geometry.h"
#include "core/RayHitStructs.h"
#include "core/Shape.h"
#include "shapes/Triangle.h"
#include "BVH.h"
#include <vector>
#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>

namespace rt {

    class Plane : public Shape {

    public:

        //
        // Constructors
        //
        Plane() {};

        Plane(Vec3f v0, Vec3f v1, Vec3f v2, Vec3f v3, std::string type, Material* material) :
            v0_1(v0), v1_1(v1), v2_1(v2), v0_2(v3), v1_2(v2), v2_2(v1), type(type), material(material), Shape(material) {};//ok 3,2,1 ; 3,1,2

        //
        // Destructor
        //
        virtual ~Plane() {};

        //
        // Intersection test function for BVH version: returns true if ray hits any object, false otherwise
        //
        bool hit(
            const Ray& ray, double t_min, double t_max, Hit& rec) const {

            Hit h = triangleIntersect(ray, v0_1, v1_1, v2_1);
            Hit h2 = triangleIntersect(ray, v0_2, v1_2, v2_2);
            if (h.hittable) {
                rec.shape = std::string("lower triangle");
                rec.distance = h.distance;
                Vec3f hitPoint(ray.origin + ray.direction * h.distance);
                hitPoint = hitPoint.normalize();
                rec.point = hitPoint;
                rec.set_face_normal(ray, h.normal);
                rec.material = material;
                rec.uv = getUV(h);
                rec.hittable = true;
                rec.normal = h.normal;
                return true;
            }
            else if (h2.hittable) {
                rec.shape = std::string("upper triangle");
                rec.distance = h2.distance;
                Vec3f hitPoint(ray.origin + ray.direction * h2.distance);
                hitPoint = hitPoint.normalize();
                rec.point = hitPoint;
                rec.set_face_normal(ray, h2.normal);
                rec.material = material;
                rec.uv = getUV(h2);
                rec.hittable = true;
                rec.normal = h2.normal;
                return true;
            }
            rec.hittable = false;
            return false;
        }

        //
        // Compute bounding box for plane
        //
        bool bounding_box(double time0, double time1, aabb& output_box) const {
            std::vector<Vec3f> vertexCoords;
            vertexCoords.push_back(v0_1);
            vertexCoords.push_back(v1_1);
            vertexCoords.push_back(v2_1);

            // compute bounds
            Vec3f min = v0_1;
            Vec3f max = v0_1;
            for (auto v : vertexCoords) {
                if (v.x < min.x)
                    min.x = v.x;
                if (v.x > max.x)
                    max.x = v.x;

                if (v.y < min.y)
                    min.y = v.y;
                if (v.y > max.y)
                    max.y = v.y;

                if (v.z < min.z)
                    min.z = v.z;
                if (v.z > max.z)
                    max.z = v.z;
            }
            output_box = aabb(min, max);
            return true;
        }

        /**
         * Computes U and V reflection vectors to map texture image pixels on sphere.
         * Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html,
         * UV mapping the primitives (page.23).
         *
         * @param pos the hit position (x and y coordinates)
         *
         * @return pos the mapped hit postion
         *
         */
        Vec2f getUV(Hit hit) const
        {
            // compute UV mapping using barycentric interpolation
            float totalArea, alpha, beta, gamma;
            Vec2f tex;

            // position is at lower triangle 
            if (hit.shape == std::string("lower triangle")) {
                totalArea = getTriangleArea(v0_1, v1_1, v2_1);
                alpha = getTriangleArea(v1_1, v2_1, hit.point) / totalArea;
                beta = getTriangleArea(v0_1, hit.point, v2_1) / totalArea;
                gamma = getTriangleArea(hit.point, v0_1, v1_1) / totalArea;
                tex = alpha * Vec2f(0, 1) + beta * Vec2f(1, 1) + gamma * Vec2f(1, 0);
            }
            // position is at upper triangle 
            else {
                totalArea = getTriangleArea(v0_2, v1_2, v2_2);
                alpha = getTriangleArea(v1_2, v2_2, hit.point) / totalArea;
                beta = getTriangleArea(v0_2, hit.point, v2_2) / totalArea;
                gamma = getTriangleArea(v0_2, v1_2, hit.point) / totalArea;
                tex = alpha * Vec2f(0, 1) + beta * Vec2f(1, 0) + gamma * Vec2f(0, 0);
                //tex = alpha * Vec2f(0, 0) + beta * Vec2f(1, 0) + gamma * Vec2f(0, 1);
            }

            //nearest neighbor
            if (tex.x > 1.0f) {
                tex.x = 1.0f;
            }
            if (tex.y > 1.0f) {
                tex.y = 1.0f;
            }
            return tex;
        }

        /**
         * Computes U and V reflection vectors to map texture image pixels on sphere.
         * Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html,
         * UV mapping the primitives (page.23).
         *
         * @return triangle area
         *
         */
        float getTriangleArea(Vec3f a, Vec3f b, Vec3f c)const
        {
            Vec3f ab = b - a;
            Vec3f ac = c - a;
            return ab.crossProduct(ac).length() / 2.0f;
        }

        /**
         * Computes U and V reflection vectors to map texture image pixels on sphere.
         * Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html,
         * UV mapping the primitives (page.23).
         *
         * @return hit records
         *
         */
        Hit triangleIntersect(Ray ray, Vec3f v0, Vec3f v1, Vec3f v2) const {
            Hit h;

            // compute the plane's normal
            Vec3f v0v1 = v1 - v0;
            Vec3f v0v2 = v2 - v0;
            // no need to normalize
            Vec3f N = (v0v1.crossProduct(v0v2)).normalize(); // N
            float denom = N.dotProduct(N);

            // Step 1: finding P

            // check if the ray and plane are parallel.
            float NdotRayDirection = N.dotProduct(ray.direction);
            if (fabs(NdotRayDirection) < 1e-8) {  // they are parallel 
                h.hittable = false;
                return h;
            }

            // compute d parameter using equation 2
            float d = -N.dotProduct(v0);

            // compute t, distance from ray origin to intersection (equation 3)
            float t = -(N.dotProduct(ray.origin) + d) / NdotRayDirection;
            // check if the triangle is behind the ray
            if (t < 0) {
                h.hittable = false;
                return h;
            }

            // compute the intersection point using equation 1
            Vec3f P = ray.origin + t * ray.direction;

            // Step 2: inside-outside test
            Vec3f C; // vector perpendicular to triangle's plane

            // edge 0
            Vec3f edge0 = v1 - v0;
            Vec3f vp0 = P - v0;
            C = edge0.crossProduct(vp0);
            if (N.dotProduct(C) < 0) {
                // P is on the right side
                h.hittable = false;
                return h;
            }

            // edge 1
            Vec3f edge1 = v2 - v1;
            Vec3f vp1 = P - v1;
            C = edge1.crossProduct(vp1);
            if ((N.dotProduct(C)) < 0) {
                // P is on the right side
                h.hittable = false;
                return h;
            }

            // edge 2
            Vec3f edge2 = v0 - v2;
            Vec3f vp2 = P - v2;
            C = edge2.crossProduct(vp2);
            if ((N.dotProduct(C)) < 0) {
                // P is on the right side
                h.hittable = false;
                return h;
            }

            // this ray hits the triangle: store hit elements    
            h.distance = t;
            Vec3f hitPoint(ray.origin + ray.direction * t);
            hitPoint = hitPoint.normalize();
            h.point = hitPoint;
            h.normal = N;
            h.hittable = true;

            return h;
        };

        Hit intersect(Ray ray) const {

            Hit h = triangleIntersect(ray, v0_1, v1_1, v2_1);
            Hit h2 = triangleIntersect(ray, v0_2, v1_2, v2_2);
            if (h.hittable) {
                h.shape = std::string("lower triangle");
                return h;
            }
            else if (h2.hittable) {
                h2.shape = std::string("upper triangle");
                return h2;
            }
            return h;
        };

        std::string getType() const {
            return type;
        }



    private:
        std::string type;
        Vec3f v0_1, v1_1, v2_1;
        Vec3f v0_2, v1_2, v2_2;

        Material* material;

    };



} //namespace rt




#endif /* PLANE_H_ */
