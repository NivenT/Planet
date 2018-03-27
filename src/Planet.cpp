#include "Planet.h"
#include "Player.h"

using namespace glm;
using namespace std;

Planet::~Planet() {
    m_tiles.clear();
    for (auto& row : m_layout) {
        row.clear();
    }
    m_layout.clear();
    m_dimensions = vec2(0);
    m_sea_level = -1;
}

Planet Planet::new_test() {
    Planet test;
    test.m_tiles.push_back(Tile(vec4(0.4, 0.7, 0.1, 0.8))); // grass
    test.m_tiles.push_back(Tile(vec4(0.65, 0.85, 1, 0.3))); // sky
    test.m_tiles.push_back(Tile(vec4(0.5, 0.2, 0.1, 0.5))); // dirt

    test.m_dimensions[0] = 28;
    test.m_dimensions[1] = round(2*M_PI*test.m_dimensions[0]);

    test.m_sea_level = 10;
    const int grass_level = 18;

    for (int r = 0; r < test.m_dimensions[0]; r++) {
        test.m_layout.emplace_back();
        for (int c = 0; c < test.m_dimensions[1]; c++) {
            test.m_layout.back().push_back(r >= grass_level ? 2 :
                                           r >= test.m_sea_level ? 0 : 1);
        }
    }

    test.m_gravity = vec2(0, -9.81);
    return test;
}

/// TODO: replace with a #define OFFSET or make some of these functions inline
vec2 Planet::getOffset() const {
    return vec2(-m_dimensions[1]*TILE_SIZE/2., m_sea_level*TILE_SIZE);   
}

vec2 Planet::getDimensions() const {
    return vec2(m_dimensions.y, m_dimensions.x) * TILE_SIZE;
}

vec2 Planet::getTileTopLeft(int r, int c) const {
    const vec2 offset = getOffset();
    return offset - (float)r*TILE_DY + (float)c*TILE_DX;
}

vec2 Planet::getTileCenter(int row, int col) const {
    const vec2 offset = getOffset();
    return offset - (float)row*TILE_DY + (float)col*TILE_DX + vec2(TILE_SIZE, -TILE_SIZE)/2.f;
}

b2Vec2 Planet::getGravity() const {
    return b2Vec2(m_gravity.x, m_gravity.y);
}

// TODO: Account for missing tiles
b2ChainShape Planet::createOutline() const {
    b2ChainShape chain_shape;

    vector<b2Vec2> vertices;
    for (int c = 0; c < m_dimensions[1]; c++) {
        const vec2 tl = getTileTopLeft(m_sea_level, c);
        vertices.emplace_back(tl.x, tl.y);
    }
    for (int r = m_sea_level; r < m_dimensions[0]; r++) {
        const vec2 tr = getTileTopLeft(r, m_dimensions[1]-1) + TILE_DX;
        vertices.emplace_back(tr.x, tr.y);
    }
    for (int c = 0; c < m_dimensions[1]; c++) {
        const vec2 br = getTileTopLeft(m_dimensions[0]-1, m_dimensions[1]-c-1) + TILE_DX - TILE_DY;
        vertices.emplace_back(br.x, br.y);
    }
    for (int r = m_sea_level; r < m_dimensions[0]; r++) {
        const vec2 bl = getTileTopLeft(r, 0) - TILE_DY;
        vertices.emplace_back(bl.x, bl.y);
    }
    
    chain_shape.CreateLoop(vertices.data(), vertices.size());
    return chain_shape;
}

float Planet::getRadius() const {
    return (m_dimensions[0] - m_sea_level)*TILE_SIZE;
}

float Planet::getHeight() const {
    return m_dimensions[0] * TILE_SIZE;
}

void Planet::add_to_world(b2World* world) {
    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position = b2Vec2(0, 0);
    m_body = world->CreateBody(&body_def);

    b2ChainShape chain_shape = createOutline();
    b2FixtureDef fixture_def;
    fixture_def.shape = &chain_shape;
    fixture_def.filter.categoryBits = PLANET_CATEGORY_BITS;
    m_body->CreateFixture(&fixture_def);
}

void Planet::render(nta::SpriteBatch& batch) const {
    const vec2 offset = getOffset();
    for (int r = 0; r < m_dimensions[0]; r++) {
        for (int c = 0; c < m_dimensions[1]; c++) {
            m_tiles[m_layout[r][c]].render(batch, offset - (float)r*TILE_DY + (float)c*TILE_DX);
        }
    }
}

/// TODO: Make lines below sea level DEBUG_BOX2D_AABB_COLOR
void Planet::render_debug(nta::DebugBatch& dbatch) const {
    return;
    static const size_t NUM_PIECES = 150;
    const vec2 offset = getOffset();

    for (int r = 0; r <= m_dimensions[0]; r++) {
        vec4 color = r == m_sea_level ? vec4(0,1,1,1) : vec4(1);
        vec2 left(offset - (float)r*TILE_DY), 
             right(offset - (float)r*TILE_DY + (float)m_dimensions[1]*TILE_DX);
        //render_line_in_pieces(pbatch, left, right, NUM_PIECES, color);
        dbatch.addLine(left, right, NUM_PIECES, color);
    }
    for (int c = 0; c <= m_dimensions[1]; c++) {
        vec2 top(offset + (float)c*TILE_DX),
             bottom(offset - (float)m_dimensions[0]*TILE_DY + (float)c*TILE_DX);
        dbatch.addLine(top, bottom, NUM_PIECES);
    }
}