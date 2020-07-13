
/**
 * Structs
 */
struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 emissionColor;
    float emissionStrength;
    float shininess;
};

/**
 * In variables
 */
in vec3 frag_normal;
in vec4 frag_position;
in vec3 frag_vcolor;
in vec2 frag_texcoord0;

/**
 * Uniforms
 */

// Index into scene
uniform float sceneIndex;
uniform float childIndex;
uniform float materialID;

// Sample textures
uniform sampler2D textureDiffuse;
uniform sampler2D textureMaterialLookup;

// Vertex attributes
uniform int VERTEXTYPE_ATTRIB_POSITION;
uniform int VERTEXTYPE_ATTRIB_COLOR;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD0;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD1;
uniform int VERTEXTYPE_ATTRIB_BONES;
uniform int VERTEXTYPE_ATTRIB_BONEWEIGHTS;

/**
 * Out variables
 */
layout (location = 0) out vec4 out_diffuse;
layout (location = 1) out vec4 out_aux;

/**
 * Main
 */
void main () {
    vec4 frag = vec4(1.0);
    
    // Re-construct our material from our material texture
    Material mat;
    // float materialID = texture(textureMaterial, frag_texcoord.xy).y;
    float y = materialID / 1024.0;
    
    mat.diffuse = texture(textureMaterialLookup, vec2(0.0, y)).xyz;
    
    // Start with our material color
    out_diffuse = vec4(mat.diffuse, 1.0);
    
    if(VERTEXTYPE_ATTRIB_COLOR == 1) {
        out_diffuse = vec4(frag_vcolor, 1.0);
    }
    
    if(VERTEXTYPE_ATTRIB_TEXCOORD0 == 1) {
        out_diffuse = texture(textureDiffuse, frag_texcoord0.st);
    }
    
    out_aux = vec4(sceneIndex, 0, 0, childIndex);
}
