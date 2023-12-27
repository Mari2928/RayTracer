/*
 * TriMesh.h
 *
 *
 */

#ifndef TRIMESH_H_
#define TRIMESH_H_

#include "math/geometry.h"
#include "core/RayHitStructs.h"
#include "core/Shape.h"
#include "shapes/Triangle.h"
#include "BVH.h"
#include <vector>
#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>

namespace rt{

class TriMesh: public Shape{

public:

	//
	// Constructors and destructor
	//
	TriMesh() {};

    TriMesh(
        const uint32_t nfaces,
        const std::unique_ptr<uint32_t[]>& faceIndex,
        const std::unique_ptr<uint32_t[]>& vertsIndex,
        const std::unique_ptr<Vec3f[]>& verts,
        std::unique_ptr<Vec3f[]>& normals,
        std::unique_ptr<Vec2f[]>& st,
        Material* material);


	virtual ~TriMesh() {};

    //
    // Abstract functions
    //
    std::string getType() const { return type; }

    /**
     * Computes whether a ray hit the specific instance of a trimesh shape and returns the hit data
     *
     * @param ray cast ray to check for intersection with shape
     *
     * @return hit struct containing intersection information
     *
     */
    Hit intersect(Ray ray) const {

        Hit h;
        uint32_t j = 0;
        h.hittable = false;
        h.distance = std::numeric_limits<float>::max();
        for (uint32_t i = 0; i < numTris; ++i) {
            const Vec3f& v0 = P[trisIndex[j]];
            const Vec3f& v1 = P[trisIndex[j + 1]];
            const Vec3f& v2 = P[trisIndex[j + 2]];
            float t = std::numeric_limits<float>::max(), u, v;
            Hit hitTri = triangleIntersect(ray, v0, v1, v2);
            if (hitTri.hittable && hitTri.distance < h.distance) {
                Vec3f hitPoint(ray.origin + ray.direction * hitTri.distance);
                h.point = hitPoint.normalize();
                h.distance = hitTri.distance;
                h.uv = hitTri.uv;
                h.normal = hitTri.normal;
                h.triIndex = i;
                h.hittable = true;
            }
            j += 3;
        }
        return h;
    };

    // to be implemented for BVH
    bool hit(
        const Ray& r, double t_min, double t_max, Hit& rec) const {

        return true;
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const {
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
        return hit.uv;
    }

    /**
     * Computes whether a ray hit the specific instance of a triangle shape and returns the hit data
     *
     * @param ray cast ray to check for intersection with shape
     * @param v0, v1, v2 triangle vertices
     *
     * @return hit struct containing intersection information
     *
     */
    Hit triangleIntersect(Ray ray, Vec3f v0, Vec3f v1, Vec3f v2) const{
        Hit h;

        // compute the plane's normal
        Vec3f v0v1 = v1 - v0;
        Vec3f v0v2 = v2 - v0;
        // no need to normalize
        Vec3f N = (v0v1.crossProduct(v0v2)).normalize(); // N
        float denom = N.dotProduct(N);

        Vec3f pvec =ray.direction.crossProduct(v0v2);
        float det = v0v1.dotProduct(pvec);

        // Step 1: finding P

        // check if the ray and plane are parallel.
        float NdotRayDirection = N.dotProduct(ray.direction);
        if (fabs(NdotRayDirection) < 1e-8) {  // they are parallel 
            h.hittable = false;
            return h;
        }

        // compute UV 
        float invDet = 1 / det;
        Vec3f tvec = ray.origin - v0;
        float u = tvec.dotProduct(pvec) * invDet;
        Vec3f qvec = tvec.crossProduct(v0v1);
        float v = ray.direction.dotProduct(qvec) * invDet;
        h.uv = Vec2f(u, v);

        // compute d parameter 
        float d = -N.dotProduct(v0);

        // compute t, distance from ray origin to intersection 
        float t = -(N.dotProduct(ray.origin) + d) / NdotRayDirection;
        // check if the triangle is behind the ray
        if (t < 0) {
            h.hittable = false;
            return h;
        }

        // compute the intersection point 
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



    //
    // Trimesh members
    //
    uint32_t numTris;                        // number of triangles
    std::unique_ptr<uint32_t[]> trisIndex;   // vertex index array
    std::unique_ptr<Vec3f[]> P;              // triangles vertex position
    std::unique_ptr<Vec2f[]> texCoordinates; // triangles texture coordinates
    std::unique_ptr<Vec3f[]> N;              // triangles vertex normals
    


private:
    std::string type;
    Material* material;

};




} //namespace rt




#endif /* TRIMESH_H_ */
