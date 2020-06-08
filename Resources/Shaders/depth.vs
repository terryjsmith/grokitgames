
/**
 * In variables
 */
layout(location = 0) in vec3 vertex_position;
layout(location = 5) in vec4 vertex_bones;
layout(location = 6) in vec4 vertex_weights;

// Vertex attributes
uniform int VERTEXTYPE_ATTRIB_POSITION;
uniform int VERTEXTYPE_ATTRIB_BONES;
uniform int VERTEXTYPE_ATTRIB_BONEWEIGHTS;

/**
 * Uniforms
 */
uniform mat4 projectionMatrix;
uniform mat4 modelviewMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 boneMatrix[100];

/**
 * Out variables
 */
out vec4 frag_position;

/**
 * Main (default)
 */
void main () {
    vec3 transformed_position = vertex_position.xyz;
    
    if(VERTEXTYPE_ATTRIB_BONES == 1) {
        mat4 BoneTransform = boneMatrix[int(vertex_bones[0])] * vertex_weights[0];
        BoneTransform += boneMatrix[int(vertex_bones[1])] * vertex_weights[1];
        BoneTransform += boneMatrix[int(vertex_bones[2])] * vertex_weights[2];
        BoneTransform += boneMatrix[int(vertex_bones[3])] * vertex_weights[3];
                
        vec4 bposition = BoneTransform * vec4(transformed_position.xyz, 1.0);
        transformed_position = bposition.xyz;
    }
    
    mat4 mvpMatrix = projectionMatrix * modelviewMatrix;
    mat4 lightMatrix = projectionMatrix * viewMatrix;
    gl_Position = mvpMatrix * vec4(transformed_position.xyz, 1.0);
    
    frag_position = mvpMatrix * vec4(transformed_position.xyz, 1.0);
}
