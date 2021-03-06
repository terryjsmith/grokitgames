
#ifndef shaderprogram_h
#define shaderprogram_h

#include <Render/Shader.h>

/**
 * A shader program - a combination vertex and fragment shader
 */
GIGA_CLASS()
class GIGA_API ShaderProgram : public GigaObject {
public:
    ShaderProgram();
    ~ShaderProgram() = default;
    
    GIGA_CLASS_NAME("ShaderProgram");
    
    /**
     * Initialize (set shaders)
     */
    virtual void Initialize(Shader* vertexShader, Shader* fragmentShader);
    
    /**
     * Bind/unbind
     */
    virtual void Bind() { }
    virtual void Unbind() { }
    
    /**
     * Get the location of a uniform variable
     */
    virtual int Uniform(std::string name) { return(0); }
    
    /**
     * Get the location of an attribute
     */
    virtual int Attribute(std::string name) { return(0); }
    
    /**
     * Variable setting functions
     */
    virtual void Set(std::string name, float value) { }
    virtual void Set(std::string name, vector2 value) { }
    virtual void Set(std::string name, vector3 value) { }
    virtual void Set(std::string name, vector4 value) { }
    virtual void Set(std::string name, int value) { }
    virtual void Set(std::string name, unsigned int value) { }
    virtual void Set(std::string name, matrix4 value) { }
    
    /**
     * Serialize / deserialize
     */
    virtual void Serialize(DataRecord* record);
    virtual void Deserialize(DataRecord* record);
    
    /**
     * Show a string representation of an object (defaults to giga name)
     */
    virtual std::string ToString();
    
    /**
     * A variable inside the shader program (to cache its location)
     */
    struct ShaderVariable {
        std::string name;
        int location;
        bool uniform;
    };
    
protected:
    Shader* m_vertexShader;
    Shader* m_fragmentShader;
    
    // Cache of shader locations
    Array<ShaderVariable*> m_vars;
    
    // Definitions
    Array<std::string> m_definitions;
};

#endif
