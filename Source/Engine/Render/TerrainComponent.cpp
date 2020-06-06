
#include <Render/TerrainComponent.h>
#include <IO/JSONDataLoader.h>
#include <IO/ResourceSystem.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>

TerrainComponent::TerrainComponent() {
    m_splat = 0;
    m_numTextures = 0;
    m_textures[0] = m_textures[1] = m_textures[2] = m_textures[3] = 0;
    applyLighting = true;
}

void TerrainComponent::Load(std::string json, vector3 offset) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Load JSON
    JSONDataLoader* loader = new JSONDataLoader();
    loader->Open(json);
    std::vector<DataRecord*> records = loader->GetRecords("terrains");
    
    // Load the heightmap texture
    Texture2D* hmaptex = (Texture2D*)resourceSystem->LoadResource(records[0]->Get("heightmap")->AsString(), "Texture2D");
    unsigned char* data = (unsigned char*)hmaptex->GetData();
    
    // Load splat map
    Variant* splat = records[0]->Get("splatmap");
    if(splat != 0) {
        m_splat = (Texture2D*)resourceSystem->LoadResource(splat->AsString(), "Texture2D");
    }
    
    // Load textures
    m_numTextures = records[0]->Get("textures")->Size();
    for(int i = 0; i < m_numTextures; i++) {
        Variant* textures = records[0]->Get("textures");
        Variant* v = (*textures)[i];
        std::string filename = v->AsString();
        m_textures[i] = (Texture2D*)resourceSystem->LoadResource(filename, "Texture2D");
        
        m_textures[i]->SetTextureFilter(FILTER_LINEAR);
        m_textures[i]->SetTextureFilter(FILTER_MIPMAP);
        m_textures[i]->GenerateMipMaps();
    }
    
    Texture2D* normaltex = 0;
    if(records[0]->Get("normalmap") == 0) {
        normaltex = renderSystem->CreateTexture2D();
        
        // Generate normal map
        int width = hmaptex->GetWidth();
        int channels = hmaptex->GetChannels();
        unsigned char* normalData = (unsigned char*)malloc(width * width * 3);
        for(int y = 0; y < width; y++) {
            for(int x = 0; x < width; x++) {
                vector3 normTopLeft, normTopRight, normBottomLeft, normBottomRight;
                int indexThis, indexLeft, indexRight, indexTop, indexBottom;
                
                indexThis = ((y * width) + x) * channels;
                indexLeft = ((y * width) + (x - 1)) * channels;
                indexRight = ((y * width) + (x + 1)) * channels;
                indexTop = (((y + 1) * width) + x) * channels;
                indexBottom = (((y - 1) * width) + x) * channels;
                
                normTopLeft = normTopRight = normBottomLeft = normBottomRight = vector3(0, 0, 0);
                
                // Top left
                if(x > 0 && y < (width - 1)) {
                    vector3 vec1 = vector3(x - 1, (float)data[indexLeft], y);
                    vector3 vec2 = vector3(x, (float)data[indexThis], y);
                    vector3 vec3 = vector3(x, (float)data[indexTop], y + 1);
                    
                    normTopLeft = glm::cross(vec2 - vec1, vec2 - vec3);
                }
                
                // Top right
                if(x < (width - 1) && y < (width - 1)) {
                    vector3 vec1 = vector3(x + 1, (float)data[indexRight], y);
                    vector3 vec2 = vector3(x, (float)data[indexThis], y);
                    vector3 vec3 = vector3(x, (float)data[indexTop], y + 1);
                    
                    normTopRight = glm::cross(vec2 - vec3, vec2 - vec1);
                }
                
                // Bottom left
                if(x > 0 && y > 0) {
                    vector3 vec1 = vector3(x - 1, (float)data[indexLeft], y);
                    vector3 vec2 = vector3(x, (float)data[indexThis], y);
                    vector3 vec3 = vector3(x, (float)data[indexBottom], y - 1);
                    
                    normBottomLeft = glm::cross(vec2 - vec3, vec2 - vec1);
                }
                
                // Bottom right
                if(x < (width - 1) && y > 0) {
                    vector3 vec1 = vector3(x + 1, (float)data[indexRight], y);
                    vector3 vec2 = vector3(x, (float)data[indexThis], y);
                    vector3 vec3 = vector3(x, (float)data[indexBottom], y - 1);
                    
                    normBottomRight = glm::cross(vec2 - vec1, vec2 - vec3);
                }
                
                vector3 normal = glm::normalize(normTopLeft + normTopRight + normBottomLeft + normBottomRight);
                
                int offset = ((y * width) + x) * 3;
                normalData[offset + 0] = ((normal.x * 0.5f) + 0.5f) * 255.0f;
                normalData[offset + 1] = ((normal.y * 0.5f) + 0.5f) * 255.0f;
                normalData[offset + 2] = ((normal.z * 0.5f) + 0.5f) * 255.0f;
            }
        }
        
        normaltex->SetData(width, width, COLOR_RGB, COLOR_RGB, TEXTURE_TYPE_BYTE, normalData);
        //normaltex->Save("normalmap.bmp");
        free(normalData);
    }
    else {
        normaltex = (Texture2D*)resourceSystem->LoadResource(records[0]->Get("normalmap")->AsString(), "Texture2D");
    }
    
    // Create the "root" terrain quad
    TerrainQuad* quad = new TerrainQuad();
    quad->Load(hmaptex, normaltex, vector2(0), hmaptex->GetWidth());
    
    // Save
    children.push_back(quad);
    transform->SetWorldPosition(offset);
    
    free(data);
    delete hmaptex;
    delete loader;
}

void TerrainComponent::Unload() {
    if(renderable) {
        delete(renderable);
        renderable = 0;
    }
}

void TerrainComponent::Update(Scene* scene) {
    CameraComponent* camera = scene->camera;
    TerrainQuad* quad = dynamic_cast<TerrainQuad*>(children[0]);
    quad->SetLOD(camera->transform->GetWorldPosition());
}
