
#ifndef variantref_h
#define variantref_h

#include <Core/Variant.h>

class GIGA_API VariantRef {
public:
    VariantRef();
    ~VariantRef();
    
    /**
     * Constructors
     */
    VariantRef(int32_t* value);
    VariantRef(uint32_t* value);
    VariantRef(int64_t* value);
    VariantRef(uint64_t* value);
    VariantRef(bool* value);
    VariantRef(float* value);
    VariantRef(vector2* value);
    VariantRef(vector3* value);
    VariantRef(vector4* value);
    VariantRef(quaternion* value);
    VariantRef(std::string* value);
    VariantRef(GigaObject** value);
    VariantRef(const VariantRef& value);
    
    /**
     * Set operators
     */
    VariantRef& operator =(int32_t* rhs);
    VariantRef& operator =(uint32_t* rhs);
    VariantRef& operator =(int64_t* rhs);
    VariantRef& operator =(uint64_t* rhs);
    VariantRef& operator =(bool* rhs);
    VariantRef& operator =(float* rhs);
    VariantRef& operator =(vector2* rhs);
    VariantRef& operator =(vector3* rhs);
    VariantRef& operator =(vector4* rhs);
    VariantRef& operator =(quaternion* rhs);
    VariantRef& operator =(std::string* rhs);
    VariantRef& operator =(GigaObject** rhs);
    VariantRef& operator =(const VariantRef& rhs);
    
    /**
     * Checkers
     */
    bool IsInt();
    bool IsUInt();
    bool IsInt64();
    bool IsUInt64();
    bool IsBool();
    bool IsFloat();
    bool IsNumeric();
    bool IsVector2();
    bool IsVector3();
    bool IsVector4();
    bool IsQuaternion();
    bool IsString();
    bool IsObject();
    bool IsNull();
    bool IsArray();
    
    /**
     * Get operators
     */
    int32_t* AsInt();
    uint32_t* AsUInt();
    int64_t* AsInt64();
    uint64_t* AsUInt64();
    bool* AsBool();
    float* AsFloat();
    vector2* AsVector2();
    vector3* AsVector3();
    vector4* AsVector4();
    quaternion* AsQuaternion();
    std::string* AsString();
    GigaObject** AsObject();
    template<class T> T* AsObject() {
        if (m_type == Variant::VAR_OBJECT) {
            return(dynamic_cast<T*>(m_data));
        }
        
        return(0);
    }
    
protected:
    void* m_data;
    int m_type;
};

#endif
