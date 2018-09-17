#include "World.h"

using namespace std;
using namespace glm;
using namespace nta;

/// New World Stuff
World::World(const WorldParams& params, bool player) : m_world(params.planet.getGravity()) {
    m_planet = params.planet;
    m_planet.add_to_world(&m_world);
    m_ecs.add_component<GarbageComponent>(m_ecs.gen_entity());
    m_ecs.add_component<EventQueueComponent>(m_ecs.gen_entity(), &m_world);
    
    for (auto& item : params.items) {
        Entity id = m_ecs.gen_entity();

        m_ecs.add_component<PickupComponent>(id);
        m_ecs.add_component<TextureComponent>(id, item.tex, item.color);
        m_ecs.add_component<SensorPhysicsComponent>(id, item.max_speed);
        m_ecs.add_component<EffectComponent>(id, item.use_script);

        /// \todo Store ComponentID from above call to add_component instead
        m_ecs.add_component_to_list<GraphicsComponent>(m_ecs.get_component<TextureComponent>(id).unwrap().get_id());

        SensorPhysicsComponent& physics = m_ecs.get_component<SensorPhysicsComponent>(id).unwrap();
        m_ecs.add_component_to_list<PhysicsComponent>(physics.get_id());
        physics.add_to_world(&m_world, item, id);

        vec2 tl = physics.getTopLeft(), cen = physics.getCenter();
        float ang = physics.getOrientation();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &item.extents), id);
    }
    for (auto& enemy : params.enemies) {
        Entity id = m_ecs.gen_entity();

        m_ecs.add_component<AnimationComponent>(id, enemy.tex, enemy.anim_dims,
                                                enemy.anims, enemy.color);
        m_ecs.add_component<PhysicsComponent>(id, enemy.max_speed);
        m_ecs.add_component<HealthComponent>(id, enemy.init_health, ENEMY_HEALTH_CATEGORY,
                                             ENEMY_HEALTH_MASK, ENEMY_HEALTH_COLOR);
        m_ecs.add_component<ScriptComponent>(id, enemy.update_script);

        m_ecs.add_component_to_list<GraphicsComponent>(m_ecs.get_component<AnimationComponent>(id).unwrap().get_id());
        m_ecs.add_component_to_list<CountdownComponent>(m_ecs.get_component<HealthComponent>(id).unwrap().get_id());
        m_ecs.add_component_to_list<ControllerComponent>(m_ecs.get_component<ScriptComponent>(id).unwrap().get_id());

        PhysicsComponent& physics = m_ecs.get_component<PhysicsComponent>(id).unwrap();
        physics.add_to_world(&m_world, enemy, id);

        vec2 tl = physics.getTopLeft(), cen = physics.getCenter();
        float ang = physics.getOrientation();
        float mass = physics.getMass();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &enemy.extents), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), id);
    }
    for (auto& spawner : params.spawners) {
        Entity id = m_ecs.gen_entity();

        m_ecs.add_component<AnimationComponent>(id, spawner.tex, spawner.anim_dims,
                                                spawner.anims, spawner.color);
        m_ecs.add_component<SensorPhysicsComponent>(id, spawner.max_speed);
        m_ecs.add_component<HealthComponent>(id, spawner.init_health, ENEMY_HEALTH_CATEGORY,
                                             ENEMY_HEALTH_MASK, ENEMY_HEALTH_COLOR);
        m_ecs.add_component<SpawnerComponent>(id, spawner.spawn_rate*TARGET_FPS,
                                              EnemyParams::load(utils::Json::from_file(spawner.spawn)));

        m_ecs.add_component_to_list<GraphicsComponent>(m_ecs.get_component<AnimationComponent>(id).unwrap().get_id());
        m_ecs.add_component_to_list<CountdownComponent>(m_ecs.get_component<HealthComponent>(id).unwrap().get_id());
        m_ecs.add_component_to_list<CountdownComponent>(m_ecs.get_component<SpawnerComponent>(id).unwrap().get_id());

        SensorPhysicsComponent& physics = m_ecs.get_component<SensorPhysicsComponent>(id).unwrap();
        m_ecs.add_component_to_list<PhysicsComponent>(physics.get_id());
        physics.add_to_world(&m_world, spawner, id);

        vec2 tl = physics.getTopLeft(), cen = physics.getCenter();
        float ang = physics.getOrientation();
        float mass = physics.getMass();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &spawner.extents), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), id);
    }
    if (player) {
        add_player();
    }
}

World::~World() {
}

vec2 World::get_player_center() const {
    return m_ecs.get_component<PhysicsComponent>(m_player).unwrap().getCenter();
}

void World::add_player() {
    CreationParams params;
    params.position = PLAYER_INIT_POS;
    params.extents = PLAYER_EXTENTS;
    params.max_speed = PLAYER_MAX_SPEED;
    params.density = PLAYER_DENSITY;
    params.friction = PLAYER_FRICTION;
    params.restitution = PLAYER_RESTITUTION;
    MotionAnimation temp[] = PLAYER_MOTION_ANIMATIONS;

    m_player = m_ecs.gen_entity();

    m_ecs.add_component<PlayerAnimationComponent>(m_player, PLAYER_ANIMATION_FILE,
                                                  PLAYER_ANIMATION_DIMS, temp);
    m_ecs.add_component<PhysicsComponent>(m_player, params.max_speed);
    m_ecs.add_component<PlayerControllerComponent>(m_player);
    m_ecs.add_component<HealthComponent>(m_player, PLAYER_INIT_HEALTH,
                                         PLAYER_HEALTH_CATEGORY, PLAYER_HEALTH_MASK,
                                         PLAYER_HEATLH_COLOR);
    m_ecs.add_component<InventoryComponent>(m_player);
    m_ecs.add_component<AttackComponent>(m_player);

    m_ecs.add_component_to_list<GraphicsComponent>(m_ecs.get_component<PlayerAnimationComponent>(m_player).unwrap().get_id());
    m_ecs.add_component_to_list<AnimationComponent>(m_ecs.get_component<PlayerAnimationComponent>(m_player).unwrap().get_id());
    m_ecs.add_component_to_list<ControllerComponent>(m_ecs.get_component<PlayerControllerComponent>(m_player).unwrap().get_id());
    m_ecs.add_component_to_list<CountdownComponent>(m_ecs.get_component<HealthComponent>(m_player).unwrap().get_id());
    m_ecs.add_component_to_list<CountdownComponent>(m_ecs.get_component<InventoryComponent>(m_player).unwrap().get_id());

    auto& physics = m_ecs.get_component<PhysicsComponent>(m_player).unwrap();
    physics.add_to_world(&m_world, params, m_player);

    float mass = physics.getMass();
    m_ecs.broadcast(Message(MESSAGE_TOGGLE_SHOW_HEALTH_BAR), m_player);
    m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &params.extents), m_player);
    m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), m_player);
}

void World::render(SpriteBatch& batch, SpriteBatch& overlay_batch, 
                      SpriteFont* font) const {
    m_planet.render(batch);
    for (auto graphics : m_ecs.get_component_list<GraphicsComponent>()) {
        graphics->render(batch);
    }
    for (auto health : m_ecs.get_component_list<HealthComponent>()) {
        health->render(batch);
    }
    for (auto inventory : m_ecs.get_component_list<InventoryComponent>()) {
        inventory->render(overlay_batch, font);
    }
    for (auto attack : m_ecs.get_component_list<AttackComponent>()) {
        attack->render(batch);
    }
}

void World::render_debug(DebugBatch& batch) const {
    m_planet.render_debug(batch);
    for (auto graphics : m_ecs.get_component_list<GraphicsComponent>()) {
        graphics->render_debug(batch);
    }
}

bool World::update(UpdateParams& params) {
    params.planet = &m_planet;
    params.world = &m_world;
    params.player_pos = get_player_center();

    for (auto controller : m_ecs.get_component_list<ControllerComponent>()) {
        controller->act(params);
    }
    for (auto physics : m_ecs.get_component_list<PhysicsComponent>()) {
        physics->update(params);
    }
    for (auto animation : m_ecs.get_component_list<AnimationComponent>()) {
        animation->step(params.dt);
    }
    for (auto attack : m_ecs.get_component_list<AttackComponent>()) {
        attack->step(params.dt);
    }
    for (auto count : m_ecs.get_component_list<CountdownComponent>()) {
        count->countdown();
    }
    for (auto queue : m_ecs.get_component_list<EventQueueComponent>()) {
        queue->process();
    }
    for (auto garbage : m_ecs.get_component_list<GarbageComponent>()) {
        garbage->dump();
    }
    m_world.Step(params.dt, 6, 2);

    // Should prbably ensure that m_player has a PlayerControllerComponent
    return !m_ecs.does_entity_exist(m_player);
}
