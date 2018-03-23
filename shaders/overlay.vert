#version 150

// (0,0) is bottom left of screen and (100, 100) is top right
in vec3 pos;
in vec4 color;
in vec2 uv;
in float hasTex;

out vec4 fragColor;
out vec2 fragUV;
out float hasTexture;

void main() {
    // for gl_Position (-1, 1) is top left and (1, -1) is bottom right
    gl_Position = vec4(pos.xy/50.0 - 1.0, pos.z, 1);

    fragColor = color;
    fragUV = uv;
    hasTexture = hasTex;
}
