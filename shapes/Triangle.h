/*
 * Triangle.h
 *
 *
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "math/geometry.h"
#include "core/RayHitStructs.h"
#include "core/Shape.h"
#include "BVH.h"

namespace rt{

class Triangle: public Shape{

public:

	//
	// Constructors and destructor
	//
	Triangle() {};
	Triangle(Vec3f v0, Vec3f v1, Vec3f v2, std::string type, Material* material) :v0(v0), v1(v1), v2(v2), type(type), material(material), Shape(material) {};

	virtual ~Triangle() {};

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
    Vec2f getUV(const Hit hit) const {
        //barycentric interpolation
        float totalArea = getTriangleArea(v0, v1, v2);
        float u = getTriangleArea(v1, v2, hit.point) / totalArea;
        float v = getTriangleArea(v0, hit.point, v2) / totalArea;
        float w = getTriangleArea(v0, v1, hit.point) / totalArea;

        // nearest neighbor
        Vec2f P = u * Vec2f(0, 1) + v * Vec2f(1, 1) + w * Vec2f(1, 0);
        if (P.x > 1.0f) {
            P.x = 1.0f;
        }
        if (P.y > 1.0f) {
            P.y = 1.0f;
        }
        return P;
    }
    

    float getTriangleArea(Vec3f a, Vec3f b, Vec3f c) const
    {
        Vec3f ab = b - a;
        Vec3f ac = c - a;
        return ab.crossProduct(ac).length() / 2.0f;
    }

	//
	// Functions that need to be implemented, since Sphere is a subclass of Shape
	//
    Hit intersect(Ray ray) const {
        Hit h;

        // compute the triangle's normal
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
            h.hittable = false; // P is on the right side
            return h;
        }

        // edge 1
        Vec3f edge1 = v2 - v1;
        Vec3f vp1 = P - v1;
        C = edge1.crossProduct(vp1);
        if ((N.dotProduct(C)) < 0) {
            h.hittable = false; // P is on the right side
            return h;
        }

        // edge 2
        Vec3f edge2 = v0 - v2;
        Vec3f vp2 = P - v2;
        C = edge2.crossProduct(vp2);
        if ((N.dotProduct(C)) < 0) {
            h.hittable = false; // P is on the right side
            return h;
        }

        // this ray hits the triangle: store hit elements
        h.distance = t;
        Vec3f hitPoint(ray.origin + ray.direction * h.distance);
        hitPoint = hitPoint.normalize();
        h.point = hitPoint;
        h.shape = std::string("triangle");

        h.hittable = true;
        h.normal = N;

        return h;
	};

    std::string getType() const {
        return type;
    }


    //
    // Intersection test function for BVH version: returns true if ray hits any object, false otherwise
    //
    bool hit(
        const Ray& ray, double t_min, double t_max, Hit& h) const {

        // compute the triangle's normal
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
            return false;
        }

        // compute d parameter using equation 2
        float d = -N.dotProduct(v0);

        // compute t, distance from ray origin to intersection (equation 3)
        float t = -(N.dotProduct(ray.origin) + d) / NdotRayDirection;
        // check if the triangle is behind the ray
        if (t < 0) {
            h.hittable = false;
            return false;
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
            h.hittable = false; // P is on the right side
            return false;
        }

        // edge 1
        Vec3f edge1 = v2 - v1;
        Vec3f vp1 = P - v1;
        C = edge1.crossProduct(vp1);
        if ((N.dotProduct(C)) < 0) {
            h.hittable = false; // P is on the right side
            return false;
        }

        // edge 2
        Vec3f edge2 = v0 - v2;
        Vec3f vp2 = P - v2;
        C = edge2.crossProduct(vp2);
        if ((N.dotProduct(C)) < 0) {
            h.hittable = false; // P is on the right side
            return false;
        }

        // this ray hits the triangle: store hit elements
        h.distance = t;
        Vec3f hitPoint(ray.origin + ray.direction * h.distance);
        hitPoint = hitPoint.normalize();
        h.point = hitPoint;
        h.shape = std::string("triangle");
        h.set_face_normal(ray, N);
        h.material = material;
        h.uv = getUV(h);
        h.hittable = true;
        h.normal = N;

        return true;
    }

    //
    // Compute bounding box for triangle
    //
    bool bounding_box(double time0, double time1, aabb& output_box) const {
        std::vector<Vec3f> vertexCoords;
        vertexCoords.push_back(v0);
        vertexCoords.push_back(v1);
        vertexCoords.push_back(v2);

        // compute bounds
        Vec3f min = v0;
        Vec3f max = v0;
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


private:
    std::string type;
	Vec3f v0, v1, v2;
    Material* material;
    
};



} //namespace rt




#endif /* TRIANGLE_H_ */
