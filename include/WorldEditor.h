#ifndef WORLDEDITOR_H_INCLUDED
#define WORLDEDITOR_H_INCLUDED

#include <nta/Screen.h>
#include <nta/Camera2D.h>
#include <nta/GLSLProgram.h>
#include <nta/SpriteFont.h>

#include "Planet.h"

class WorldEditor : public nta::Screen {
private:
    void prepare_batches();
    void render_batches(const nta::Camera2D camera);
    void render_miniworld();

    nta::SpriteFont* m_font = nullptr;
    nta::GLSLProgram* m_prog = nullptr;
    nta::GLSLProgram* m_planet_prog = nullptr;
    nta::SpriteBatch m_batch;
    nta::Camera2D m_camera;

    Planet m_planet;
    bool m_square_planet = false;
public:
    WorldEditor();
    ~WorldEditor();
    void onFocus();
    void offFocus();
    void update();
    void render();
    void init();
};

#endif // WORLDEDITOR_H_INCLUDED
