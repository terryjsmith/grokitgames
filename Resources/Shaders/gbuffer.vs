
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

// Vertex attributes
uniform int VERTEXTYPE_ATTRIB_POSITION;
uniform int VERTEXTYPE_ATTRIB_COLOR;
uniform int VERTEXTYPE_ATTRIB_NORMAL;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD0;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD1;
uniform int VERTEXTYPE_ATTRIB_BONES;
uniform int VERTEXTYPE_ATTRIB_BONEWEIGHTS;

/**
 * Uniforms
 */
uniform mat4 projectionMatrix;
uniform mat4 modelviewMatrix;
uniform mat4 modelMatrix;
uniform mat4 boneMatrix[100];
uniform vec3 cameraPosition;

/**
 * Out variables
 */
out vec4 frag_position;
out vec3 frag_normal;
out vec2 frag_texcoord0;
out vec2 frag_texcoord1;
flat out float frag_depth;

out vec3 frag_vcolor;

/**
 * Main (default)
 */
void main () {
    vec3 transformed_position = vertex_position.xyz;
    vec3 transformed_normal = vertex_normal.xyz;
    
    if(VERTEXTYPE_ATTRIB_BONES == 1) {
        mat4 BoneTransform = boneMatrix[int(vertex_bones[0])]  * vertex_weights[0];
        BoneTransform += boneMatrix[int(vertex_bones[1])]  * vertex_weights[1];
        BoneTransform += boneMatrix[int(vertex_bones[2])] * vertex_weights[2];
        BoneTransform += boneMatrix[int(vertex_bones[3])] * vertex_weights[3];
    
        vec4 bposition = BoneTransform * vec4(transformed_position.xyz, 1.0);
        transformed_position = bposition.xyz;
        
        bposition = BoneTransform * vec4(transformed_normal.xyz, 0.0);
        transformed_normal = bposition.xyz;
    }
    
    mat4 mvpMatrix = projectionMatrix * modelviewMatrix;
    gl_Position = mvpMatrix * vec4(transformed_position.xyz, 1.0);
    
    // Send through position, normal and texture coords
    frag_position = modelviewMatrix * vec4(transformed_position.xyz, 1.0);
    frag_normal = mat3(modelviewMatrix) * normalize(transformed_normal);
    vec4 depth_position = modelMatrix * vec4(transformed_position.xyz, 1.0);
    
    if(VERTEXTYPE_ATTRIB_TEXCOORD0 == 1) {
        frag_texcoord0 = vertex_texcoord0;
    }
    
    if(VERTEXTYPE_ATTRIB_TEXCOORD1 == 1) {
        frag_texcoord1 = vertex_texcoord1;
    }
    
    if(VERTEXTYPE_ATTRIB_COLOR == 1) {
        frag_vcolor = vertex_color;
    }
    
    frag_depth = length(cameraPosition - depth_position.xyz);
}
