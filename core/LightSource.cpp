/*
 * LightSource.cpp
 *
 */
#include "LightSource.h"

namespace rt{

    /**
     * Factory function that returns light source subclass based on light specifications
     *
     * @param position light specifications json object
     *
     * @return light source subclass instance
     *
     */
	LightSource::LightSource(Vec3f position) {
		position[2] = -position[2]; // flip the light 
		this->position = position;
		this->direction = position.normalize();
		this->color = Vec3f(0,0,0);
		this->intensity = 1.0f;
	}




} //namespace rt

