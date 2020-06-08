
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

// Sample textures
uniform sampler2D textureDiffuse;

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
layout (location = 0) out vec3 out_diffuse;

/**
 * Main
 */
void main () {
    vec4 frag = vec4(1.0);
    
    if(VERTEXTYPE_ATTRIB_COLOR == 1) {
        out_diffuse = frag_vcolor;
    }
    
    // If this fragment is transparent, discard it
    if(frag.a < 0.5) {
        discard;
    }
    
    if(VERTEXTYPE_ATTRIB_TEXCOORD0 == 1) {
        out_diffuse = texture(textureDiffuse, frag_texcoord0.st).rgb;
    }
}
