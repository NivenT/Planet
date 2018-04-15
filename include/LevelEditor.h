#ifndef LEVELEDITOR_H_INCLUDED
#define LEVELEDITOR_H_INCLUDED

#include <nta/Screen.h>
#include <nta/Camera2D.h>
#include <nta/GLSLProgram.h>
#include <nta/SpriteFont.h>

class LevelEditor : public nta::Screen {
private:
    nta::SpriteFont* m_font = nullptr;
    nta::GLSLProgram* m_prog = nullptr;
    nta::SpriteBatch m_batch;
    nta::Camera2D m_camera;
public:
    LevelEditor();
    ~LevelEditor();
    void onFocus();
    void offFocus();
    void update();
    void render();
    void init();
};

#endif // LEVELEDITOR_H_INCLUDED
