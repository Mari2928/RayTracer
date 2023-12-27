/*
 * BlinnPhong.cpp
 *
 *
 */

#include "BlinnPhong.h"
#include<algorithm>


namespace rt{

     /**
      * Get Blinn Phong reflected color
      *
      * @param rayDirection ray direction
      * @param hit hitpoint information
      * @param material shape's material
      * @param lightIntensity light intensity at the hitpoint
      * @param color diffusecolor of the shape
      * @param lightDirection light direction
      * @param isVisible true if the shape is not in shadow, false otherwise
      *
      * @return color with reflections and refractions using Blinn-Phong model
      *
      */
    Vec3f BlinnPhong::getReflectedColor(Vec3f rayDirection, Hit hit, Material* material, Vec3f lightIntensity, Vec3f color, Vec3f lightDirection, bool isVisible) {
        float ior = 1.3;
        Vec3f refractionColor = 0, reflectionColor = 0;        

        // add diffuse and specular reflections
        Vec3f diffuse = isVisible * lightIntensity * material->getKd() * color ;
        Vec3f reflectionDirection = reflect(lightDirection, hit.point);
        //Vec3f reflectionDirection = reflect(lightDirection, hit.normal);
        Vec3f specular = pow(rayDirection.dotProduct(reflectionDirection), material->getSpecularexponent()) * material->getKs() * color;
        reflectionColor = diffuse + specular;

        // no kr parameter        
        if (material->getKr() > 1) {
            return reflectionColor;
        }

        // if there is a kr parameter, add perfect refraction 
        Vec3f refractionDirection = refract(lightDirection, hit.normal, ior).normalize();
        specular = pow(rayDirection.dotProduct(refractionDirection), material->getSpecularexponent()) * material->getKs() * color;
        refractionColor = diffuse + specular;

        return reflectionColor * material->getKr() + refractionColor * (1 - material->getKr());      

    }

    /**
     * Get Clamp the value
     */
    float BlinnPhong::clamp(const float& lo, const float& hi, const float& v)
    {
        return std::max(lo, std::min(hi, v));
    }

    /**
     * Compute reflection direction
     *
     * @param I light direction
     * @param N hitpoint normal
     *
     * return reflection direction
     */
    Vec3f BlinnPhong::reflect(Vec3f I, Vec3f N) {
        return I - 2 * I.dotProduct(N) * N;
    }

    /**
     * Compute refraction direction
     *
     * @param I light direction
     * @param N hitpoint normal
     * @param ior scalar to adjust refraction
     *
     * return refraction direction
     */
    Vec3f BlinnPhong::refract(Vec3f I, Vec3f N, float ior) {
        float cosi = clamp(-1, 1, I.dotProduct(N));
        float etai = 1, etat = ior;
        Vec3f n = N;
        if (cosi < 0) { cosi = -cosi; }
        else { std::swap(etai, etat); n = -N; }
        float eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
    }

    

} //namespace rt
