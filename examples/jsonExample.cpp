/*
 * jsonExample.cpp
 *
 * How-to example for parsing and manipulating json input
 */

#include <iostream>
#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "core/Material.h"

#define _USE_MATH_DEFINES  // for MSVC, for M_PI
#include <math.h>

using namespace rapidjson;


int main(int argc, char* argv[]){

	std::string jsonfile="../examples/example.json";

	//parse json input file
	std::ifstream ifs(jsonfile);
	IStreamWrapper is(ifs);
	Document d;
	d.ParseStream(is);

	//variable d contains the top json object
	std::cout<<"Variable d contains json object:"<<(d.IsObject()?"true":"false")<<std::endl;
	std::cout<<"d top level object contains "<<d.MemberCount()<<" members:"<<std::endl;

	//iterating over members
	for (Value::ConstMemberIterator itr = d.MemberBegin();itr != d.MemberEnd(); ++itr)
	    printf("%s\n", itr->name.GetString());

	//querying members
	std::cout<<"d has member 'scene':"<<(d.HasMember("scene")?"true":"false")<<std::endl;

	//retieve member
	Value& scene=d["scene"];

	std::cout<<"scene has member 'shapes':"<<(scene.HasMember("shapes")?"true":"false")<<std::endl;

	
	Value& shapes=scene["shapes"];

	
	//Material* material = new Material(shapes[0]["material"], std::string("sphere"), std::string("none"));

	std::cout << "material has member diffusecolor?:" << (shapes[0]["material"].HasMember("diffusecolor") ? "true" : "false") << std::endl;

	//iterate through arrays
	std::cout<<"'shapes' is array:"<<(shapes.IsArray()?"true":"false")<<std::endl;
	std::cout<<"'shapes' contains "<<shapes.Size()<<" elements:"<<std::endl;
	for (SizeType i = 0; i < shapes.Size(); i++) {// Retrieve values through GetXXX() functions
		printf("shapes[%d] = %s radius: %f \n", i,
			shapes[i].GetObject()["type"].GetString(),
			shapes[i].GetObject()["radius"].GetFloat());

		std::cout << "material has member 'kr':" << (shapes[i]["material"].HasMember("kr") ? "true" : "false") << std::endl;
	}
	Vec3f color(0.0,0.0,0.0);
	auto arr = shapes[0]["material"].GetObject()["diffusecolor"].GetArray();
	for (int i = 0; i < arr.Size(); i++) {
		color.x = arr[0].GetFloat();
		color.y = arr[1].GetFloat();
		color.z = arr[2].GetFloat();
	}

	auto theta = 35 * M_PI / 180;
	auto h = tan(theta / 2);

	float focalLength = (800 / (2 * tan(theta / 2)))/800.0;

	std::cout << "normalized position:" << Vec3f(0.0, -3.5, 2.0).normalize() << std::endl;




}
