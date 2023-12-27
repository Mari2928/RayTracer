/*
 * Camera.cpp
 *
 */
#include <iostream>

#include "Camera.h"
#include "cameras/Pinhole.h"
#include "cameras/ThinLens.h"

namespace rt{

	Camera::~Camera(){};



/**
 * Factory function that returns camera subclass based on camera specifications
 *
 * @param cameraSpecs camera specifications json object
 *
 * @return camera subclass instance
 *
 */
Camera* Camera::createCamera(Value& cameraSpecs){

	//check if cameratype is defined

	if (!cameraSpecs.HasMember("type")){
		std::cerr<<"Camera type not specified"<<std::endl;
		exit(-1);
	}

	std::string cameraType=cameraSpecs["type"].GetString();

	//return camera object based on camera specs
	if (cameraType.compare("pinhole")==0){

		// parse position
		Vec3f position(0.0, 0.0, 0.0);
		auto pos = cameraSpecs["position"].GetArray();
		for (int i = 0; i < pos.Size(); i++) {
			position.x = pos[0].GetFloat();
			position.y = pos[1].GetFloat();
			position.z = pos[2].GetFloat();
		}

		// parse lookat
		Vec3f lookat(0.0, 0.0, 0.0);
		auto la = cameraSpecs["lookat"].GetArray();
		for (int i = 0; i < la.Size(); i++) {
			lookat.x = la[0].GetFloat();
			lookat.y = la[1].GetFloat();
			lookat.z = la[2].GetFloat();
		}

		// parse up
		Vec3f up(0.0, 0.0, 0.0);
		auto up_ = cameraSpecs["up"].GetArray();
		for (int i = 0; i < up_.Size(); i++) {
			up.x = up_[0].GetFloat();
			up.y = up_[1].GetFloat();
			up.z = up_[2].GetFloat();
		}

		return new Pinhole(cameraSpecs["width"].GetInt(),
				cameraSpecs["height"].GetInt(),
				cameraSpecs["fov"].GetInt(), position, lookat, up);

	}else if (cameraType.compare("thinlens")==0){
		return new ThinLens();
	}

	return 0;

}



} //namespace rt
