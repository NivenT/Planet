#version 130

uniform mat3 camera;
// includes atmosphere
uniform float normalized_planet_height;
// does not include atmosphere
uniform float normalized_planet_radius;

in vec3 pos;
in vec4 color;
in vec2 uv;
in float hasTex;

out vec4 fragColor;
out vec2 fragUV;
out float hasTexture;

float cmplmag2(vec2 z) {
    return z.x*z.x + z.y*z.y;
}

float cmplmag(vec2 z) {
    return sqrt(cmplmag2(z));
}

vec2 cmplconj(vec2 z) {
    return vec2(z.x, -z.y);
}

vec2 cmplmul(vec2 lhs, vec2 rhs) {
    return vec2(lhs.x*rhs.x - lhs.y-rhs.y, lhs.x*rhs.y + lhs.y*rhs.x);
}

vec2 cmpldiv(vec2 lhs, vec2 rhs) {
    return cmplmul(lhs, cmplconj(rhs))/cmplmag2(rhs);
}

vec2 cmplexp(vec2 z) {
    return exp(z.x) * vec2(cos(z.y), sin(z.y));
}

void main() {
    /*
    vec2 circ_pos = (planet_radius + pos.y) * cmplexp(pos.yx/planet_height);
    vec3 position = camera * vec3(circ_pos, 1);
    gl_Position = vec4(position.xy, pos.z, 1);
    /**/
    vec3 position = camera * vec3(pos.xy, 1);
    vec2 circ_pos = (normalized_planet_radius + position.y) * 
                        cmplexp(position.yx/normalized_planet_height);
    gl_Position = vec4(circ_pos.y, circ_pos.x - normalized_planet_radius, pos.z, 1);
    /**/
    fragColor = color;
    fragUV = uv;
    hasTexture = hasTex;
}
