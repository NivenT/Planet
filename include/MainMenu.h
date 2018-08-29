#ifndef MAINMENU_H_INCLUDED
#define MAINMENU_H_INCLUDED

#include <nta/SpriteFont.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Screen.h>

struct Button {
    bool isPressed(nta::crvec2 mouse) const {
        return ((mouse.x >= bounds.x && mouse.x <= bounds.x+bounds[2]) &&
                (mouse.y <= bounds.y && mouse.y >= bounds.y-bounds[3]));
    }
    void draw(nta::SpriteFont* font, nta::SpriteBatch& batch) const {
        batch.addGlyph(bounds, glm::vec4(0,0,1,1), backgroundTexture, .5, backgroundColor);
        font->drawText(batch, name, bounds);
    }
    glm::vec4 bounds;
    glm::vec4 backgroundColor;
    std::string name;
    GLuint backgroundTexture = 0;
    int next_index;
};

class MainMenu : public nta::Screen {
private:
    nta::SpriteFont* m_font = nullptr;
    nta::GLSLProgram* m_simpleProg = nullptr;
    nta::SpriteBatch m_hudBatch;
    nta::Camera2D m_hudCamera;
    Button m_buttons[2];

    // void* because I'm too lazy to type #include "World.h"
    void* m_world = nullptr;
public:
    MainMenu();
    ~MainMenu();
    void onFocus(const nta::ScreenSwitchInfo& info);
    void offFocus();
    void update();
    void render();
    void init();
};

#endif // MAINMENU_H_INCLUDED
