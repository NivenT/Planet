#version 130

uniform sampler2D sampler;
uniform float is_light;

in vec4 fragColor;
in vec2 fragUV;
in float hasTexture;

out vec4 finalColor;

float light_decay(float dist) {
    //return 1./(1+exp(10*dist-5));
    return 1 - dist*dist;
    //return 1 - dist;
}

void main() {
    if (is_light > 0.5) {
        finalColor = vec4(fragColor.rgb, max(fragColor.a * light_decay(length(fragUV)), 0));
    } else {
        finalColor = hasTexture > 0.5 ? fragColor * texture(sampler, fragUV) : fragColor;
    }
    
}