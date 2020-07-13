
/**
 * In variables
 */
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec2 vertex_texcoord0;
layout(location = 4) in vec2 vertex_texcoord1;
layout(location = 5) in vec4 vertex_bones;
layout(location = 6) in vec4 vertex_weights;
layout(location = 7) in vec3 vertex_offset;

// Vertex attributes
uniform int VERTEXTYPE_ATTRIB_POSITION;
uniform int VERTEXTYPE_ATTRIB_COLOR;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD0;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD1;
uniform int VERTEXTYPE_ATTRIB_BONES;
uniform int VERTEXTYPE_ATTRIB_BONEWEIGHTS;
uniform int VERTEXTYPE_ATTRIB_OFFSETS;

/**
 * Uniforms
 */
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

/**
 * Out variables
 */
out vec4 frag_position;
out vec3 frag_normal;
out vec3 frag_vcolor;
out vec2 frag_texcoord0;
out vec2 frag_texcoord1;

/**
 * Main (default)
 */
void main () {
    vec3 transformed_position = vertex_position.xyz;
    
    if(VERTEXTYPE_ATTRIB_OFFSETS == 1) {
        transformed_position += vertex_offset;
    }
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(transformed_position.xyz, 1.0);
    
    // Send through position, normal and texture coords
    frag_position = viewMatrix * modelMatrix * vec4(transformed_position.xyz, 1.0);
    frag_vcolor = vertex_color;
    frag_texcoord0 = vertex_texcoord0;
}
