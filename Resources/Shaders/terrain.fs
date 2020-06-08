
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

/**
 * Uniforms
 */

// Textures
uniform sampler2D normalTexture;
uniform sampler2D textureMaterialLookup;

uniform sampler2D diffuseTextures[4];
uniform sampler2D splatMap;

uniform int numTextures;

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
    vec4 diffuse1 = vec4(1.0);
    vec4 diffuse2 = vec4(1.0);
    vec4 diffuse3 = vec4(1.0);
    
    // Sample our texture
    if(VERTEXTYPE_ATTRIB_COLOR == 1) {
        diffuse1 = vec4(frag_vcolor, 1.0);
    }
    
    if(VERTEXTYPE_ATTRIB_TEXCOORD0 == 1) {
        diffuse1 = texture(diffuseTextures[0], frag_texcoord0.st);
        diffuse2 = texture(diffuseTextures[1], frag_texcoord0.st);
        diffuse3 = texture(diffuseTextures[2], frag_texcoord0.st);
    }
    
    vec4 splat = texture(splatMap, frag_texcoord1.st);
    diffuse1 = mix(diffuse1, diffuse2, splat.g);
    diffuse1 = mix(diffuse1, diffuse3, splat.b);
    
    // If this fragment is transparent, discard it
    if(diffuse1.a < 0.5) {
        discard;
    }
    
    out_diffuse = diffuse1.rgb;
    out_position = frag_position.xyz;
    out_normal = frag_normal;
    out_aux = vec3(-1, 0, frag_depth);
}
