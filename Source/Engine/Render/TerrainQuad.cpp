
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
    
    if(m_normals) {
        free(m_normals);
        m_normals = 0;
    }
}

void TerrainQuad::Unload() {
    if(m_data) {
        free(m_data);
        m_data = 0;
    }
    
    if(m_normals) {
        free(m_normals);
        m_normals = 0;
    }
}

void TerrainQuad::Load(Texture2D* hmap, Texture2D* nmap, vector2 offset, vector2 quadDimensions) {
    TerrainSystem* terrainSystem = GetSystem<TerrainSystem>();
    uint32_t maxQuadSize = terrainSystem->GetMaxQuadSize();
    uint32_t imageWidth = hmap->GetWidth();
    
    m_aabb.Create(vector3(offset.x, -256.0f, offset.y), vector3(offset.x + quadDimensions.x, 256.0f, offset.y + quadDimensions.x));
    
    // If we're not smaller than the max quad size, create children
    if(quadDimensions.x > maxQuadSize) {
        uint32_t newWidth = quadDimensions.x / 2.0f;
        
        // Load quad 0
        TerrainQuad* quad0 = new TerrainQuad();
        quad0->Load(hmap, nmap, vector2(offset.x, offset.y), vector2(newWidth));
        children.push_back(quad0);
        
        // Load quad 1
        TerrainQuad* quad1 = new TerrainQuad();
        quad1->Load(hmap, nmap, vector2(offset.x + newWidth, offset.y), vector2(newWidth));
        children.push_back(quad1);
        
        // Load quad 2
        TerrainQuad* quad2 = new TerrainQuad();
        quad2->Load(hmap, nmap, vector2(offset.x, offset.y + newWidth), vector2(newWidth));
        children.push_back(quad2);
        
        // Load quad 3
        TerrainQuad* quad3 = new TerrainQuad();
        quad3->Load(hmap, nmap, vector2(offset.x + newWidth, offset.y + newWidth), vector2(newWidth));
        children.push_back(quad3);
        
        return;
    }
    
    // Get terrain scale factor
    float terrainScale = terrainSystem->GetTerrainScale();
    
    // If we get here, load data
    unsigned int actualSize = quadDimensions.x + 1;
    int channels = hmap->GetChannels();
    int nchannels = nmap->GetChannels();
    m_data = (unsigned char*)malloc(actualSize * actualSize);
    m_normals = (float*)malloc(actualSize * actualSize * 3 * sizeof(float));
    
    // Get source data
    unsigned char* data = (unsigned char*)hmap->GetData();
    unsigned char* ndata = (unsigned char*)nmap->GetData();
    std::vector<float> heights;
    
    int nmapWidth = nmap->GetWidth();
    
    float mmin = 255;
    float mmax = 0;
    
    for(int y = 0; y < actualSize; y++) {
        for(int x = 0; x < actualSize; x++) {
            int ptr = (((int)((offset.y + y) * imageWidth) + ((int)offset.x + x)) * channels);
            int nptr = (((int)((offset.y + y) * nmapWidth) + ((int)offset.x + x)) * nchannels);
            unsigned char height = data[ptr];
            
            m_data[(y * actualSize) + x] = height;
            vector3 normal = vector3((((float)ndata[nptr + 0] / 255.0f) - 0.5) / 0.5f,
                                     (((float)ndata[nptr + 1] / 255.0f) - 0.5) / 0.5f,
                                     (((float)ndata[nptr + 2] / 255.0f) - 0.5) / 0.5f);
            normal = glm::normalize(normal);
            
            m_normals[(((y * actualSize) + x) * 3) + 0] = normal.x;
            m_normals[(((y * actualSize) + x) * 3) + 1] = normal.y;
            m_normals[(((y * actualSize) + x) * 3) + 2] = normal.z;
            
            mmin = std::min(mmin, (float)height * terrainScale);
            mmax = std::max(mmax, (float)height * terrainScale);
        }
    }
    
    // Save
    m_width = quadDimensions.x;
    m_totalWidth = imageWidth;
    m_offset = offset * terrainScale;
    
    m_aabb.Create(vector3(m_offset.x, mmin, m_offset.y), vector3(m_offset.x + quadDimensions.x * terrainScale, mmax, m_offset.y + quadDimensions.x * terrainScale));
    
    // Set transform
    transform->SetWorldPosition(vector3(m_offset.x, 0, m_offset.y));
}

void TerrainQuad::SetLOD(vector3 cameraPosition) {
    if(children.size()) {
        for(int i = 0; i < 4; i++) {
            TerrainQuad* quad = dynamic_cast<TerrainQuad*>(children[i]);
            quad->SetLOD(cameraPosition);
        }
        
        return;
    }
    
    if(renderable == 0) {
        renderable = new Renderable();
    }
    
    // Get render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    TerrainSystem* terrainSystem = GetSystem<TerrainSystem>();
    
    // Get terrain scale factor
    float terrainScale = terrainSystem->GetTerrainScale();
    
    // Determine the LOD
    float minDistance = FLT_MAX;
    
    for(int i = 0; i < 8; i++) {
        minDistance = std::min(minDistance, glm::length(cameraPosition - m_aabb.points[i]));
    }
    
    // If out of distance, set to -1 (don't render)
    uint32_t level = 0;
    if(minDistance < 30) {
        level = 0;
    }
    if(minDistance < 80) {
        level = 1;
    }
    if(minDistance > 80) {
        level = 2;
    }
    
    //level = 0;
    
    if(m_currentLOD == level)
        return;
    
    m_currentLOD = level;
    
    unsigned int actualSize = m_width + 1;
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
            int offset = (z * actualSize) + x;
            int noffset = ((z * actualSize) + x) * 3;
            float height = (float)m_data[offset] * terrainScale;
            
            // Position
            vertexData.push_back(x * terrainScale);
            vertexData.push_back(height);
            vertexData.push_back(z * terrainScale);
            
            mmax = std::max(mmax, height);
            
            // Normal
            vertexData.push_back((float)m_normals[noffset + 0]);
            vertexData.push_back((float)m_normals[noffset + 1]);
            vertexData.push_back((float)m_normals[noffset + 2]);
            
            // Tex coord 0
            vertexData.push_back(x * terrainScale * 0.5);
            vertexData.push_back(z * terrainScale * 0.5);
            
            // Tex coord 1
            vector2 actualOffset = m_offset / terrainScale;
            vertexData.push_back((actualOffset.x + x) / m_totalWidth);
            vertexData.push_back((actualOffset.y + z) / m_totalWidth);
        }
    }
    
    VertexBuffer* vertexBuffer = renderable->vertexBuffer;
    if(vertexBuffer == 0) {
        vertexBuffer = renderSystem->CreateVertexBuffer();
        
        VertexFormat* vertexFormat = renderSystem->CreateVertexFormat();
        vertexFormat->Initialize();
        
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_NORMAL, 3, 3);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD0, 2, 6);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD1, 2, 8);
        
        vertexBuffer->Create(vertexFormat, vertexCount, vertexData.data(), true);
        renderable->vertexBuffer = vertexBuffer;
    }
    else {
        vertexBuffer->SetData(vertexCount, vertexData.data());
    }
    
    //free(data);
    
    unsigned int indexCount = actualSize * actualSize * 6;
    std::vector<unsigned int> indexData;
    //indexData.resize(indexCount);
    
    //unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * indexCount);
    for(int z = 0; z < (m_width - 1); z += 2) {
        for(int x = 0; x < (m_width - 1); x += 2) {
            int vec1, vec2, vec3;
            vector3 v1, v2, v3, n;
            float d;
            
            /**
             * LOD 0
             * *-----*----*
             * |  \   |  /   |
             * *-----*----*
             * |  /   |  \   |
             * *----*-----*
             */
            
            if(level == 0) {
                // Bottom left quad
                vec1 = (z * actualSize) + x;
                vec2 = ((z + 1) * actualSize) + x;
                vec3 = ((z + 1) * actualSize) + (x + 1);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                vec1 = (z * actualSize) + x;
                vec2 = ((z + 1) * actualSize) + (x + 1);
                vec3 = (z * actualSize) + (x + 1);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                // Top left quad
                vec1 = ((z + 1) * actualSize) + x;
                vec2 = ((z + 2) * actualSize) + x;
                vec3 = ((z + 1) * actualSize) + (x + 1);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                vec1 = ((z + 2) * actualSize) + x;
                vec2 = ((z + 1) * actualSize) + (x + 1);
                vec3 = ((z + 2) * actualSize) + (x + 1);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                // Top right quad
                vec1 = ((z + 1) * actualSize) + (x + 1);
                vec2 = ((z + 2) * actualSize) + (x + 1);
                vec3 = ((z + 2) * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                vec1 = ((z + 1) * actualSize) + (x + 1);
                vec2 = ((z + 2) * actualSize) + (x + 2);
                vec3 = ((z + 1) * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                // Bottom right quad
                vec1 = (z * actualSize) + (x + 1);
                vec2 = ((z + 1) * actualSize) + (x + 1);
                vec3 = (z * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
            
                vec1 = (z * actualSize) + (x + 2);
                vec2 = ((z + 1) * actualSize) + (x + 1);
                vec3 = ((z + 1) * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
            }
            
            /**
            * LOD 1
            * *----------*
            * |  \      /   |
            * |     *      |
            * |  /      \   |
            * *----------*
            */
            
            if(level == 1) {
                // Left tri
                vec1 = (z * actualSize) + x;
                vec2 = ((z + 2) * actualSize) + x;
                vec3 = ((z + 1) * actualSize) + (x + 1);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));

                if(d < 0) {
                    int ff = 0;
                }
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                // Top tri
                vec1 = ((z + 1) * actualSize) + (x + 1);
                vec2 = ((z + 2) * actualSize) + x;
                vec3 = ((z + 2) * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));

                if(d < 0) {
                    int ff = 0;
                }
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                // Right tri
                vec1 = (z * actualSize) + (x + 2);
                vec2 = ((z + 1) * actualSize) + (x + 1);
                vec3 = ((z + 2) * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));

                if(d < 0) {
                    int ff = 0;
                }
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                // Bottom tri
                vec1 = (z * actualSize) + x;
                vec2 = ((z + 1) * actualSize) + (x + 1);
                vec3 = (z * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                if(d < 0) {
                    int ff = 0;
                }
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
            }
            
            /**
            * LOD 2
            * *----------*
            * |  \          |
            * |      \      |
            * |         \   |
            * *----------*
            */
            
            if(level == 2) {
                // Left tri
                vec1 = (z * actualSize) + x;
                vec2 = ((z + 2) * actualSize) + x;
                vec3 = (z * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                if(d < 0) {
                    int ff = 0;
                }
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
                
                
                // Right tri
                vec1 = (z * actualSize) + (x + 2);
                vec2 = ((z + 2) * actualSize) + x;
                vec3 = ((z + 2) * actualSize) + (x + 2);
                v1 = vector3(vertexData[vec1 * vertexSize + 0], vertexData[vec1 * vertexSize + 1], vertexData[vec1 * vertexSize + 2]);
                v2 = vector3(vertexData[vec2 * vertexSize + 0], vertexData[vec2 * vertexSize + 1], vertexData[vec2 * vertexSize + 2]);
                v3 = vector3(vertexData[vec3 * vertexSize + 0], vertexData[vec3 * vertexSize + 1], vertexData[vec3 * vertexSize + 2]);
                n = vector3(vertexData[vec1 * vertexSize + 3], vertexData[vec1 * vertexSize + 4], vertexData[vec1 * vertexSize + 5]);
                
                d = glm::dot(n, glm::cross(v2 - v1, v3 - v1));
                
                if(d < 0) {
                    int ff = 0;
                }
                
                indexData.push_back(vec1);
                indexData.push_back(d > 0 ? vec2 : vec3);
                indexData.push_back(d > 0 ? vec3 : vec2);
            }
        }
    }
    
    IndexBuffer* indexBuffer = renderable->indexBuffer;
    if(indexBuffer == 0) {
        indexBuffer = renderSystem->CreateIndexBuffer();
        indexBuffer->Create(indexData.size(), indexData.data());
        renderable->indexBuffer = indexBuffer;
    }
    else {
        indexBuffer->SetData(indexData.size(), indexData.data());
    }
    
    // Save
    m_currentLOD = level;
}
