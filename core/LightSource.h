/*
 * LightSource.h
 *
 */

#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_

#include "math/geometry.h"
#include<algorithm>

namespace rt{

class LightSource{

public:

    //
    // Constructors
    //
    LightSource() {};
    LightSource(Vec3f position);

    //
    // Destructor
    //
    virtual ~LightSource() {}

    //
    // LightSource abstract methods (to be implemented by subclasses)
    //
    virtual Vec3f getLightIntensity(Vec3f hitPoint, Vec3f lightDirection)const = 0;
    virtual void illuminate(const Vec3f& P, Vec3f&, Vec3f&, float&) const = 0;
    
    //
    // light source members
    //
    Vec3f position;
    Vec3f direction;
    Vec3f color;
    float intensity;


private:

};

} //namespace rt



#endif /* LIGHTSOURCE_H_ */
