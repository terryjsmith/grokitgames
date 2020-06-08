
/**
 * In variables
 */
in vec4 frag_position;

/**
 * Uniforms
 */
uniform float farPlane;
uniform vec3 lightPosition;

/**
 * Main
 */
void main () {
    // Output depth
    //float distance = length(vec3(frag_position.xyz));
    //gl_FragDepth = distance / farPlane;
}
