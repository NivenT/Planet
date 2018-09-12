#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <nta/SpriteBatch.h>

struct Light {
    Light() {}
    Light(nta::crvec2 pos, nta::crvec3 col, float s, float b = 1.0) : 
        center(pos), color(col), radius(s), brightness(b) {
    }
    Light(nta::crvec2 pos, nta::crvec4 col, float s, float b = 1.0) :
        center(pos), radius(s), brightness(b) {
        color.r = col.r * col.a;
        color.g = col.g * col.a;
        color.b = col.b * col.a;
    }
    Light(nta::crvec2 pos, float s, float b = 1.0) :
        center(pos), color(glm::vec3(1)), radius(s), brightness(b) {
    }
    void render(nta::SpriteBatch& batch) {
        glm::vec4 posRect(center.x - radius, center.y + radius, 2*radius, 2*radius);
        batch.addGlyph(posRect, glm::vec4(-1,-1,2,2), 0, glm::vec4(color, brightness));
    }

    glm::vec2 center;
    glm::vec3 color;
    float radius;
    float brightness;
};

#endif // LIGHT_H_INCLUDED
