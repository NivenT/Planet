#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Agent.h"
#include "Light.h"
#include "Tile.h"

// TODO: Order these sensibly
#define PLAYER_COLOR glm::vec4(0.24, 0.00, 0.40, 1.00)
#define PLAYER_INIT_HEALTH 100.f
#define PLAYER_DIMS glm::vec2(TILE_SIZE/2.f, TILE_SIZE)
#define PLAYER_AREA (PLAYER_DIMS.x*PLAYER_DIMS.y)
#define PLAYER_HALF_DIMS (PLAYER_DIMS/2.0f)
#define PLAYER_EXTENTS PLAYER_HALF_DIMS
#define PLAYER_INIT_POS b2Vec2(0, PLAYER_DIMS.y*1.1)
#define PLAYER_DENSITY 1.2020569f
#define PLAYER_FRICTION 0.57721f
#define PLAYER_MASS (PLAYER_DENSITY*PLAYER_AREA)
#define PLAYER_FORCE (50.f*PLAYER_MASS)
#define PLAYER_JUMP_FORCE (PLAYER_FORCE*10.f)

class Player : public Agent {
public:
    Player();
    ~Player();
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);
};

#endif // PLAYER_H_INCLUDED