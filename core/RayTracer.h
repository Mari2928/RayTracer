/*
 * RayTracer.h
 *
 */
#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "math/geometry.h"
#include "core/Camera.h"
#include "core/Scene.h"
#include "Material.h"
#include "lights/PointLight.h"
#include "shapes/Sphere.h"
#include "shapes/Triangle.h"
#include "shapes/TriMesh.h"

namespace rt{



/*
 * Raytracer class declaration
 */
class RayTracer {
    

public:

    //
    // setter function : sets texture image
    //
	RayTracer();

    //
    // math function : returns the unit vector
    //
    static Vec3f unit_vector(Vec3f v);

    //
    // render function : returns the image buffer
    //
	static Vec3f* render(Camera* camera, Scene* scene, int nbounces);

    //
    // tonemap function : returns the tonemapped image buffer
    //
	static Vec3f* tonemap(Vec3f* pixelbuffer);

    //
    // ray casting function (baseline) : returns the final color
    //
    static Vec3f castRay(const Vec3f& orig, const Vec3f& dir, const std::vector<Shape*>& objects, LightSource* light, uint32_t maxDepth, uint32_t depth);
    
    //
    // trace function : returns the closest hit shape
    //
    static Shape* trace(const Vec3f& orig, const Vec3f& dir, std::vector<Shape*> objects, float shadowDistance, RayType rayType);
  
    //
    // ray casting function (BVH) : returns the final color
    //
    static Vec3f ray_color(const Vec3f& orig, const Vec3f& dir, const Shape* world, LightSource* light, int depth);
        

private:


};




} //namespace rt



#endif /* RAYTRACER_H_ */

