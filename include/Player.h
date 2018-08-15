#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Agent.h"
#include "Light.h"
#include "Item.h"
#include "Cycle.h"

class Player : public Agent {
public:
    struct AttackAnim {
        nta::Animation2D anim;
        // difference between player top_left and animation top_left
        glm::vec2 offset;
        glm::vec2 dims;
        float speed;
        int num_cycles = 1;
        float knockback = 0;
    };
private:
    void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);
    void handle_input(const UpdateParams& params);

    Cycle<Item*> m_inventory;
    AttackAnim m_attack_anim;
    uint64_t m_inventory_event_id;
public:
    Player(uint16_t type = 0);
    ~Player();
    // passkey pattern doesn't mix well with ChaiScript so next best thing
    AttackAnim* get_attack_anim(Object* requester);
    glm::vec2 getExtents() const;
    float get_attack_strength();
    void set_attacking(Object* requester);
    void add_to_world(b2World* world, const CreationParams& params);
    void render_inventory(nta::SpriteBatch& batch, nta::SpriteFont* font) const;
    void render_attack(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);
};

#endif // PLAYER_H_INCLUDED