#ifndef SPAWNER_H_INCLUDED
#define SPAWNER_H_INCLUDED

#include <nta/Observer.h>

#include "Enemy.h"

struct SpawnerParams : public EnemyParams {
    SpawnerParams() {
        init_health = NORMAL_SPAWNER_INIT_HEALTH;
    }
    SpawnerParams(const EnemyParams& super) : EnemyParams(super) {}
    nta::utils::Json json() const {
        return EnemyParams::json().merge({
            {"spawn", spawn.json()},
            {"spawn_rate", spawn_rate}
        });
    }
    static SpawnerParams load(const nta::utils::Json& json) {
        SpawnerParams ret(EnemyParams::load(json));
        ret.spawn = EnemyParams::load(json["spawn"]);
        ret.spawn_rate = json["spawn_rate"];
        return ret;
    }

    EnemyParams spawn;
    float spawn_rate = SPAWNER_SLOW_RATE;
};

// Dang, when was the last time I needed to inherit from two classes in one of my projects?
class Spawner : public Enemy, public nta::Subject {
private:
    void applyForce(float x, float y) {}
    
    EnemyParams m_spawn;
    float m_spawn_rate;
    float m_time;
public:
    Spawner(crstring texture, EnemyParams spawn, float health = NORMAL_SPAWNER_INIT_HEALTH, 
            crvec4 color = glm::vec4(1), uint16_t type = 0);
    Spawner(const SpawnerParams& params);
    void set_spawn_rate(float rate);
    void add_to_world(b2World* world, const CreationParams& params);
    void update(const UpdateParams& params);
};

#endif // SPAWNER_H_INCLUDED