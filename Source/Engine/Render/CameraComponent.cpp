
#define GLM_FORCE_RADIANS

#include <Render/CameraComponent.h>
#include <glm/gtc/matrix_transform.hpp>

CameraComponent::CameraComponent() {
    m_fnear = 0.1f;
    m_ffar = 200.0f;
    m_fov = 45.0f;
    m_aspect = 1.3;
    m_ortho = false;
    transform->SetLook(vector3(0, 0, -1));
    transform->SetUp(vector3(0, 1, 0));
    m_customProj = false;
    m_customView = false;
}

matrix4 CameraComponent::GetViewMatrix() {
    if(m_customView) {
        return(m_view);
    }
    
    Transform* transform = this->transform;
    vector3 position = transform->GetWorldPosition();

    vector3 look = transform->GetLook();
    vector3 up = transform->GetUp();

    return(glm::lookAt(position, position + look, up));
}

matrix4 CameraComponent::GetProjectionMatrix() {
    if(m_customProj) {
        return(m_proj);
    }
    
    return(glm::perspective(glm::radians(m_fov), m_aspect, m_fnear, m_ffar));
}

Frustum CameraComponent::CalculateFrustum(float fnear, float ffar) {
    Frustum returnFrustum;

    // Calculate the near and far plane points
    float nearHeight = 2 * tan(m_fov / 2) * fnear;
    float nearWidth = nearHeight * m_aspect;
    float farHeight = 2 * tan(m_fov / 2) * ffar;
    float farWidth = farHeight * m_aspect;

    vector3 look = transform->GetLook();
    vector3 up = transform->GetUp();
    vector3 right = transform->GetRight();

    // And their centers
    vector3 position = transform->GetWorldPosition();
    vector3 nearCenter = position + (look * fnear);
    vector3 farCenter = position + (look * ffar);

    returnFrustum.center = position + (look * (ffar / 2.0f));

    // And the actual coordinates
    returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT] = nearCenter - (up * (nearHeight / 2.0f)) - (right * (nearWidth / 2.0f));
    returnFrustum.fnear[Frustum::COORD_BOTTOMRIGHT] = nearCenter - (up * (nearHeight / 2.0f)) + (right * (nearWidth / 2.0f));
    returnFrustum.fnear[Frustum::COORD_TOPLEFT] = nearCenter + (up * (nearHeight / 2.0f)) - (right * (nearWidth / 2.0f));
    returnFrustum.fnear[Frustum::COORD_TOPRIGHT] = nearCenter + (up * (nearHeight / 2.0f)) + (right * (nearWidth / 2.0f));

    returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT] = farCenter - (up * (farHeight / 2.0f)) - (right * (farWidth / 2.0f));
    returnFrustum.ffar[Frustum::COORD_BOTTOMRIGHT] = farCenter - (up * (farHeight / 2.0f)) + (right * (farWidth / 2.0f));
    returnFrustum.ffar[Frustum::COORD_TOPLEFT] = farCenter + (up * (farHeight / 2.0f)) - (right * (farWidth / 2.0f));
    returnFrustum.ffar[Frustum::COORD_TOPRIGHT] = farCenter + (up * (farHeight / 2.0f)) + (right * (farWidth / 2.0f));

    // Then create our frustum planes
    returnFrustum.planes[Frustum::PLANE_NEAR].CreateFromPoints(returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT], returnFrustum.fnear[Frustum::COORD_BOTTOMRIGHT], returnFrustum.fnear[Frustum::COORD_TOPLEFT]);
    returnFrustum.planes[Frustum::PLANE_FAR].CreateFromPoints(returnFrustum.ffar[Frustum::COORD_TOPLEFT], returnFrustum.ffar[Frustum::COORD_TOPRIGHT], returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT]);
    returnFrustum.planes[Frustum::PLANE_LEFT].CreateFromPoints(returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT], returnFrustum.fnear[Frustum::COORD_TOPLEFT], returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT]);
    returnFrustum.planes[Frustum::PLANE_RIGHT].CreateFromPoints(returnFrustum.ffar[Frustum::COORD_TOPRIGHT], returnFrustum.fnear[Frustum::COORD_TOPRIGHT], returnFrustum.ffar[Frustum::COORD_BOTTOMRIGHT]);
    returnFrustum.planes[Frustum::PLANE_TOP].CreateFromPoints(returnFrustum.ffar[Frustum::COORD_TOPLEFT], returnFrustum.fnear[Frustum::COORD_TOPLEFT], returnFrustum.ffar[Frustum::COORD_TOPRIGHT]);
    returnFrustum.planes[Frustum::PLANE_BOTTOM].CreateFromPoints(returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT], returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT], returnFrustum.fnear[Frustum::COORD_BOTTOMRIGHT]);

    return returnFrustum;
}


Frustum CameraComponent::GetFrustum() {
    Frustum returnFrustum;
    
    float fnear = m_fnear;
    float ffar = m_ffar;

    // Calculate the near and far plane points
    float nearHeight = 2 * tan(m_fov / 2) * fnear;
    float nearWidth = nearHeight * m_aspect;
    float farHeight = 2 * tan(m_fov / 2) * ffar;
    float farWidth = farHeight * m_aspect;

    vector3 look = transform->GetLook();
    vector3 up = transform->GetUp();
    vector3 right = transform->GetRight();

    // And their centers
    vector3 position = transform->GetWorldPosition();
    vector3 nearCenter = position + (look * fnear);
    vector3 farCenter = position + (look * ffar);

    returnFrustum.center = position + (look * (ffar / 2.0f));

    // And the actual coordinates
    returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT] = nearCenter - (up * (nearHeight / 2.0f)) - (right * (nearWidth / 2.0f));
    returnFrustum.fnear[Frustum::COORD_BOTTOMRIGHT] = nearCenter - (up * (nearHeight / 2.0f)) + (right * (nearWidth / 2.0f));
    returnFrustum.fnear[Frustum::COORD_TOPLEFT] = nearCenter + (up * (nearHeight / 2.0f)) - (right * (nearWidth / 2.0f));
    returnFrustum.fnear[Frustum::COORD_TOPRIGHT] = nearCenter + (up * (nearHeight / 2.0f)) + (right * (nearWidth / 2.0f));

    returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT] = farCenter - (up * (farHeight / 2.0f)) - (right * (farWidth / 2.0f));
    returnFrustum.ffar[Frustum::COORD_BOTTOMRIGHT] = farCenter - (up * (farHeight / 2.0f)) + (right * (farWidth / 2.0f));
    returnFrustum.ffar[Frustum::COORD_TOPLEFT] = farCenter + (up * (farHeight / 2.0f)) - (right * (farWidth / 2.0f));
    returnFrustum.ffar[Frustum::COORD_TOPRIGHT] = farCenter + (up * (farHeight / 2.0f)) + (right * (farWidth / 2.0f));

    // Then create our frustum planes
    returnFrustum.planes[Frustum::PLANE_NEAR].CreateFromPoints(returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT], returnFrustum.fnear[Frustum::COORD_BOTTOMRIGHT], returnFrustum.fnear[Frustum::COORD_TOPLEFT]);
    returnFrustum.planes[Frustum::PLANE_FAR].CreateFromPoints(returnFrustum.ffar[Frustum::COORD_TOPLEFT], returnFrustum.ffar[Frustum::COORD_TOPRIGHT], returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT]);
    returnFrustum.planes[Frustum::PLANE_LEFT].CreateFromPoints(returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT], returnFrustum.fnear[Frustum::COORD_TOPLEFT], returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT]);
    returnFrustum.planes[Frustum::PLANE_RIGHT].CreateFromPoints(returnFrustum.ffar[Frustum::COORD_TOPRIGHT], returnFrustum.fnear[Frustum::COORD_TOPRIGHT], returnFrustum.ffar[Frustum::COORD_BOTTOMRIGHT]);
    returnFrustum.planes[Frustum::PLANE_TOP].CreateFromPoints(returnFrustum.ffar[Frustum::COORD_TOPLEFT], returnFrustum.fnear[Frustum::COORD_TOPLEFT], returnFrustum.ffar[Frustum::COORD_TOPRIGHT]);
    returnFrustum.planes[Frustum::PLANE_BOTTOM].CreateFromPoints(returnFrustum.fnear[Frustum::COORD_BOTTOMLEFT], returnFrustum.ffar[Frustum::COORD_BOTTOMLEFT], returnFrustum.fnear[Frustum::COORD_BOTTOMRIGHT]);

    return returnFrustum;
}

void CameraComponent::SetProjectionMatrix(matrix4 proj) {
    m_customProj = true;
    m_proj = proj;
}

void CameraComponent::SetViewMatrix(matrix4 view) {
    m_customView = true;
    m_view = view;
}
