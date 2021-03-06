
#include <Render/OpenGL/OpenGLTexture3D.h>
#include <Render/OpenGL/OpenGL.h>
#include <SOIL/SOIL.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

OpenGLTexture3D::~OpenGLTexture3D() {
    if (m_texture) {
        GL_CHECK(glDeleteTextures(1, &m_texture));
        m_texture = 0;
    }
}

void OpenGLTexture3D::Initialize(int width, int height, int format, int internalType, int type) {
    if (m_texture) {
        GL_CHECK(glDeleteTextures(1, &m_texture));
        m_texture = 0;
    }
    
    GL_CHECK(glGenTextures(1, &m_texture));
    
    m_width = width;
    m_height = height;
    m_channels = 0;
    m_dataType = internalType;
    
    switch(type) {
        case GL_DEPTH_COMPONENT:
            m_channels = 1;
            break;
        case GL_RED:
            m_channels = 1;
            break;
        case GL_RGB:
            m_channels = 3;
            break;
        case GL_RGBA:
            m_channels = 4;
            break;
        default:
            break;
    };
    
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
    
    for (unsigned int i = 0; i < 6; ++i) {
        int slot = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        GL_CHECK(glTexImage2D(slot, 0, format, width, height, 0, type, m_dataType, NULL));
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void OpenGLTexture3D::Bind(int slot) {
    m_slot = slot;
    
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
}

void OpenGLTexture3D::Unbind() {
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + m_slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

unsigned int OpenGLTexture3D::GetTarget(int slot) {
    return GL_TEXTURE_CUBE_MAP_POSITIVE_X + slot;
}

void OpenGLTexture3D::Save(std::string filename) {
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + m_slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
    for(int i = 0; i < 6; i++) {
        std::string new_filename = filename.substr(0, filename.length() - 4) + "_" + std::to_string(i) + ".bmp";
        // Dump out normal texture
        int byteSize = m_width * m_height * (m_dataType == GL_FLOAT ? sizeof(float) : 1) * 4;//m_channels;
        //byteSize*=2;
        unsigned char* pixels = (unsigned char*)malloc(byteSize);
        //float* pixels = (float*)malloc(m_width * m_height * sizeof(float) * m_channels);
        GL_CHECK(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_channels == 3 ? GL_RGB : GL_DEPTH_COMPONENT, m_dataType, pixels));
        
        // Convert from float to unsigned char
        unsigned char* data = (unsigned char*)malloc(m_width * m_height * m_channels);
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                int offset = ((y * m_width) + x) * m_channels;
                for(int c = 0; c < m_channels; c++) {
                    if(m_dataType == GL_FLOAT) {
                        float r = ((float*)pixels)[offset + c];
                        data[offset + c] = r * 255.0f;
                    }
                    else {
                        data[offset + c] = (pixels)[offset + c];
                    }
                }
            }
        }
        
        SOIL_save_image(new_filename.c_str(), SOIL_SAVE_TYPE_BMP, m_width, m_height, m_channels, data);
        free(data);
        free(pixels);
    }
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void OpenGLTexture3D::Load(int slot, std::string filename) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    m_dataType = GL_UNSIGNED_BYTE;
    
    if(m_texture == 0) {
        GL_CHECK(glGenTextures(1, &m_texture))
    }
    
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + m_slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
    
    std::string path = resourceSystem->FindResourcePath(filename);
    unsigned char* data = SOIL_load_image(path.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_RGB);
    GL_CHECK(glTexImage2D(slot, 0, GL_RGB, m_width, m_height, 0, GL_RGB, m_dataType, data));
    SOIL_free_image_data(data);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
