#ifndef SPAWNER_H_INCLUDED
#define SPAWNER_H_INCLUDED

#include "Enemy.h"

/*
    CreationParams enemy_params;
    enemy_params.planet = &m_planet;
    enemy_params.position = m_planet.getTileCenter(6, 130);
    enemy_params.extents = ENEMY_UNIT_EXTENTS * vec2(3,1);
    Enemy* test_enemy = new Enemy("resources/images/shoe.png", "scripts/shoe.chai");
    test_enemy->add_to_world(m_world.get(), enemy_params);
    enemy_params.position = m_planet.getTileCenter(6, 100);
    Spawner* test_spawner = new Spawner("resources/images/shoe_spawner.png", *test_enemy,
                                        m_objects);
    test_spawner->set_creation_params(enemy_params);
    test_spawner->add_to_world(m_world.get(), enemy_params);
*/

struct SpawnerParams : public EnemyParams {
    SpawnerParams() {
    }
    nta::utils::Json json() {
        return EnemyParams::json().merge({
            {"spawn", spawn.json()},
            {"spawn_rate", spawn_rate}
        });
    }

    EnemyParams spawn;
    float spawn_rate = SPAWNER_SLOW_RATE;
};

class Spawner : public Enemy {
private:
    void applyForce(float x, float y) {}
    
    const Enemy m_spawn;
    /// \todo Replace with signal
    std::vector<Object*>& m_objs;
    CreationParams m_params;
    float m_spawn_rate;
    float m_time;
public:
    Spawner(crstring texture, Enemy spawn, std::vector<Object*>& objs,
            float health = NORMAL_SPAWNER_INIT_HEALTH, crvec4 color = glm::vec4(1), 
            uint16_t type = 0);
    Spawner(const SpawnerParams& params, std::vector<Object*>& objs);
    void set_creation_params(const CreationParams& params);
    void set_spawn_rate(float rate);
    void add_to_world(b2World* world, const CreationParams& params);
    void update(const UpdateParams& params);
};

#endif // SPAWNER_H_INCLUDED