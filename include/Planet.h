#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/PrimitiveBatch.h>

#include "Tile.h"

/*
 * Coordinate system for m_layout
 * (0, 0)         ...         (0,NUM_COLS-1)
 *   .                              .
 *   .                              .
 * (m_sea_level, 0) ... (m_sea_level, NUM_COLS-1) 
 *   .                              .
 *   .                              .
 * (NUM_ROWS-1, 0)  ...  (NUM_ROWS-1, NUM_COLS-1)
 */

class Planet {
private:
    // (0,0) should be center of sea level in world coordinates
    glm::vec2 getOffset() const;

    std::vector<Tile> m_tiles;
    // indices into m_tiles 
    std::vector<std::vector<int>> m_layout;
    glm::vec2 m_gravity;
    // rows x columns (i.e. not width by height)
    glm::ivec2 m_dimensions;
    b2Body* m_body = nullptr;
    // row coordinate considered sea level on this planet
    int m_sea_level; 
public:
    Planet() {}
    static Planet new_test();
    ~Planet();
    b2Vec2 getGravity() const;
    float getRadius() const;
    float getHeight() const;
    void add_to_world(b2World* world);
    void render(nta::SpriteBatch& pbatch) const;
    void render_debug(nta::PrimitiveBatch& pbatch) const;
};

#endif // PLANET_H_INCLUDED