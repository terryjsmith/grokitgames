
#include <Render/ShaderProgram.h>
#include <Core/DataRecord.h>

ShaderProgram::ShaderProgram() {
    m_vertexShader = 0;
    m_fragmentShader = 0;
}

void ShaderProgram::Initialize(Shader* vertexShader, Shader* fragmentShader) {
    m_vertexShader = vertexShader;
    m_fragmentShader = fragmentShader;
}

void ShaderProgram::Serialize(DataRecord* record) {
    record->Set("vertexShader", new Variant(m_vertexShader));
    record->Set("fragmentShader", new Variant(m_fragmentShader));
}

void ShaderProgram::Deserialize(DataRecord* record) {
    Shader* vshader = record->Get("vertexShader")->AsObject<Shader*>();
    Shader* fshader = record->Get("vertexShader")->AsObject<Shader*>();
    
    this->Initialize(vshader, fshader);
}

std::string ShaderProgram::ToString() {
    return("ShaderProgram (" + m_vertexShader->GetResource()->filename + ", " + m_fragmentShader->GetResource()->filename + ")");
}
