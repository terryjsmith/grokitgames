
#ifndef variant_h
#define variant_h

#include <Core/Giga.h>
#include <Core/MathTypes.h>

class GigaObject;

class GIGA_API Variant {
public:
    Variant();
    virtual ~Variant();
    
    // Supported types
    enum Type {
        VAR_INT32 = 1,
        VAR_UINT32,
        VAR_INT64,
        VAR_UINT64,
        VAR_BOOL,
        VAR_FLOAT,
        VAR_VECTOR2,
        VAR_VECTOR3,
        VAR_VECTOR4,
        VAR_QUATERNION,
        VAR_STRING,
        VAR_OBJECT,
        VAR_ARRAY,
        VAR_LASTTYPE
    };
    
    struct Value {
        union {
            int32_t i32;
            uint32_t ui32;
            int64_t i64;
            uint64_t ui64;
            bool b;
            float f1;
            char* str;
            void* ptr;
            GigaObject* obj;
        };
        
        union {
            float f2;
        };
        
        union {
            float f3;
        };
        
        union {
            float f4;
        };
    };
    
    /**
     * Constructors
     */
    Variant(int32_t value);
    Variant(uint32_t value);
    Variant(int64_t value);
    Variant(uint64_t value);
    Variant(bool value);
    Variant(float value);
    Variant(char* value);
    Variant(vector2 value);
    Variant(vector3 value);
    Variant(vector4 value);
    Variant(quaternion value);
    Variant(std::string value);
    Variant(GigaObject* value);
    Variant(const Variant& value);
    
    /**
     * Set operators
     */
    Variant& operator =(int32_t rhs);
    Variant& operator =(uint32_t rhs);
    Variant& operator =(int64_t rhs);
    Variant& operator =(uint64_t rhs);
    Variant& operator =(bool rhs);
    Variant& operator =(float rhs);
    Variant& operator =(char* rhs);
    Variant& operator =(vector2 rhs);
    Variant& operator =(vector3 rhs);
    Variant& operator =(vector4 rhs);
    Variant& operator =(quaternion rhs);
    Variant& operator =(std::string rhs);
    Variant& operator =(GigaObject* rhs);
    Variant& operator =(const Variant& rhs);
    Variant* operator[] (int index); 
    
    bool operator ==(const Variant &rhs) const;
    bool operator ==(const int32_t &rhs) const;
    bool operator ==(const uint32_t &rhs) const;
    bool operator ==(const int64_t &rhs) const;
    bool operator ==(const uint64_t &rhs) const;
    bool operator ==(const bool &rhs) const;
    bool operator ==(const float &rhs) const;
    bool operator ==(const vector2 &rhs) const;
    bool operator ==(const vector3 &rhs) const;
    bool operator ==(const vector4 &rhs) const;
    bool operator ==(const quaternion &rhs) const;
    bool operator ==(const GigaObject* &rhs) const;
    
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
     * Convert value to string
     */
    std::string ToString();
    
    /**
     * Convert value from string
     */
    void FromString(std::string value, int type);
    
    /**
     * Add / remove members to array
     */
    void Add(Variant* var);
    void Remove(Variant* var);
    void Remove(uint32_t index);
    
    /**
     * Get operators
     */
    int32_t AsInt();
    uint32_t AsUInt();
    int64_t AsInt64();
    uint64_t AsUInt64();
    bool AsBool();
    float AsFloat();
    vector2 AsVector2();
    vector3 AsVector3();
    vector4 AsVector4();
    quaternion AsQuaternion();
    std::string AsString();
    GigaObject* AsObject();
    template<class T> T AsObject() {
        if (m_type == VAR_OBJECT) {
            return(dynamic_cast<T>(m_data.obj));
        }
        
        return(0);
    }
    
    /**
     * Get binary size
     */
    uint32_t Size() { return m_size; }
    
protected:
    // Type
    int m_type;
    
    // Size in bytes (or size of array)
    uint32_t m_size;
    
    // Value data
    Value m_data;
    
    // Array data
    Variant** m_array;
    
    // Size of array pool / increment
    uint32_t m_pool;
    uint32_t m_poolIncr;
};

#endif
