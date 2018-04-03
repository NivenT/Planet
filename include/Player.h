#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Agent.h"
#include "Light.h"
#include "Tile.h"
#include "Item.h"
#include "Cycle.h"

class Player : public Agent {
private:
    void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);
    void handle_input(const UpdateParams& params);

    Cycle<Item*> m_inventory;
    uint64_t m_inventory_event_id;
public:
    Player(uint16_t type = 0);
    ~Player();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void render_inventory(nta::SpriteBatch& batch, nta::SpriteFont* font) const;
    void update(const UpdateParams& params);
};

#endif // PLAYER_H_INCLUDED