
#ifndef camera_h
#define camera_h

#include <Core/MathTypes.h>
#include <Render/RenderComponent.h>
#include <Render/Frustum.h>

GIGA_CLASS()
class GIGA_API CameraComponent : public RenderComponent {
public:
    CameraComponent();
    ~CameraComponent() = default;

    GIGA_CLASS_NAME("CameraComponent");

    /**
     * Get/set near/far/aspect/FOV
     */
    void SetFar(float ffar) { m_ffar = ffar; }
    float GetFar() { return m_ffar; }
    
    void SetNear(float fnear) { m_fnear = fnear; }
    float GetNear() { return m_fnear; }
    
    void SetAspectRatio(float aspect) { m_aspect = aspect; }
    float GetAspectRatio() { return m_aspect; }
    
    void SetFOV(float fov) { m_fov = fov; }
    float GetFOV() { return m_fov; }
    
    bool GetOrthographic() { return m_ortho; }
    void SetOrthographic(bool ortho) { m_ortho = ortho; }
    
    /**
     * Get camera matrices
     */
    matrix4 GetViewMatrix();
    matrix4 GetProjectionMatrix();

    /**
     * Set custom matrices
     */
    void SetProjectionMatrix(matrix4 proj);
    void SetViewMatrix(matrix4 view);
    
    /**
    * Calculate a frustum with a near and far set of numbers with our aspect ratio and fov
    */
    Frustum CalculateFrustum(float fnear, float ffar);
    Frustum GetFrustum();

protected:
    float m_fnear, m_ffar;
    float m_fov;
    float m_aspect;
    
    matrix4 m_proj;
    bool m_customProj;
    
    matrix4 m_view;
    bool m_customView;
    
    bool m_ortho;
};

#endif

