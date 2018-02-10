#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Agent.h"
#include "Light.h"
#include "Tile.h"

class Player : public Agent {
public:
    Player();
    ~Player();
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);
};

#endif // PLAYER_H_INCLUDED