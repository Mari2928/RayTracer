/*
 * Scene.cpp
 *
 */
#include "Scene.h"
#include <iostream>
#include <vector>
#include <miniply/miniply.h>
using namespace std;




namespace rt{

    //Scene::~Scene() {};

/**
 * Parses json scene object to generate scene to render
 *
 * @param scenespecs the json scene specificatioon
 */
void Scene::createScene(Value& scenespecs){

	//----------parse json object to populate scene-----------

    Value& shapes = scenespecs["shapes"];   
    

    // Retrieve shapes and push back them    
    for (SizeType i = 0; i < shapes.Size(); i++) {
        Shape* shape;
        Material* material;
        std::string type = shapes[i].GetObject()["type"].GetString();

        // create sphere   
        if (type == std::string("sphere")) {
            float radius = shapes[i].GetObject()["radius"].GetFloat();
            Vec3f center(0.0, 0.0, 0.0); 
            auto arr = shapes[i].GetObject()["center"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                center.x = arr[0].GetFloat();
                center.y = arr[1].GetFloat();
                center.z = arr[2].GetFloat();
            }
            
            material = new Material(shapes[i]["material"], 1, std::string("none"));

            shape = new Sphere(center, radius, type, material);
        }
        // create plane 
        else if (type == std::string("plane")) {
            std::string id = shapes[i].GetObject()["id"].GetString();

            Vec3f v0(0.0, 0.0, 0.0);
            auto arr = shapes[i].GetObject()["v0"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v0.x = arr[0].GetFloat();
                v0.y = arr[1].GetFloat();
                v0.z = arr[2].GetFloat();
            }
            Vec3f v1(0.0, 0.0, 0.0);
            arr = shapes[i].GetObject()["v1"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v1.x = arr[0].GetFloat();
                v1.y = arr[1].GetFloat();
                v1.z = arr[2].GetFloat();
            }
            Vec3f v2(0.0, 0.0, 0.0);
            arr = shapes[i].GetObject()["v2"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v2.x = arr[0].GetFloat();
                v2.y = arr[1].GetFloat();
                v2.z = arr[2].GetFloat();
            }
            Vec3f v3(0.0, 0.0, 0.0);
            arr = shapes[i].GetObject()["v3"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v3.x = arr[0].GetFloat();
                v3.y = arr[1].GetFloat();
                v3.z = arr[2].GetFloat();
            }
            material = new Material(shapes[i]["material"], 2, id);
            shape = new Plane(v0, v1, v2, v3, id, material);
            
        }
        // create triangle 
        else if (type == std::string("triangle")) {
            std::string id = shapes[i].GetObject()["id"].GetString();

            Vec3f v0(0.0, 0.0, 0.0);
            auto arr = shapes[i].GetObject()["v0"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v0.x = arr[0].GetFloat();
                v0.y = arr[1].GetFloat();
                v0.z = arr[2].GetFloat();
            }
            Vec3f v1(0.0, 0.0, 0.0);
            arr = shapes[i].GetObject()["v1"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v1.x = arr[0].GetFloat();
                v1.y = arr[1].GetFloat();
                v1.z = arr[2].GetFloat();
            }
            Vec3f v2(0.0, 0.0, 0.0);
            arr = shapes[i].GetObject()["v2"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                v2.x = arr[0].GetFloat();
                v2.y = arr[1].GetFloat();
                v2.z = arr[2].GetFloat();
            }
            material = new Material(shapes[i]["material"], 4, id);
            shape = new Triangle(v0, v1, v2, id, material);
        }
        // create triangle mesh
        else if (type == std::string("trimesh")) {
            std::string id = shapes[i].GetObject()["id"].GetString();
            material = new Material(shapes[i]["material"], 4, id);

            // mesh specs
            float scale = shapes[i].GetObject()["scale"].GetFloat();
            float locX = shapes[i].GetObject()["locX"].GetFloat();
            float locY = shapes[i].GetObject()["locY"].GetFloat();
            float locZ = shapes[i].GetObject()["locZ"].GetFloat();
            int n = shapes[i].GetObject()["n"].GetInt();

            //manually create a poly sphere
            for (uint32_t i = 0; i < n; ++i) {
                int divs = 5 + i;
                Shape* mesh = generatePolyShphere(2, divs, scale, locX, locY, locZ, material);
                this->shapes.push_back(mesh);
            }

            //shape = loadMesh("../meshes/cow.geo", material);
            //if (shape == nullptr) continue;
        }

        this->shapes.push_back(shape);
    }       


    Value& lightsources = scenespecs["lightsources"];

    for (SizeType i = 0; i < lightsources.Size(); i++) {
        Vec3f lightPos(0.0, 0.0, 0.0);

        auto arr = lightsources[i].GetObject()["position"].GetArray();
        for (int i = 0; i < arr.Size(); i++) {
            lightPos.x = arr[0].GetFloat();
            lightPos.y = arr[1].GetFloat();
            lightPos.z = arr[2].GetFloat();
        }
        PointLight* light = new PointLight(lightPos);
        this->lightSources.push_back(light);
    }
}

/**
 * Load trimesh image file to create trimesh instance
 * Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-polygon-mesh/
 * @param file the image specificatioon as a external file
 * @param material the trimesh material
 *
 * @return trimesh instance
 */
TriMesh* Scene::loadMesh(const char* file, Material* material)
{
    std::ofstream ifs;
    try {
        ifs.open(file);
        if (ifs.fail()) throw;
        std::stringstream ss;

        ss << ifs.rdbuf();
        uint32_t numFaces;
        ss >> numFaces;
        std::unique_ptr<uint32_t[]> faceIndex(new uint32_t[numFaces]);
        uint32_t vertsIndexArraySize = 0;

        // reading face index array
        for (uint32_t i = 0; i < numFaces; ++i) {
            ss >> faceIndex[i];
            vertsIndexArraySize += faceIndex[i];
        }
        std::unique_ptr<uint32_t[]> vertsIndex(new uint32_t[vertsIndexArraySize]);
        uint32_t vertsArraySize = 0;

        // reading vertex index array
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
            ss >> vertsIndex[i];
            if (vertsIndex[i] > vertsArraySize) vertsArraySize = vertsIndex[i];
        }
        vertsArraySize += 1;

        // reading vertices
        std::unique_ptr<Vec3f[]> verts(new Vec3f[vertsArraySize]);
        for (uint32_t i = 0; i < vertsArraySize; ++i) {
            ss >> verts[i].x >> verts[i].y >> verts[i].z;
        }
        // reading normals
        std::unique_ptr<Vec3f[]> normals(new Vec3f[vertsIndexArraySize]);
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
            ss >> normals[i].x >> normals[i].y >> normals[i].z;
        }
        // reading st coordinates
        std::unique_ptr<Vec2f[]> st(new Vec2f[vertsIndexArraySize]);
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
            ss >> st[i].x >> st[i].y;
        }

        return new TriMesh(numFaces, faceIndex, vertsIndex, verts, normals, st, material);
    }
    catch (...) {
        ifs.close();
    }
    ifs.close();

    return nullptr;
}

/**
* Generate polygon sphere manually without a ply file to test casting and texture mapping.
* Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-polygon-mesh/ray-tracing-polygon-mesh-part-1.html,
*
* @param rad the radian angle
*
* @return divs the number of division
*
*/
TriMesh* Scene::generatePolyShphere(float rad, uint32_t divs, float scale, int locX, int locY, int locZ, Material* material)
{
    // generate points                                                                                                                                                                                      
    uint32_t numVertices = (divs - 1) * divs + 2;
    std::unique_ptr<Vec3f[]> P(new Vec3f[numVertices]);
    std::unique_ptr<Vec3f[]> N(new Vec3f[numVertices]);
    std::unique_ptr<Vec2f[]> st(new Vec2f[numVertices]);

    float u = -M_PI_2;
    float v = -M_PI;
    float du = M_PI / divs;
    float dv = 2 * M_PI / divs;

    P[0] = N[0] = Vec3f(0 +locX, -rad * scale +locY, 0 + locZ); // control location and size here
    uint32_t k = 1;
    for (uint32_t i = 0; i < divs - 1; i++) {
        u += du;
        v = -M_PI;
        for (uint32_t j = 0; j < divs; j++) {
            float x = rad * cos(u) * cos(v);
            float y = rad * sin(u);
            float z = rad * cos(u) * sin(v);

            // control location and size here
            P[k] = N[k] = Vec3f(x * scale + locX, y * scale + locY, z * scale + locZ); 
            st[k].x =( u / M_PI + 0.5)*scale +locX;
            st[k].y =( v * 0.5 / M_PI + 0.5) *scale +locY;

            v += dv, k++;
        }
    }
    P[k] = N[k] = Vec3f(0 + locX, rad *scale +locY, 0 +locZ); // control location and size here

    uint32_t npolys = divs * divs;
    std::unique_ptr<uint32_t[]> faceIndex(new uint32_t[npolys]);
    std::unique_ptr<uint32_t[]> vertsIndex(new uint32_t[(6 + (divs - 1) * 4) * divs]);

    // create the connectivity lists                                                                                                                                                                        
    uint32_t vid = 1, numV = 0, l = 0;
    k = 0;
    for (uint32_t i = 0; i < divs; i++) {
        for (uint32_t j = 0; j < divs; j++) {
            if (i == 0) {
                faceIndex[k++] = 3;
                vertsIndex[l] = 0;
                vertsIndex[l + 1] = j + vid;
                vertsIndex[l + 2] = (j == (divs - 1)) ? vid : j + vid + 1;
                l += 3;
            }
            else if (i == (divs - 1)) {
                faceIndex[k++] = 3;
                vertsIndex[l] = j + vid + 1 - divs;
                vertsIndex[l + 1] = vid + 1;
                vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
                l += 3;
            }
            else {
                faceIndex[k++] = 4;
                vertsIndex[l] = j + vid + 1 - divs;
                vertsIndex[l + 1] = j + vid + 1;
                vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 : j + vid + 2;
                vertsIndex[l + 3] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
                l += 4;
            }
            numV++;
        }
        vid = numV;
    }
    return new TriMesh(npolys, faceIndex, vertsIndex, P, N, st, material);
}

} //namespace rt
