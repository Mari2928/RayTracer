/*
 * RayTracer.cpp
 *
 */
#include "RayTracer.h"

#include "Shape.h"

#include "cameras/Pinhole.h"
#include "lights/PointLight.h"
#include "materials/BlinnPhong.h"
#include "core/RayHitStructs.h"

#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>
#include<algorithm>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <numeric> 
#include <limits>

namespace rt{

    /**
     * Math function
     *
     * @param v 3D vector
     *
     * @return unit vector
     *
     */
    Vec3f RayTracer::unit_vector(Vec3f v) {
        float mag = 1 / v.length();
        return v * mag;
           
    }

    /**
     * Trace function
     *
     * @param orig ray arigin
     * @param dir ray direction 
     * @param objects shape objects in a vector
     * @param shadowDistance shadow distance to be computed for shadow ray 
     * @param rayType ray type to determine {PRIMARY, SECONDART, SHADOW}
     * @return closest shape object if ray hits, otherwise nullptr
     *
     */
    Shape* RayTracer::trace(const Vec3f& orig, const Vec3f& dir, std::vector<Shape*> objects, float shadowDistance, RayType rayType)
    {
        Hit hitShape;   
        Shape* closestObject = nullptr;
        float minDist = std::numeric_limits<float>::max();
        for (uint32_t k = 0; k < objects.size(); ++k) {            
            Ray ray;
            ray.origin = orig;
            ray.direction = dir;
            hitShape = objects.at(k)->intersect(ray); 

            if(hitShape.hittable && hitShape.distance < shadowDistance){ 
                // skip if ray is shadow ray
                if (rayType == SHADOW ) continue;
                if (hitShape.distance < minDist) {
                    minDist = hitShape.distance;
                    closestObject = objects.at(k);
                 }                
            }
        }
        return closestObject;
    }

    /**
     * Ray casting function (baseline)
     *
     * @param orig ray arigin
     * @param dir ray direction
     * @param objects shape objects in a vector
     * @param light light source
     * @param maxDepth the max number of bounce
     * @param depth the current number of bounce
     *
     * @return final color in linear RGB value
     *
     */
    Vec3f RayTracer::castRay( const Vec3f& orig, const Vec3f& dir, const std::vector<Shape*>& objects, LightSource* light, uint32_t maxDepth, uint32_t depth)
    {
        if (depth > maxDepth) {
            return Vec3f(0.01, 0.01, 0.01);
        }
        
        Vec3f hitColor = Vec3f(0.01, 0.01, 0.01); // background color
        Shape* hitObject = trace(orig, dir, objects, std::numeric_limits<float>::max(), PRIMARY);

        // if ray hits an object, compute reflected colors
        if (hitObject != nullptr) {
            Ray ray;
            ray.origin = orig;
            ray.direction = dir;
            Hit hitShape = hitObject->intersect(ray);

            // retrieve object specs and compute prelim settings
            Vec3f hitPoint = hitShape.point;
            Vec3f N = hitShape.normal;    
            Material* material = hitObject->getMaterial();
            hitColor = material->getDiffusecolor();
            //Vec3f lightDir = (hitShape.point - light->position).normalize();
            float bias = 1e-4;                
            Vec3f lightIntensity = light->getLightIntensity(hitShape.point, light->position);                

            // compute shadow and light elements   
            Vec3f lightDir, intensity;
            float shadowDistance;
            light->illuminate(hitPoint, lightDir, intensity, shadowDistance);
            bool isVisible = trace(hitPoint+N*bias , -lightDir, objects, shadowDistance, SHADOW) == nullptr;

            // texture mapping
            if(!material->getTPath().empty()){
                hitColor = material->getColor(hitObject->getUV(hitShape));
            }

            // compute diffuse and specular reflections
            BlinnPhong* blinnPhong;
            hitColor = blinnPhong->getReflectedColor(dir, hitShape, material, lightIntensity, hitColor, lightDir, isVisible);
            

            // add if material needs perfect reflections
            if(hitObject->getMaterial()->getKr() < 1)
            {
                Vec3f reflectionDirection = (dir) - 2 * (dir).dotProduct(N) * N;
                Vec3f reflectionRayOrig = (reflectionDirection.dotProduct(N) < 0) ? hitPoint + N : hitPoint - N;
                hitColor = hitColor + castRay(reflectionRayOrig, reflectionDirection, objects, light, maxDepth, depth + 1) *hitObject->getMaterial()->getKr();
            }
        }
        return hitColor;
    }

    /**
     * Ray casting function (BVH)
     *
     * @param orig ray arigin
     * @param dir ray direction
     * @param world root node of the shape objects nodes
     * @param light light source
     * @param depth the max number of bounce
     *
     * @return final color in linear RGB value
     *
     */
    Vec3f RayTracer::ray_color(const Vec3f& orig, const Vec3f& dir, const Shape* world, LightSource* light, int depth) {
        Hit hitShape;
        Vec3f hitColor = Vec3f(0.01, 0.01, 0.01); // background color

        // exceeded the ray bounce limit, no more light is gathered
        if (depth <= 0)
            return hitColor;

        // if ray hits nothing, return the background color
        Ray ray;
        ray.origin = orig;
        ray.direction = dir;
        if (!world->hit(ray, 0.001, std::numeric_limits<float>::max(), hitShape))
            return Vec3f(0.01, 0.01, 0.01);
        
        // retrieve object specs and compute prelim settings
        Vec3f hitPoint = hitShape.point;
        Material* material = hitShape.material;
        hitColor = material->getDiffusecolor(); // Vec3f(1,1,1);
        Vec3f lightDir = (hitShape.point - light->position).normalize();
        float bias = 1e-4;
        Vec3f N = hitShape.normal;
        Vec3f lightIntensity = light->getLightIntensity(hitPoint, light->position);


        // texture mapping        
        if (!material->getTPath().empty()) {
            hitColor = material->getColor(hitShape.uv);
        }

        // compute diffuse and specular reflections
        BlinnPhong* blinnPhong;
        bool isVisible = false;
        hitColor = blinnPhong->getReflectedColor(dir, hitShape, material, lightIntensity, hitColor, lightDir, isVisible);

        
        //add if material needs perfect reflections
        if (material->getKr() < 1)
        {            
            Vec3f reflectionDirection = (dir)-2 * (dir).dotProduct(N) * N;
            Vec3f reflectionRayOrig = (reflectionDirection.dotProduct(N) < 0) ? hitPoint + N : hitPoint - N;
            hitColor = hitColor + ray_color(reflectionRayOrig, reflectionDirection, world, light, depth-1) * material->getKr();
        }
        
        return hitColor;
    }


/**
 * Performs ray tracing to render a photorealistic scene.
 * Support normal ray tracer (BASELINE) and BVH optimized ray tracer. Note that BVH version does not support TriMesh.
 * Comment out the lines 227, 253, and 256 appropriately to switch the version. 
 *
 * @param camera the camera viewing the scene
 * @param scene the scene to render, including objects and lightsources
 * @param nbounces the number of bounces to consider for raytracing
 *
 * @return a pixel buffer containing pixel values in linear RGB format
 */
Vec3f* RayTracer::render(Camera* camera, Scene* scene, int nbounces){

	Vec3f* pixelbuffer=new Vec3f[camera->getHeight()* camera->getWidth()];
    

	//----------main rendering function to be filled------

    std::vector<Shape*> shapes = scene->getShapes();    

    // set texture map if available
    for (int i = 0; i < shapes.size(); ++i) {
        Shape* shape = shapes.at(i);
        if (!shape->getMaterial()->getTPath().empty()) {
            shape->getMaterial()->setTextureMap();
        }
    }

    // create BVH tree and nodes
    //Shape* BVHShapes = new BVH(shapes, 0, shapes.size(), 0, 0); // <--- comment this out for BVH

    // lightsource
    std::vector<LightSource*> lightSources = scene->getLightSources();
    LightSource* light = lightSources.at(0);
    

    // check if ray hit the shapes    
    for (unsigned i = 0; i < camera->getHeight(); ++i) {
        for (unsigned j = 0; j < camera->getWidth(); ++j) {
            
            float x = float(j) / float(camera->getHeight());
            float y = float(i) / float(camera->getWidth());            
            x = x * 2.0f - 1.0f;
            y = y * 2.0f - 1.0f;  

            Ray ray;
            // cast with camera ray
            //ray = camera->getCameraRay(x, y);

            // cast without camera 
            ray.direction = Vec3f(x, y, -1.0f);
            ray.direction.normalize();
            ray.origin = camera->getPosition();            
                        
            // BASELINE ray tracer: Use example.json to run this code
            Vec3f color = castRay(ray.origin, ray.direction, shapes, light, nbounces, 0); // <--- comment out when use BVH

            // BVH ray tracer: Use example_bvh.json or example_bvh_test.json if run this code. BVH does not support TriMesh
            //Vec3f color = ray_color(ray.origin, ray.direction, BVHShapes, light, nbounces);

            pixelbuffer[camera->getHeight() * i + j] = color *255.0;
        }
    }   

	return pixelbuffer;

}

/**
 * Tonemaps the rendered image (conversion of linear RGB values [0-1] to low dynamic range [0-255]
 *
 * @param pixelbuffer the array of size width*height containing linear RGB pixel values
 *
 * @return the tonemapped image
 */
Vec3f* RayTracer::tonemap(Vec3f* pixelbuffer){

	//---------tonemapping function to be filled--------
    //for (int i = 0; i < pixelbuffer->length(); i++) {
        //pixelbuffer[i].x = pixelbuffer[i].x * 255.0;
        //pixelbuffer[i].y = pixelbuffer[i].y * 255.0;
        //pixelbuffer[i].z = pixelbuffer[i].z * 255.0;
    //}

	return pixelbuffer;

}





} //namespace rt


