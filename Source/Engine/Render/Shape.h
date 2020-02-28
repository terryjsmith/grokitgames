
#ifndef shape_h
#define shape_h

#include <Render/MeshComponent.h>
#include <Render/VertexFormat.h>

class GIGA_API Shape : public MeshComponent {
public:
    Shape() = default;
    ~Shape() = default;
    
    GIGA_CLASS_NAME("Shape");
    
    void SetVertices(VertexFormat* fmt, float* data, int count);
};

#endif
