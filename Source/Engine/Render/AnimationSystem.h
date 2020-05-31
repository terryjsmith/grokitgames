
#ifndef animationsystem_h
#define animationsystem_h

#include <Core/System.h>
#include <Render/MeshComponent.h>

GIGA_CLASS(Singleton)
class GIGA_API AnimationSystem : public System {
public:
    AnimationSystem() = default;
    ~AnimationSystem() = default;
    
    GIGA_CLASS_NAME("AnimationSystem");
    
    /**
     * Initialize system
     */
    void Initialize();
    
    /**
     * Update (animation components)
     */
    void Update(float delta);
    
    /**
     * Start an animation
     */
    GIGA_FUNCTION() void StartAnimation(MeshComponent* mc, std::string animation, bool loop, float weight = 1.0f);
    
protected:
    /**
     * Update bone hierarchy
     */
    void UpdateBoneHierarchy(AnimationInstance* instance, Mesh* mesh, Node* n, matrix4 parentTransform);
};

#endif
