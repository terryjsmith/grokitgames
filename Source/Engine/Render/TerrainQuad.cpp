
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

void TerrainQuad::Load(Texture2D* hmap, Texture2D* nmap, vector2 fileOffset, float width) {
    TerrainSystem* terrainSystem = GetSystem<TerrainSystem>();
    uint32_t maxQuadSize = terrainSystem->GetMaxQuadSize();
    uint32_t imageWidth = hmap->GetWidth();
    int nmapWidth = nmap->GetWidth();
    
    unsigned int actualSize = width + 1;
    int channels = hmap->GetChannels();
    int nchannels = nmap->GetChannels();
    
    // Get terrain scale factor
    float terrainScale = terrainSystem->GetTerrainScale();
    
    if(renderable == 0) {
        renderable = new Renderable();
    }
    
    // Get min/max Y
    float minY = FLT_MAX;
    float maxY = -FLT_MAX;
    unsigned char* data = (unsigned char*)hmap->GetData();
    
    for(int z = 0; z < actualSize; z++) {
        for(int x = 0; x < actualSize; x++) {
            int ptr = (((int)((fileOffset.y + z) * imageWidth) + ((int)fileOffset.x + x)) * channels);
            unsigned char height = data[ptr];
            
            minY = std::min(minY, (float)height * terrainScale);
            maxY = std::max(maxY, (float)height * terrainScale);
        }
    }
    
    renderable->aabb = new AABB();
    renderable->aabb->Create(vector3(0.0f), vector3(width * terrainScale, maxY - minY, width * terrainScale));
    
    // If we're not smaller than the max quad size, create children
    if(width > maxQuadSize) {
        uint32_t newWidth = width / 2.0f;
        
        // Load quad 0
        TerrainQuad* quad0 = new TerrainQuad();
        quad0->Load(hmap, nmap, vector2(fileOffset.x, fileOffset.y), newWidth);
        children.push_back(quad0);
        
        // Load quad 1
        TerrainQuad* quad1 = new TerrainQuad();
        quad1->Load(hmap, nmap, vector2(fileOffset.x + newWidth, fileOffset.y), newWidth);
        children.push_back(quad1);
        
        // Load quad 2
        TerrainQuad* quad2 = new TerrainQuad();
        quad2->Load(hmap, nmap, vector2(fileOffset.x, fileOffset.y + newWidth), newWidth);
        children.push_back(quad2);
        
        // Load quad 3
        TerrainQuad* quad3 = new TerrainQuad();
        quad3->Load(hmap, nmap, vector2(fileOffset.x + newWidth, fileOffset.y + newWidth), newWidth);
        children.push_back(quad3);
        
        return;
    }
    
    // If we get here, load data
    m_data = (unsigned char*)malloc(actualSize * actualSize);
    m_normals = (float*)malloc(actualSize * actualSize * 3 * sizeof(float));
    
    // Get source data
    unsigned char* ndata = (unsigned char*)nmap->GetData();
    
    for(int z = 0; z < actualSize; z++) {
        for(int x = 0; x < actualSize; x++) {
            int ptr = (((int)((fileOffset.y + z) * imageWidth) + ((int)fileOffset.x + x)) * channels);
            int nptr = (((int)((fileOffset.y + z) * nmapWidth) + ((int)fileOffset.x + x)) * nchannels);
            unsigned char height = data[ptr];
            
            m_data[(z * actualSize) + x] = height;
            vector3 normal = vector3((((float)ndata[nptr + 0] / 255.0f) - 0.5) / 0.5f,
                                     (((float)ndata[nptr + 1] / 255.0f) - 0.5) / 0.5f,
                                     (((float)ndata[nptr + 2] / 255.0f) - 0.5) / 0.5f);
            normal = glm::normalize(normal);
            
            m_normals[(((z * actualSize) + x) * 3) + 0] = normal.x;
            m_normals[(((z * actualSize) + x) * 3) + 1] = normal.y;
            m_normals[(((z * actualSize) + x) * 3) + 2] = normal.z;
            
        }
    }
    
    // Save
    m_width = width;
    m_totalWidth = imageWidth;
    m_fileOffset = fileOffset;
    
    // Set transform
    m_transform->SetWorldPosition(vector3(fileOffset.x * terrainScale, 0, fileOffset.y * terrainScale));
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
    Sphere* boundingSphere = this->GetBoundingSphere();
    float minDistance = glm::distance(cameraPosition, boundingSphere->center) - boundingSphere->radius;
    
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
    Array<float> vertexData;
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
            vector2 actualOffset = m_fileOffset / terrainScale;
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
        vertexBuffer->SetData(vertexCount, 0, vertexData.data());
    }
    
    //free(data);
    
    unsigned int indexCount = actualSize * actualSize * 6;
    Array<unsigned int> indexData;
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
