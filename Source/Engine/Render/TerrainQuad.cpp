
#include <Render/TerrainQuad.h>
#include <Render/TerrainSystem.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>

TerrainQuad::TerrainQuad() {
    m_data = 0;
    m_currentLOD = 10;
}

TerrainQuad::~TerrainQuad() {
    if(m_data) {
        free(m_data);
        m_data = 0;
    }
}

void TerrainQuad::Unload() {
    if(m_data) {
        free(m_data);
        m_data = 0;
    }
    
    if(children.size()) {
        for(int i = 0; i < 4; i++) {
            if(children[i]) {
                TerrainQuad* quad = dynamic_cast<TerrainQuad*>(children[i]);
                quad->Unload();
            }
        }
        
        children.clear();
    }
    
    if(vertexBuffer) {
        delete(vertexBuffer);
        vertexBuffer = 0;
    }
    
    if(indexBuffer) {
        delete indexBuffer;
        indexBuffer = 0;
    }
}

void TerrainQuad::Load(Texture2D* hmap, vector2 offset, vector2 quadDimensions) {
    TerrainSystem* terrainSystem = GetSystem<TerrainSystem>();
    uint32_t maxQuadSize = terrainSystem->GetMaxQuadSize();
    uint32_t imageWidth = hmap->GetWidth();
    
    m_aabb.Create(vector3(offset.x, -256.0f, offset.y), vector3(offset.x + quadDimensions.x, 256.0f, offset.y + quadDimensions.x));
    
    // If we're not smaller than the max quad size, create children
    if(quadDimensions.x > maxQuadSize) {
        uint32_t newWidth = quadDimensions.x / 2.0f;
        
        // Load quad 0
        TerrainQuad* quad0 = new TerrainQuad();
        quad0->Load(hmap, vector2(offset.x, offset.y), vector2(newWidth));
        children.push_back(quad0);
        
        // Load quad 1
        TerrainQuad* quad1 = new TerrainQuad();
        quad1->Load(hmap, vector2(offset.x + newWidth, offset.y), vector2(newWidth));
        children.push_back(quad1);
        
        // Load quad 2
        TerrainQuad* quad2 = new TerrainQuad();
        quad2->Load(hmap, vector2(offset.x, offset.y + newWidth), vector2(newWidth));
        children.push_back(quad2);
        
        // Load quad 3
        TerrainQuad* quad3 = new TerrainQuad();
        quad3->Load(hmap, vector2(offset.x + newWidth, offset.y + newWidth), vector2(newWidth));
        children.push_back(quad3);
        
        return;
    }
    
    // Get terrain scale factor
    float terrainScale = terrainSystem->GetTerrainScale();
    
    // If we get here, load data
    unsigned int actualSize = quadDimensions.x + 1;
    int channels = hmap->GetChannels();
    m_data = (unsigned char*)malloc(actualSize * actualSize);
    unsigned char* data = (unsigned char*)hmap->GetData();
    std::vector<float> heights;
    
    float mmin = 255;
    float mmax = 0;
    
    for(int y = 0; y < actualSize; y++) {
        for(int x = 0; x < actualSize; x++) {
            int ptr = (((int)((offset.y + y) * imageWidth) + ((int)offset.x + x)) * channels);
            unsigned char height = data[ptr];
            
            if(height > 50){
                int i = 0;
                i++;
            }
            
            m_data[(y * actualSize) + x] = height;
            mmin = std::min(mmin, (float)height * terrainScale);
            mmax = std::max(mmax, (float)height * terrainScale);
        }
    }
    
    m_aabb.Create(vector3(offset.x, mmin, offset.y), vector3(offset.x + quadDimensions.x, mmax, offset.y + quadDimensions.x));
    
    // Save
    m_width = quadDimensions.x;
    m_totalWidth = imageWidth;
    m_offset = offset * terrainScale;
}

void TerrainQuad::SetLOD(vector3 cameraPosition) {
    if(children.size()) {
        for(int i = 0; i < 4; i++) {
            TerrainQuad* quad = dynamic_cast<TerrainQuad*>(children[i]);
            quad->SetLOD(cameraPosition);
        }
        
        return;
    }
    
    // Get render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    TerrainSystem* terrainSystem = GetSystem<TerrainSystem>();
    
    // Get terrain scale factor
    float terrainScale = terrainSystem->GetTerrainScale();
    
    // Determine the LOD
    float distance = glm::length(m_aabb.min - cameraPosition);
    
    // If out of distance, set to -1 (don't render)
    if(distance > 100.0f) {
        //m_currentLOD = -1;
        //return;
    }
    
    uint32_t level = 0;
    
    if(m_currentLOD == level)
        return;
    
    m_currentLOD = level;
    
    uint32_t skip = pow(2, level);
    unsigned int actualSize = (m_width / skip) + 1;
    unsigned int vertexSize = 3 /* position */ + 3 /* normal */ + 4 /* tex coords */;
    unsigned int vertexCount = actualSize * actualSize;
    std::vector<float> vertexData;
    //vertexData.resize(vertexCount * vertexSize);
    //float* data = (float*)malloc(sizeof(float) * vertexCount * vertexSize);
    //memset(data, 0, sizeof(float) * vertexCount * vertexSize);
    
    // Position data first
    float mmax = 0.0f;
    for(int z = 0; z < actualSize; z++) {
        for(int x = 0; x < actualSize; x++) {
            vertexData.push_back(x * terrainScale);
            float height = (float)m_data[(z * actualSize) + x] * terrainScale;
            mmax = std::max(mmax, height);
            vertexData.push_back(height);
            vertexData.push_back(z * terrainScale);
            
            vertexData.push_back(0.0f);
            vertexData.push_back(0.0f);
            vertexData.push_back(0.0f);
            
            vertexData.push_back(x);
            vertexData.push_back(z);
            
            vertexData.push_back((m_offset.x + x) / actualSize);
            vertexData.push_back((m_offset.y + z) / actualSize);
        }
    }
    
    // Calculate normals
    for(int z = 0; z < actualSize - 1; z++) {
        for(int x = 0; x < actualSize - 1; x++) {
            int one = ((z * actualSize) + x) * vertexSize;
            int two = (((z + 1) * actualSize) + x) * vertexSize;
            int three = ((z * actualSize) + (x + 1)) * vertexSize;
            int four = (((z + 1) * actualSize) + (x + 1)) * vertexSize;

            vector3 first = vector3(vertexData[one + 0], vertexData[one + 1], vertexData[one + 2]);
            vector3 second = vector3(vertexData[two + 0], vertexData[two + 1], vertexData[two + 2]);
            vector3 third = vector3(vertexData[three + 0], vertexData[three + 1], vertexData[three + 2]);
            vector3 fourth = vector3(vertexData[four + 0], vertexData[four + 1], vertexData[four + 2]);
            
            vector3 avg1 = vector3(vertexData[one + 3], vertexData[one + 4], vertexData[one + 5]);
            vector3 avg2 = vector3(vertexData[two + 3], vertexData[two + 4], vertexData[two + 5]);
            vector3 avg3 = vector3(vertexData[three + 3], vertexData[three + 4], vertexData[three + 5]);
            vector3 avg4 = vector3(vertexData[four + 3], vertexData[four + 4], vertexData[four + 5]);
            
            // Bottom left
            vector3 vec1 = first - second;
            vector3 vec2 = first - third;
            
            vector3 normal = glm::normalize(glm::cross(vec1, vec2));
            avg1 += normal;
            avg2 += normal;
            avg3 += normal;
            
            // Top right
            vec1 = fourth - third;
            vec2 = fourth - second;
            
            normal = glm::normalize(glm::cross(vec1, vec2));
            
            avg2 += normal;
            avg3 += normal;
            avg4 += normal;
            
            avg1 = glm::normalize(avg1);
            avg2 = glm::normalize(avg2);
            avg3 = glm::normalize(avg3);
            avg4 = glm::normalize(avg4);
            
            vertexData[one + 3] = avg1.x;
            vertexData[one + 4] = avg1.y;
            vertexData[one + 5] = avg1.z;
            
            vertexData[two + 3] = avg2.x;
            vertexData[two + 4] = avg2.y;
            vertexData[two + 5] = avg2.z;
            
            vertexData[three + 3] = avg3.x;
            vertexData[three + 4] = avg3.y;
            vertexData[three + 5] = avg3.z;
            
            vertexData[four + 3] = avg4.x;
            vertexData[four + 4] = avg4.y;
            vertexData[four + 5] = avg4.z;
        }
    }
    
    if(vertexBuffer == 0) {
        vertexBuffer = renderSystem->CreateVertexBuffer();
        
        VertexFormat* vertexFormat = renderSystem->CreateVertexFormat();
        vertexFormat->Initialize();
        
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_NORMAL, 3, 3);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD0, 2, 6);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD1, 2, 8);
        
        vertexBuffer->Create(vertexFormat, vertexCount, vertexData.data(), true);
    }
    else {
        vertexBuffer->SetData(vertexCount, vertexData.data());
    }
    
    //free(data);
    
    unsigned int indexCount = actualSize * actualSize * 6;
    std::vector<unsigned int> indexData;
    //indexData.resize(indexCount);
    
    //unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * indexCount);
    for(int z = 0; z < m_width; z++) {
        for(int x = 0; x < m_width; x++) {
            indexData.push_back((z * actualSize) + x);
            indexData.push_back(((z + 1) * actualSize) + x);
            indexData.push_back((z * actualSize) + (x+ 1));
            
            // Second triangle
            indexData.push_back((z * actualSize) + (x+ 1));
            indexData.push_back(((z + 1) * actualSize) + x);
            indexData.push_back(((z + 1) * actualSize) + (x + 1));
        }
    }
    
    if(indexBuffer == 0) {
        indexBuffer = renderSystem->CreateIndexBuffer();
        indexBuffer->Create(indexCount, indexData.data());
    }
    else {
        indexBuffer->SetData(indexCount, indexData.data());
    }
    
    // Save
    m_currentLOD = level;
}
