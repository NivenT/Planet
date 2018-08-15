#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/SpriteBatch.h>
#include <nta/DebugBatch.h>

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

class WorldEditor;

class Planet {
private:
    std::vector<std::vector<b2Vec2>> createOutline() const;
    bool validCoord(const glm::ivec2& coord) const;
    void set_num_rows(int num_rows);

    std::vector<std::vector<Tile>> m_tiles;
    glm::vec2 m_gravity;
    // rows x columns (i.e. not width by height)
    union {
        glm::ivec2 m_dimensions;
        struct {
            int rows;
            int cols;
        };
    };
    b2Body* m_body = nullptr;
    // row coordinate considered sea level on this planet
    int m_sea_level; 
public:
    Planet() {}
    static Planet new_test();
    static Planet load(const nta::utils::Json& json);
    ~Planet();
    nta::utils::Json json() const;
    // returns width x height in world coordinates
    glm::vec2 getDimensions() const;
    b2Vec2 getGravity() const;
    glm::vec2 getTileTopLeft(int row, int col) const;
    glm::vec2 getTileCenter(int row, int col) const;
    glm::ivec2 getCoord(glm::vec2 pos) const;
    // (0,0) should be center of sea level in world coordinates
    glm::vec2 getOffset() const;
    const Tile& getTile(const glm::ivec2& coord) const;
    float getRadius() const;
    float getHeight() const;
    void remove_tile(const glm::ivec2& coord);
    void add_to_world(b2World* world);
    void render(nta::SpriteBatch& batch) const;
    void render_debug(nta::DebugBatch& dbatch) const;
    void clear();

    friend WorldEditor;
};

#endif // PLANET_H_INCLUDED