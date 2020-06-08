
out vec4 frag_color;
  
in vec2 frag_texcoord;

uniform sampler2D inputTexture;
uniform int samples;
uniform bool horizontal;
uniform float weight[21] = float[] (
0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
0.14107424, 0.132526984, 0.109868729, 0.080381679, 0.051898313,
0.029570767, 0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337);

void main() {
    vec2 tex_offset = 1.0 / textureSize(inputTexture, 0); // gets size of single texel
    vec3 result = texture(inputTexture, frag_texcoord).rgb * weight[0]; // current fragment's contribution
    int halfSamples = samples / 2;
    if(horizontal) {
        for(int i = 0; i < samples; i++) {
            result += texture(inputTexture, vec2(frag_texcoord.x, frag_texcoord.y + (i - halfSamples) * tex_offset.x)).rgb * weight[i];
        }
    }
    else {
        for(int i = 0; i < samples; i++) {
            result += texture(inputTexture, vec2(frag_texcoord.x + (i - halfSamples) * tex_offset.y, frag_texcoord.y)).rgb * weight[i];
        }
    }
    frag_color = vec4(result, 1.0);
}
