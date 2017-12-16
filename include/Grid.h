#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include "nta/PrimitiveBatch.h"

#define TILE_SIZE 10.f // The length of one side of the dodecagon

struct RegularPolygon {
    std::size_t numSides;
    glm::vec2 center;
    glm::vec4 color;
    float sideLength;
    float orientation;
};

class Grid {
private:
    void render_tile(nta::PrimitiveBatch& pbatch, crvec2 pos);
    
    std::vector<RegularPolygon> m_tile;
public:
    Grid();
    ~Grid();
    void render(nta::PrimitiveBatch& pbatch);
};

#endif // GRID_H_INCLUDED
