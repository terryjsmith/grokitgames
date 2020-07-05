
#ifndef vertexformat_h
#define vertexformat_h

#include <Core/Giga.h>

enum GIGA_API {
    VERTEXTYPE_ATTRIB_POSITION      = 1,
    VERTEXTYPE_ATTRIB_COLOR         = 1 << 1,
    VERTEXTYPE_ATTRIB_NORMAL        = 1 << 2,
    VERTEXTYPE_ATTRIB_TEXCOORD0     = 1 << 3,
    VERTEXTYPE_ATTRIB_TEXCOORD1     = 1 << 4,
    VERTEXTYPE_ATTRIB_BONES         = 1 << 5,
    VERTEXTYPE_ATTRIB_BONEWEIGHTS   = 1 << 6,
    VERTEXTYPE_ATTRIB_OFFSETS       = 1 << 7,
};

class GIGA_API VertexFormat {
public:
    VertexFormat() = default;
    virtual ~VertexFormat();
    
    virtual void Initialize() { }
    virtual void Destroy() { }
    
    /**
     * Add/remove/check for vertex attribute
     */
    void AddVertexAttrib(int attrib, int size, int offset, int stride = 0);
    void RemoveVertexAttrib(int attrib);
    bool HasVertexAttrib(int attrib);
    
    int GetVertexSize() { return m_vertexSize; }
    
    /**
     * Enable a particular vertex attribute (if this vertex type has it)
     */
    virtual bool EnableAttribute(int index, int attrib) { return(false); }
    virtual bool DisableAttribute(int index) { return(false); }
    
    /**
     * Bind/unbind this vertex type
     */
    virtual void Bind() { }
    virtual void Unbind() { }
    
    /**
     * Set divisor/stride per index (instancing)
     */
    virtual void SetDivisor(int index, int divisor) { }
    
    // Internal vertex attribute layout
    struct VertexAttrib {
        int attrib;
        int size;
        int offset;
        int stride;
    };
    
protected:
    int m_vertexSize;
    int m_attribBitmask;
    std::map<int, VertexAttrib*> m_attribs;
};


#endif
