
#include <Core/VariantRef.h>

VariantRef::VariantRef() {
    m_type = 0;
}

VariantRef::~VariantRef() {

}

VariantRef::VariantRef(int32_t* value) {
    *this = value;
}

VariantRef::VariantRef(uint32_t* value) {
    *this = value;
}

VariantRef::VariantRef(int64_t* value) {
    *this = value;
}

VariantRef::VariantRef(uint64_t* value) {
    *this = value;
}

VariantRef::VariantRef(bool* value) {
    *this = value;
}

VariantRef::VariantRef(float* value) {
    *this = value;
}

VariantRef::VariantRef(vector2* value) {
    *this = value;
}

VariantRef::VariantRef(vector3* value) {
    *this = value;
}

VariantRef::VariantRef(vector4* value) {
    *this = value;
}

VariantRef::VariantRef(quaternion* value) {
    *this = value;
}

VariantRef::VariantRef(std::string* value) {
    *this = value;
}

VariantRef::VariantRef(GigaObject** value) {
    *this = value;
}

VariantRef::VariantRef(const VariantRef& value) {
    *this = value;
}

VariantRef& VariantRef::operator =(int32_t* rhs) {
    m_type = Variant::VAR_INT32;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(uint32_t* rhs) {
    m_type = Variant::VAR_UINT32;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(int64_t* rhs) {
    m_type = Variant::VAR_INT64;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(uint64_t* rhs) {
    m_type = Variant::VAR_UINT64;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(bool* rhs) {
    m_type = Variant::VAR_BOOL;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(float* rhs) {
    m_type = Variant::VAR_FLOAT;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(vector2* rhs) {
    m_type = Variant::VAR_VECTOR2;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(vector3* rhs) {
    m_type = Variant::VAR_VECTOR3;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(vector4* rhs) {
    m_type = Variant::VAR_VECTOR4;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(quaternion* rhs) {
    m_type = Variant::VAR_QUATERNION;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(std::string* rhs) {
    m_type = Variant::VAR_STRING;
    m_data = rhs;
    return *this;
}

VariantRef& VariantRef::operator =(GigaObject** rhs) {
    m_type = Variant::VAR_OBJECT;
    m_data = rhs;
    
    // what to do about m_size here?
    return *this;
}

VariantRef& VariantRef::operator =(const VariantRef& rhs) {
    m_type = rhs.m_type;
    m_data = rhs.m_data;
    return *this;
}

bool VariantRef::IsInt() {
    return(m_type == Variant::VAR_INT32 || m_type == Variant::VAR_INT64);
}

bool VariantRef::IsUInt() {
    return(m_type == Variant::VAR_UINT32 || m_type == Variant::VAR_UINT64);
}

bool VariantRef::IsInt64() {
    return(m_type == Variant::VAR_INT64);
}

bool VariantRef::IsUInt64() {
    return(m_type == Variant::VAR_UINT64);
}

bool VariantRef::IsBool() {
    return(m_type == Variant::VAR_BOOL);
}

bool VariantRef::IsFloat() {
    return(m_type == Variant::VAR_FLOAT || m_type == Variant::VAR_INT32 || m_type == Variant::VAR_UINT32);
}

bool VariantRef::IsNumeric() {
    return(m_type == Variant::VAR_INT32 || m_type == Variant::VAR_UINT32 || m_type == Variant::VAR_INT64 || m_type == Variant::VAR_UINT64 || m_type == Variant::VAR_FLOAT);
}

bool VariantRef::IsVector2() {
    return(m_type == Variant::VAR_VECTOR2);
}

bool VariantRef::IsVector3() {
    return(m_type == Variant::VAR_VECTOR3);
}

bool VariantRef::IsVector4() {
    return(m_type == Variant::VAR_VECTOR4);
}

bool VariantRef::IsQuaternion() {
    return(m_type == Variant::VAR_QUATERNION);
}

bool VariantRef::IsString() {
    return(m_type == Variant::VAR_STRING);
}

bool VariantRef::IsObject() {
    return(m_type == Variant::VAR_OBJECT);
}

bool VariantRef::IsNull() {
    return(m_type == 0);
}

int32_t* VariantRef::AsInt() {
    if (m_type == Variant::VAR_INT32) {
        return((int32_t*)m_data);
    }
    
    return(0);
}

uint32_t* VariantRef::AsUInt() {
    if (m_type == Variant::VAR_UINT32) {
        return((uint32_t*)m_data);
    }

    return(0);
}

int64_t* VariantRef::AsInt64() {
    if (m_type == Variant::VAR_INT64) {
        return((int64_t*)m_data);
    }
    
    return(0);
}

uint64_t* VariantRef::AsUInt64() {
    if (m_type == Variant::VAR_UINT64) {
        return((uint64_t*)m_data);
    }
    
    return(0);
}

bool* VariantRef::AsBool() {
    if (m_type == Variant::VAR_BOOL) {
        return((bool*)m_data);
    }
    
    return(0);
}

float* VariantRef::AsFloat() {
    if (m_type == Variant::VAR_FLOAT) {
        return((float*)m_data);
    }
    
    return(0);
}

vector2* VariantRef::AsVector2() {
    if (m_type == Variant::VAR_VECTOR2) {
        return((vector2*)m_data);
    }
    
    return(0);
}

vector3* VariantRef::AsVector3() {
    if (m_type == Variant::VAR_VECTOR3) {
        return((vector3*)m_data);
    }
    
    return(0);
}

vector4* VariantRef::AsVector4() {
    if (m_type == Variant::VAR_VECTOR4) {
        return((vector4*)m_data);
    }
    
    return(0);
}

quaternion* VariantRef::AsQuaternion() {
    if (m_type == Variant::VAR_QUATERNION) {
        return((quaternion*)m_data);
    }
    
    return(0);
}

std::string* VariantRef::AsString() {
    if (m_type == Variant::VAR_STRING) {
        return((std::string*)m_data);
    }
    
    return(0);
}

GigaObject** VariantRef::AsObject() {
    if (m_type == Variant::VAR_OBJECT) {
        return((GigaObject**)m_data);
    }
    
    return(0);
}
