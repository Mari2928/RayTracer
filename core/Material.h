/*
 * Material.h
 *
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <string>
#include "rapidjson/document.h"
#include "math/geometry.h"


using namespace rapidjson;

namespace rt{

class Material{
public:
	//
	// Constructors
	//
	Material() {};
	Material(Value& material, int type, std::string id);

	//
	// Destructor
	//
	virtual ~Material() {};

	//
	// setter function : sets texture image
	//
	void setTextureMap();

	//
	// mapping function : returns the mapped coordinates from the UV coordinates for texture mapping
	//
	Vec3f mapLookup(Vec2f pos);

	//
	// color function : returns the final color
	//
	Vec3f getColor(Vec2f pos);


	//
	// Getters
	//
	float getKs() const {
		return ks;
	}
	float getKd() const {
		return kd;
	}
	float getKr() const {
		return kr;
	}
	int getSpecularexponent() const {
		return specularexponent;
	}
	Vec3f getDiffusecolor() const {
		return diffusecolor;
	}
	std::string getTPath() const {
		return tPath;
	}
	int getTWidth() const {
		return tWidth;
	}
	int getTHeight() const {
		return tHeight;
	}
	int getType() const {
		return type;
	}
	unsigned char* getTextureImg() const {
		return textureImg;
	}


private:
	float ks;
	float kd;
	float kr;
	int specularexponent;
	Vec3f diffusecolor;
	std::string tPath;
	int tWidth;
	int tHeight;
	int type;

	unsigned char* textureImg;
	int channels;
};


} //namespace rt



#endif /* MATERIAL_H_ */
