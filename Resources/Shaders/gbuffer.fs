
/**
 * In variables
 */
in vec2 frag_texcoord0;
in vec3 frag_normal;
in vec4 frag_position;
in vec2 frag_texcoord1;
in vec3 frag_vcolor;
flat in float frag_depth;

// Vertex attributes
uniform int VERTEXTYPE_ATTRIB_POSITION;
uniform int VERTEXTYPE_ATTRIB_COLOR;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD0;
uniform int VERTEXTYPE_ATTRIB_TEXCOORD1;
uniform int VERTEXTYPE_ATTRIB_BONES;
uniform int VERTEXTYPE_ATTRIB_BONEWEIGHTS;

/**
 * Uniforms
 */

// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D textureMaterialLookup;

// Index into scene
uniform float sceneIndex;
uniform float materialID;

/**
 * Out variables
 */
layout (location = 0) out vec3 out_diffuse;
layout (location = 1) out vec3 out_position;
layout (location = 2) out vec3 out_normal;
layout (location = 3) out vec3 out_aux;

/**
 * Main
 */
void main () {
    vec4 frag = vec4(1.0);
    
    // Sample our texture
    if(VERTEXTYPE_ATTRIB_COLOR == 1) {
        frag = vec4(frag_vcolor, 1.0);
    }
    
    if(VERTEXTYPE_ATTRIB_TEXCOORD0 == 1) {
        frag = texture(diffuseTexture, frag_texcoord0.st);
    }
    
    // If this fragment is transparent, discard it
    if(frag.a < 0.5) {
        discard;
    }
    
    float y = materialID / 1024.0;
    vec3 diffuse = texture(textureMaterialLookup, vec2(0.0, y)).xyz;
    
    out_diffuse = diffuse * frag.rgb;
    out_position = frag_position.xyz;
    out_normal = frag_normal;
    out_aux = vec3(sceneIndex, materialID, frag_depth);
}
