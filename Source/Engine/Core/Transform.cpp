
#include <Core/Transform.h>
#include <Core/DataRecord.h>
#include <glm/gtx/transform.hpp>

Transform::Transform() {
    m_position = vector3(0, 0, 0);
    m_scaling = vector3(1, 1, 1);
    m_rotation = quaternion(1, 0, 0, 0);
    
    // Initialize to zero so we know if we're using them (length = 0)
    m_up = vector3(0, 0, 0);
    m_right = vector3(0, 0, 0);
    m_look = vector3(0, 0, 0);
    
    m_parent = 0;
}

void Transform::SetParent(Transform* transform) {
    m_parent = transform;
    m_parent->AddChild(this);
}

vector3 Transform::GetWorldPosition() {
    vector3 sum = m_position;
    if (m_parent) {
        sum += m_parent->GetWorldPosition();
    }
    
    return(sum);
}

vector3 Transform::GetWorldScaling() {
    vector3 sum = m_scaling;
    if (m_parent) {
        sum *= m_parent->GetWorldScaling();
    }
    
    return(sum);
}

quaternion Transform::GetWorldRotation() {
    quaternion sum = m_rotation;
    if (m_parent) {
        sum *= m_parent->GetWorldRotation();
    }
    
    return(sum);
}

void Transform::SetLocalPosition(vector3 position) {
    m_position = position;
}

void Transform::SetWorldPosition(vector3 position) {
    if (m_parent) {
        m_parent->SetWorldPosition(position);
        return;
    }
    
    m_position = position;
}

void Transform::SetLocalScaling(vector3 scaling) {
    m_scaling = scaling;
}

void Transform::SetWorldScaling(vector3 scaling) {
    if (m_parent) {
        m_parent->SetWorldScaling(scaling);
        return;
    }
    
    m_scaling = scaling;
}

void Transform::SetLocalRotation(quaternion rotation) {
    m_rotation = rotation;
}

void Transform::SetWorldRotation(quaternion rotation) {
    m_look = m_up = m_right = vector3(0, 0, 0);
    
    if (m_parent) {
        m_parent->SetWorldRotation(rotation);
        return;
    }
    
    m_rotation = rotation;
}

matrix4 Transform::GetMatrix() {
    matrix4 t = glm::translate(glm::mat4(1.0), GetWorldPosition());
    matrix4 s = glm::scale(glm::mat4(1.0), GetWorldScaling());
    matrix4 r = glm::toMat4(GetWorldRotation());
    
    matrix4 modelMatrix = t * r * s;
    
    return(modelMatrix);
}

vector3 Transform::GetUp() {
    if(glm::length(m_up)) return(m_up);
    
    quaternion rotation = this->GetWorldRotation();
    return(rotation * vector3(0, 1, 0));
}

vector3 Transform::GetRight() {
    if(glm::length(m_right)) return(m_right);
    
    quaternion rotation = this->GetWorldRotation();
    return(rotation * vector3(1, 0, 0));
}

vector3 Transform::GetLook() {
    if(glm::length(m_look)) return(m_look);
    
    quaternion rotation = this->GetWorldRotation();
    return(rotation * vector3(0, 0, -1));
}

void Transform::SetUp(vector3 up) {
    m_up = up;
}

void Transform::SetRight(vector3 right) {
    m_right = right;
}

void Transform::SetLook(vector3 look) {
    m_look = look;
}

void Transform::Move(vector3 amount) {
    if (m_root && m_parent) {
        m_parent->Move(amount);
        return;
    }
    
    m_position += amount;
}

void Transform::Rotate(vector3 axis, float degrees) {
    m_look = m_up = m_right = vector3(0, 0, 0);
    
    if (m_root && m_parent) {
        m_parent->Rotate(axis, degrees);
        return;
    }
    
    quaternion delta = glm::angleAxis(glm::radians(degrees), axis);
    m_rotation = m_rotation * delta;
}

void Transform::Scale(vector3 amount) {
    if (m_root && m_parent) {
        m_parent->Scale(amount);
        return;
    }
    
    m_scaling *= amount;
}

void Transform::AddChild(Transform* transform) {
    auto index = m_children.find(transform);
    if(index == m_children.end()) {
        m_children.push_back(transform);
    }
}

void Transform::RemoveChild(Transform* transform) {
    auto index = m_children.find(transform);
    if(index != m_children.end()) {
        m_children.erase(index);
    }
}

void Transform::Serialize(DataRecord* record) {
    record->Set("position", new Variant(m_position));
    record->Set("scaling", new Variant(m_scaling));
    record->Set("rotation", new Variant(m_rotation));
}

void Transform::Deserialize(DataRecord* record) {
    m_position = record->Get("position")->AsVector3();
    m_scaling = record->Get("scaling")->AsVector3();
    m_rotation = record->Get("rotation")->AsQuaternion();
}
