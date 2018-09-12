#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include "components.h"

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
    void save(nta::crstring path) {
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

// Temporary class. Will replace World in the future
class World {
private:
    void add_player();

    Planet m_planet;
    b2World m_world;

    nta::ECS m_ecs;
    nta::EntityID m_player;
public:
    World(const WorldParams& params, bool player = true);
    ~World();

    const Planet& get_planet() const { return m_planet; }
    const b2World* get_b2World() const { return &m_world; }
    glm::vec2 get_player_center() const;

    void render(nta::SpriteBatch& batch, nta::SpriteBatch& overlay_batch,
                nta::SpriteFont* font) const;
    void render_debug(nta::DebugBatch& batch) const;

    // returns true if game over (player dead)
    bool update(UpdateParams& params);
};

#endif // WORLD_H_INCLUDED
