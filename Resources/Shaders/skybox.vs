
layout (location = 0) in vec3 vertex_position;

uniform mat4 projectionMatrix;
uniform mat4 modelviewMatrix;

out vec3 frag_texcoord;

void main() {
    mat4 mvpMatrix = projectionMatrix * modelviewMatrix;
    vec4 WVP_Pos = mvpMatrix * vec4(vertex_position, 1.0);
    
    gl_Position = WVP_Pos.xyww;
    frag_texcoord = vertex_position;
}
