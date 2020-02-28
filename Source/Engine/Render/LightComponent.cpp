
#include <Render/LightComponent.h>

LightComponent::LightComponent() {
    m_color = vector3(1.0f, 1.0f, 1.0f);
    m_attenuation = 10.0f;
    m_passes = 1;
    m_depthPass = 0;
    m_cameras = 0;
    m_depthTextures = 0;
}

LightComponent::~LightComponent() {
    if(m_depthTextures) {
        free(m_depthTextures);
        m_depthTextures = 0;
    }
    
    if(m_cameras) {
        free(m_cameras);
        m_cameras = 0;
    }
}
