#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include "Player.h"

class World {
private:
    Planet m_planet;
    // m_objects[0] == m_player
    std::vector<Object*> m_objects;
    // Not sure if having the player here is the best idea or not
    Player* m_player;
    b2World m_world;
    uint16_t m_flags; 
public:
    World(const Planet& planet);
    ~World();
    const Planet& get_planet() const;
    const Player* get_player() const;
    const b2World* get_b2World() const;
    void init();
    void add_object(Object* obj, const CreationParams& params);
    // return true if obj was removed
    bool remove_object(Object* obj);
    void set_flags(uint16_t flags);
    void unset_flags(uint16_t flags);
    void clear_flags();
    bool are_flags_set(uint16_t flags) const;
    void render(nta::SpriteBatch& batch, nta::SpriteBatch& overlay_batch,
                nta::SpriteBatch& light_batch, nta::SpriteFont* font) const;
    void render_debug(nta::DebugBatch& batch) const;
    // returns true if game over (player dead)
    bool update(UpdateParams& params);
    void destroy();

    // Probably don't need to make it a friend, but it's just easier this way
    friend WorldEditor;
};

#endif // WORLD_H_INCLUDED
