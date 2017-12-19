#ifndef MAINGAME_H_INCLUDED
#define MAINGAME_H_INCLUDED

#include "nta/PrimitiveBatch.h"
#include "nta/SpriteBatch.h"
#include "nta/GLSLProgram.h"
#include "nta/Camera2D.h"
#include "nta/Screen.h"

#include "Planet.h"

class MainGame : public nta::Screen {
private:
    nta::GLSLProgram* m_simpleProg = nullptr;
    nta::SpriteBatch  m_batch;
    nta::PrimitiveBatch m_pbatch;
    nta::Camera2D m_camera;

    Planet m_planet;
    float m_time;
public:
    MainGame();
    ~MainGame();
    void update();
    void render();
    void init();
};

#endif // MAINGAME_H_INCLUDED
