#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include "Spawner.h"
#include "Item.h"
#include "Player.h"

struct WorldParams {
    void clear() {
        items.clear();
        enemies.clear();
        spawners.clear();
        planet.clear();
    }
    nta::utils::Json json() const {
        nta::utils::Json ret = {
            {"planet", planet.json()}
        };
        for (const auto& item : items) {
            ret["items"].push_back(item.json());
        }
        for (const auto& enemy : enemies) {
            ret["enemies"].push_back(enemy.json());
        }
        for (const auto& spawner : spawners) {
            ret["spawners"].push_back(spawner.json());
        }
        return ret;
    }
    void save(crstring path) {
        nta::IOManager::writeFileFromBuffer(path, json().dump(2));
    }
    static WorldParams load(const nta::utils::Json& json) {
        WorldParams ret;
        ret.planet = Planet::load(json["planet"]);
        for (const auto& item : json["items"]) {
            ret.items.push_back(ItemParams::load(item));
        }
        for (const auto& enemy : json["enemies"]) {
            ret.enemies.push_back(EnemyParams::load(enemy));
        }
        for (const auto& spawner : json["spawners"]) {
            ret.spawners.push_back(SpawnerParams::load(spawner));
        }
        return ret;
    }

    std::vector<ItemParams> items;
    std::vector<EnemyParams> enemies;
    std::vector<SpawnerParams> spawners;
    Planet planet;
};

class World : public nta::Observer {
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
    World(const WorldParams& params);
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
                nta::SpriteFont* font) const;
    void render_debug(nta::DebugBatch& batch) const;
    // returns true if game over (player dead)
    bool update(UpdateParams& params);
    void destroy();

    void onNotify(const nta::Event&);
};

#endif // WORLD_H_INCLUDED
