
out vec3 frag_color;

in vec3 frag_texcoord;

uniform samplerCube skybox;

void main() {
    frag_color = texture(skybox, frag_texcoord).rgb;
}
