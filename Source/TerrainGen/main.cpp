
#include <iostream>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <algorithm>

int main(int argc, const char * argv[]) {
    std::string baseHeightmap;
    std::string leftHeightmap;
    std::string rightHeightmap;
    std::string topHeightmap;
    std::string bottomHeightmap;
    int textureWidth;
    unsigned char* splatmap = 0;
    
    std::cout << "Enter the filename of your base heightmap (blank for new): ";
    std::cin >> baseHeightmap;
    
    if(baseHeightmap.empty() == true) {
        std::cout << "Enter the width/height for the heightmap: ";
        std::cin >> textureWidth;
    }
    
    std::cout << "Enter the left heightmap: ";
    std::cin >> leftHeightmap;
    
    std::cout << "Enter the right heightmap: ";
    std::cin >> rightHeightmap;
    
    std::cout << "Enter the top heightmap: ";
    std::cin >> topHeightmap;
    
    std::cout << "Enter the bottom heightmap: ";
    std::cin >> bottomHeightmap;
    
    // Our heightmap data
    unsigned char* hmap_data;
    
    // If we have a base heightmap, load it
    if(baseHeightmap.empty() == false) {
        std::cout << "Loading heightmap from file... ";
        int width, height, channels;
        hmap_data = SOIL_load_image(baseHeightmap.c_str(), &width, &height, &channels, SOIL_LOAD_L);
        textureWidth = width;
        std::cout << "complete" << std::endl;
    }
    else {
        // Otherwise, generate a new one
        std::cout << "Generating new heightmap... ";
        hmap_data = (unsigned char*)malloc(textureWidth * textureWidth);
        
        std::cout << "complete" << std::endl;
    }
    
    // Smoothing?
    int smoothingPasses;
    std::cout << "Enter smoothing passes (1 - 50): ";
    std::cin >> smoothingPasses;
    
    for(int i = 0; i < smoothingPasses; i++) {
        unsigned char* new_data = (unsigned char*)malloc(textureWidth * textureWidth);
        
        std::cout << "Smoothing pass " << i << "... ";
        for(int y = 0; y < textureWidth; y++) {
            for(int x = 0; x < textureWidth; x++) {
                int offset = (y * textureWidth) + x;
                int height = hmap_data[offset];
                int points = 1;
                
                if(x > 0) {
                    offset = (y * textureWidth) + x - 1;
                    height += hmap_data[offset];
                    points++;
                }
                
                if(x < (textureWidth - 1)) {
                    offset = (y * textureWidth) + x + 1;
                    height += hmap_data[offset];
                    points++;
                }
                
                if(y > 0) {
                    offset = ((y - 1) * textureWidth) + x;
                    height += hmap_data[offset];
                    points++;
                }
                
                if(y < (textureWidth - 1)) {
                    offset = ((y + 1) * textureWidth) + x;
                    height += hmap_data[offset];
                    points++;
                }
                
                offset = (y * textureWidth) + x;
                height = height / points;
                new_data[offset] = height;
            }
        }
        
        free(hmap_data);
        hmap_data = new_data;
        std::cout << "complete." << std::endl;
    }
    
    // Save image
    std::string outputHeightmap;
    std::cout << "New heightmap filename prefix (no file ext): ";
    std::cin >> outputHeightmap;
    
    std::cout << "Saving heightmap... ";
    std::string hmapFilename = outputHeightmap + ".bmp";
    SOIL_save_image(hmapFilename.c_str(), SOIL_SAVE_TYPE_BMP, textureWidth, textureWidth, 1, hmap_data);
    std::cout << "complete" << std::endl;
    
    splatmap = (unsigned char*)malloc(textureWidth * textureWidth * 3);
    memset(splatmap, 0, textureWidth * textureWidth * 3);
    
    // Start with base
    std::cout << "Generating splat map... ";
    for(int y = 0; y < textureWidth; y++) {
        for(int x = 0; x < textureWidth; x++) {
            int offset = ((y * textureWidth) + x) * 3;
            splatmap[offset] = 255;
            
            if(y < (textureWidth - 1)) {
                offset = (y * textureWidth) + x;
                int height1 = hmap_data[offset];
                
                offset = ((y + 1) * textureWidth) + x;
                int height2 = hmap_data[offset];
                
                if(std::abs(height1 - height2) >= 3) {
                    float diff = std::abs(height2 - height1) / 5.0f;
                    diff = std::max(diff, 1.0f);
                    
                    splatmap[offset * 3] = (1.0f - diff) * 255;
                    splatmap[offset * 3 + 1] = diff * 255;
                }
            }
            
            if(y > 0) {
                offset = (y * textureWidth) + x;
                int height1 = hmap_data[offset];
                
                offset = ((y - 1) * textureWidth) + x;
                int height2 = hmap_data[offset];
                
                if(std::abs(height1 - height2) >= 3) {
                    float diff = std::abs(height2 - height1) / 5.0f;
                    diff = std::max(diff, 1.0f);
                    
                    splatmap[offset * 3] = (1.0f - diff) * 255;
                    splatmap[offset * 3 + 1] = diff * 255;
                }
            }
            
            if(x > 0) {
                offset = (y * textureWidth) + x;
                int height1 = hmap_data[offset];
                
                offset = (y * textureWidth) + (x - 1);
                int height2 = hmap_data[offset];
                
                if(std::abs(height1 - height2) >= 3) {
                    float diff = std::abs(height2 - height1) / 5.0f;
                    diff = std::max(diff, 1.0f);
                    
                    splatmap[offset * 3] = (1.0f - diff) * 255;
                    splatmap[offset * 3 + 1] = diff * 255;
                }
            }
            
            if(x < (textureWidth - 1)) {
                offset = (y * textureWidth) + x;
                int height1 = hmap_data[offset];
                
                offset = (y * textureWidth) + (x + 1);
                int height2 = hmap_data[offset];
                
                if(std::abs(height1 - height2) >= 3) {
                    float diff = std::abs(height2 - height1) / 5.0f;
                    diff = std::max(diff, 1.0f);
                    
                    splatmap[offset * 3] = (1.0f - diff) * 255;
                    splatmap[offset * 3 + 1] = diff * 255;
                }
            }
        }
    }
    std::cout << "complete." << std::endl;
    
    std::cout << "Saving splat map... ";
    std::string splatmapFilename = outputHeightmap + "_splat.bmp";
    SOIL_save_image(splatmapFilename.c_str(), SOIL_SAVE_TYPE_BMP, textureWidth - 1, textureWidth - 1, 3, splatmap);
    std::cout << "complete" << std::endl;
    
    std::cout << "Generating normals... ";
    float* normals = (float*)malloc(textureWidth * textureWidth * 3 * sizeof(float));
    
    for(int y = 0; y < textureWidth; y++) {
        for(int x = 0; x < textureWidth; x++) {
            int offset = (y * textureWidth) + x;
            
            int left, right, up, down;
            left = right = up = down = hmap_data[offset];
            
            if(x > 0) {
                offset = (y * textureWidth) + (x - 1);
                left = hmap_data[offset];
            }
            
            if(x < (textureWidth - 1)) {
                offset = (y * textureWidth) + (x + 1);
                right = hmap_data[offset];
            }
            
            if(y > 0) {
                offset = ((y - 1) * textureWidth) + x;
                down = hmap_data[offset];
            }
            
            if(y < (textureWidth - 1)) {
                offset = ((y + 1) * textureWidth) + x;
                up = hmap_data[offset];
            }
            
            glm::vec3 normal = glm::normalize(glm::vec3(left - right, 2.0f, down - up));
            offset = ((y * textureWidth) + x) * 3;
            
            normals[offset + 0] = normal.x;
            normals[offset + 1] = normal.y;
            normals[offset + 2] = normal.z;
        }
    }
    std::cout << "complete." << std::endl;
    
    // Smooth normals over faces
    unsigned char* normalmap = (unsigned char*)malloc(textureWidth * textureWidth * 3);
    std::cout << "Smoothing normals... ";
    for(int y = 0; y < textureWidth; y++) {
        for(int x = 0; x < textureWidth; x++) {
            int offset = ((y * textureWidth) + x) * 3;
            glm::vec3 normal = glm::vec3(normals[offset + 0], normals[offset + 1], normals[offset + 2]);
            
            /*if(x > 0) {
                offset = ((y * textureWidth) + (x - 1)) * 3;
                glm::vec3 normal2 = glm::vec3(normals[offset + 0], normals[offset + 1], normals[offset + 2]);
                normal = normal + normal2;
            }
            
            if(x < (textureWidth - 1)) {
                offset = ((y * textureWidth) + (x + 1)) * 3;
                glm::vec3 normal2 = glm::vec3(normals[offset + 0], normals[offset + 1], normals[offset + 2]);
                normal = normal + normal2;
            }
            
            if(y > 0) {
                offset = (((y - 1) * textureWidth) + x) * 3;
                glm::vec3 normal2 = glm::vec3(normals[offset + 0], normals[offset + 1], normals[offset + 2]);
                normal = normal + normal2;
            }
            
            if(y < (textureWidth - 1)) {
                offset = (((y + 1) * textureWidth) + x) * 3;
                glm::vec3 normal2 = glm::vec3(normals[offset + 0], normals[offset + 1], normals[offset + 2]);
                normal = normal + normal2;
            }*/
            
            normal = glm::normalize(normal);
            
            offset = ((y * textureWidth) + x) * 3;
            normalmap[offset + 0] = (((normal.x * 0.5f) + 0.5f) * 255.0f);
            normalmap[offset + 1] = (((normal.y * 0.5f) + 0.5f) * 255.0f);
            normalmap[offset + 2] = (((normal.z * 0.5f) + 0.5f) * 255.0f);
        }
    }
    std::cout << "complete." << std::endl;
    
    std::cout << "Saving normal map... ";
    std::string normalsFilename = outputHeightmap + "_normals.bmp";
    SOIL_save_image(normalsFilename.c_str(), SOIL_SAVE_TYPE_BMP, textureWidth - 1, textureWidth - 1, 3, normalmap);
    std::cout << "complete" << std::endl;
    
    return(0);
}
