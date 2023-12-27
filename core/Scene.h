/*
 * Scene.h
 *
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "rapidjson/document.h"

//#include "miniply/miniply.h"
//#pragma comment("miniply/miniply.h", "Ws2_32.lib") 

#include "core/LightSource.h"
#include "core/Shape.h"
#include "core/Material.h"
#include "shapes/TriMesh.h"
#include "shapes/Triangle.h"
#include "shapes/Plane.h"
#include "shapes/Sphere.h"
#include "lights/PointLight.h"

using namespace rapidjson;

namespace rt{

class Scene {
public:
	//
	// Constructor
	//
	Scene(){};

	//
	// Destructor
	//
	// virtual ~Scene();

	//
	// factory function : create scene with shapes
	//
	void createScene(Value& scenespecs);

	//
	// load function : returns the trimesh instance based on image specifications in a file
	//
	TriMesh* loadMesh(const char* filename, Material* material);

	//
	// factory function : returns the trimesh instance based on the trimesh specifications
	//
	TriMesh* generatePolyShphere(float rad, uint32_t divs, float scale, int locX, int locY, int locZ, Material* material);

	//
	// Getters and Setters
	//
	std::vector<Shape*> getShapes() const {
		return shapes;
	}

	std::vector<LightSource*> getLightSources() const {
		return lightSources;
	}

private:

	std::vector<LightSource*> lightSources;
	std::vector<Shape*> shapes;
};

} //namespace rt



#endif /* SCENE_H_ */
