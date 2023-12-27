/*
 * Material.cpp
 *
 */


#include "core/Material.h"
#include <string.h>
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"


using namespace rapidjson;

namespace rt{    
    
    /**
     * Factory function that returns material subclass based on material specifications
     *
     * @param material material specifications json object
     * @param type material type in integer
     * @param id material id in string
     *
     * @return material subclass instance
     *
     */
    Material::Material(Value& material, int type, std::string id="none") {

        if (material.HasMember("ks")) {
            this->ks = material.GetObject()["ks"].GetFloat();
        }
        if (material.HasMember("kd")) {
            this->kd = material.GetObject()["kd"].GetFloat();
        }           
        if (material.HasMember("kr")) {
            this->kr = material.GetObject()["kr"].GetFloat();
        }
        if (!material.HasMember("kr")) {
            this->kr = 2.0;
        }
        if (material.HasMember("specularexponent")) {
            this->specularexponent = material.GetObject()["specularexponent"].GetInt();
        }
        if (material.HasMember("tPath")) {
            this->tPath = material.GetObject()["tPath"].GetString();
        }
        if (material.HasMember("tWidth")) {
            this->tWidth = material.GetObject()["tWidth"].GetInt();
        }
        if (material.HasMember("tHeight")) {
            this->tHeight = material.GetObject()["tHeight"].GetInt();
        }
        if (material.HasMember("diffusecolor")) {
            Vec3f diffusecolor(0.0, 0.0, 0.0);
            auto arr = material.GetObject()["diffusecolor"].GetArray();
            for (int i = 0; i < arr.Size(); i++) {
                diffusecolor.x = arr[0].GetFloat();
                diffusecolor.y = arr[1].GetFloat();
                diffusecolor.z = arr[2].GetFloat();
            }            
            this->diffusecolor = diffusecolor;
        }       

    };

    /**
     * Setter function that sets the texture image information based on image specifications
     *
     */
    void Material::setTextureMap()
    {       
        int width, height, channels;
        this->textureImg = stbi_load(this->tPath.c_str(), &width, &height, &channels, 0);
        this->channels = channels;
    }

    /**
     * Mapping function
     *
     * @param pos UV coordinates
     *
     * @return texture mapped linear RGB color based on UV coordinates
     *
     */
    Vec3f Material::mapLookup(Vec2f pos)
    {
        Vec3f result;
        
        float u1 = pos.x - (float)floor(pos.x);
        float v1 = pos.y - (float)floor(pos.y);

        // scale u and v to the texture image of width and height
        //int u = u1 * this->tWidth;
        //int v = v1 * this->tHeight;
        u1*= this->tWidth-1;
        v1*= this->tHeight-1;

        unsigned bytePerPixel = this->channels;
        /*unsigned char* pixelOffset = this->textureImg + ((int)u1 + this->tWidth * (int)v1) * bytePerPixel;*/
        unsigned char* pixelOffset = this->textureImg + ((int)u1 + this->tWidth * (int)v1) * bytePerPixel;
        unsigned char r = pixelOffset[0];
        unsigned char g = pixelOffset[1];
        unsigned char b = pixelOffset[2];
        unsigned char a = this->channels >= 4 ? pixelOffset[3] : 0xff;

        result.x = (r > 255) ? 1.0: r / 255.0f;
        result.y = (g > 255) ? 1.0 : g / 255.0f;
        result.z = (b > 255) ? 1.0 : b / 255.0f;

        
        return result;
    }

    /**
     * Getter function
     *
     * @param pos UV coordinates
     *
     * @return final linear RGB color on hitpoint
     *
     */
    Vec3f Material::getColor(Vec2f pos) {
        Vec3f col = this->diffusecolor;
        if (this->textureImg)
        {
            col = mapLookup(pos);
        }
        return col;
    }


} //namespace rt


