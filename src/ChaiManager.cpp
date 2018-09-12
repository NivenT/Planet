#include <nta/IOManager.h>
#include <nta/InputManager.h>

#include "ChaiManager.h"

using namespace glm;
using namespace std;
using namespace nta;

chaiscript::ChaiScript ChaiManager::m_chai;
map<string, function<void()>> ChaiManager::m_scripts;

chaiscript::ChaiScript& ChaiManager::get_chai() {
    return m_chai;
}

function<void()> ChaiManager::get_script(crstring file_name) {
    if (m_scripts.find(file_name) == m_scripts.end()) {
        string contents;
        IOManager::readFileToBuffer(file_name, contents);
        m_scripts[file_name] = eval_snippet<function<void()>>(contents);

    }
    return m_scripts[file_name];
}

void ChaiManager::destroy() {
    m_scripts.clear();
}

// TODO: Add everything else that'll be needed
// Note: There's gotta be a better way to do this
void ChaiManager::init() {
    Logger::writeToLog("Initializing ChaiManager...");
    Logger::indent();

    add_global_const(chaiscript::const_var(STANDING), "STANDING");
    add_global_const(chaiscript::const_var(RUNNING), "RUNNING");
    add_global_const(chaiscript::const_var(JUMPING), "JUMPING");
    add_global_const(chaiscript::const_var(FALLING), "FALLING");
    add(chaiscript::user_type<ObjectMotionState>(), "ObjectMotionState");

    // really gotta figure out this snake case v. camel case thing
    add(chaiscript::fun(&Planet::getCoord), "getCoord");
    add(chaiscript::fun(&Planet::getTile), "getTile");
    add(chaiscript::fun(&Planet::remove_tile), "remove_tile");
    add(chaiscript::fun(&Planet::getGravity), "getGravity");
    add(chaiscript::user_type<Planet>(), "Planet");

    add(chaiscript::fun(&UpdateParams::planet), "planet");
    add(chaiscript::fun(&UpdateParams::world), "world");
    add(chaiscript::fun(&UpdateParams::player_pos), "player_pos");
    add(chaiscript::user_type<UpdateParams>(), "UpdateParams");

    add(chaiscript::fun(&Tile::active), "active");
    add(chaiscript::fun(&Tile::solid), "solid");
    add(chaiscript::fun(&Tile::destructable), "destructable");
    add(chaiscript::user_type<Tile>(), "Tile");

    add(chaiscript::fun(&InputManager::isPressed), "isPressed");
    add(chaiscript::fun(&InputManager::justPressed), "justPressed");
    add(chaiscript::fun(&InputManager::justReleased), "justReleased");
    add(chaiscript::user_type<InputManager>(), "InputManager");

    add(chaiscript::constructor<Animation2D(crstring, int, size_t, size_t, float)>(), "Animation2D");
    add(chaiscript::constructor<Animation2D(crstring, crivec2, size_t, size_t, float)>(), "Animation2D");
    add(chaiscript::fun([](Animation2D& a, const Animation2D& b){a = b;}), "=");
    add(chaiscript::fun(&Animation2D::get_index), "get_index");
    add(chaiscript::fun(&Animation2D::get_start), "get_start");
    add(chaiscript::fun(&Animation2D::get_length), "get_length");
    add(chaiscript::fun(&Animation2D::get_frame_dims), "get_frame_dims");
    add(chaiscript::user_type<Animation2D>(), "Animation2D");

    add(chaiscript::fun(&ECS::gen_entity), "gen_entity");
    add(chaiscript::fun(&ECS::delete_entity), "delete_entity");
    add(chaiscript::fun(&ECS::delete_component), "delete_component");
    add(chaiscript::fun(&ECS::delete_components), "delete_components");
    add(chaiscript::fun(&ECS::has_component), "has_component");
    add(chaiscript::fun(&ECS::does_entity_exist), "does_entity_exist");
    add(chaiscript::fun(&ECS::get_owner), "get_owner");
    add(chaiscript::fun(&ECS::get_component_list), "get_component_list");
    add(chaiscript::fun(&ECS::get_components), "get_components");
    add(chaiscript::fun(&ECS::get_siblings), "get_siblings");
    add(chaiscript::fun(&ECS::get_component), "get_component");
    add(chaiscript::user_type<ECS>(), "ECS");

    add(chaiscript::user_type<Component>(), "Component");

    add(chaiscript::fun(&PhysicsComponent::getCenter), "getCenter");
    add(chaiscript::fun(&PhysicsComponent::getExtents), "getExtents");
    add(chaiscript::fun(&PhysicsComponent::getTopLeft), "getTopLeft");
    add(chaiscript::fun(&PhysicsComponent::getVelocity), "getVelocity");
    add(chaiscript::fun(&PhysicsComponent::getOrientation), "getOrientation");
    add(chaiscript::fun(&PhysicsComponent::getMass), "getMass");
    add(chaiscript::fun(&PhysicsComponent::is_standing), "is_standing");
    add(chaiscript::fun(&PhysicsComponent::applyForce), "applyForce");
    add(chaiscript::user_type<PhysicsComponent>(), "PhysicsComponent");
    m_chai.add(chaiscript::base_class<Component, PhysicsComponent>());

    add(chaiscript::fun(&HealthComponent::getHealth), "getHealth");
    add(chaiscript::fun(&HealthComponent::getMaxHealth), "getMaxHealth");
    add(chaiscript::user_type<HealthComponent>(), "HealthComponent");
    m_chai.add(chaiscript::base_class<Component, HealthComponent>());

    add(chaiscript::fun(&PickupComponent::getOwner), "getOwner");
    add(chaiscript::fun(&PickupComponent::is_picked_up), "is_picked_up");
    add(chaiscript::user_type<PickupComponent>(), "PickupComponent");
    m_chai.add(chaiscript::base_class<Component, PickupComponent>());

    add(chaiscript::fun(&AttackComponent::m_anim), "anim");
    add(chaiscript::fun(&AttackComponent::m_offset), "offset");
    add(chaiscript::fun(&AttackComponent::m_dims), "dims");
    add(chaiscript::fun(&AttackComponent::m_knockback), "knockback");
    add(chaiscript::fun(&AttackComponent::m_num_cycles), "num_cycles");
    add(chaiscript::fun(&AttackComponent::is_attacking), "is_attacking");
    add(chaiscript::user_type<AttackComponent>(), "AttackComponent");
    m_chai.add(chaiscript::base_class<Component, AttackComponent>());

    add(chaiscript::fun(&vec2::x), "x");
    add(chaiscript::fun(&vec2::y), "y");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return a-b;}), "-");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return a+b;}), "+");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return a*b;}), "*");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return a/b;}), "/");
    add(chaiscript::fun([](vec2& a, crvec2 b){return a=b;}), "=");
    add(chaiscript::constructor<vec2(float, float)>(), "vec2");
    add(chaiscript::constructor<vec2(const vec2&)>(), "vec2");
    add(chaiscript::fun(&ivec2::x), "x");
    add(chaiscript::fun(&ivec2::y), "y");
    add(chaiscript::constructor<vec2(int, int)>(), "ivec2");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return dot(a,b);}), "dot");

    add(chaiscript::fun([](crvec2 x){return normalize(x);}), "normalize");

    add(chaiscript::fun(&b2Vec2::x), "x");
    add(chaiscript::fun(&b2Vec2::y), "y");

    add_global_const(chaiscript::const_var(AGENT_STATE_SHOW_HEALTH), "AGENT_STATE_SHOW_HEALTH");
    add_global_const(chaiscript::const_var(AGENT_STATE_ATTACKING), "AGENT_STATE_ATTACKING");
    add_global_const(chaiscript::const_var(AGENT_SLOW_ACCELERATION), "AGENT_SLOW_ACCELERATION");
    add_global_const(chaiscript::const_var(AGENT_NORMAL_ACCELERATION), "AGENT_NORMAL_ACCELERATION");
    add_global_const(chaiscript::const_var(AGENT_FAST_ACCELERATION), "AGENT_FAST_ACCELERATION");
    add_global_const(chaiscript::const_var(PLAYER_EXTENTS), "PLAYER_EXTENTS");
    add_global_const(chaiscript::const_var(PLAYER_DIMS), "PLAYER_DIMS");
    add_global_const(chaiscript::const_var(PLAYER_WEAK_ATT_FORCE), "PLAYER_WEAK_ATT_FORCE");
    add_global_const(chaiscript::const_var(SMALL_ITEM_EXTENTS), "SMALL_ITEM_EXTENTS");
    add_global_const(chaiscript::const_var(TILE_SIZE), "TILE_SIZE");
    add_global_const(chaiscript::const_var(COMPONENT_SAVE_LIST_ID), "COMPONENT_SAVE_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_GRAPHICS_LIST_ID), "COMPONENT_GRAPHICS_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_PHYSICS_LIST_ID), "COMPONENT_PHYSICS_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_CONTROLLER_LIST_ID), "COMPONENT_CONTROLLER_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_INVENTORY_LIST_ID), "COMPONENT_INVENTORY_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_HEALTH_LIST_ID), "COMPONENT_HEALTH_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_ANIMATION_LIST_ID), "COMPONENT_ANIMATION_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_PICKUP_LIST_ID), "COMPONENT_PICKUP_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_COUNTDOWN_LIST_ID), "COMPONENT_COUNTDOWN_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_GARBAGE_LIST_ID), "COMPONENT_GARBAGE_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_EVENTQ_LIST_ID), "COMPONENT_EVENTQ_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_EFFECT_LIST_ID), "COMPONENT_EFFECT_LIST_ID");
    add_global_const(chaiscript::const_var(COMPONENT_ATTACK_LIST_ID), "COMPONENT_ATTACK_LIST_ID");

    Logger::unindent();
    Logger::writeToLog("Initialized ChaiManager");
}

void ChaiManager::eval_script(crstring file_name, const ChaiParams& params) {
    PhysicsComponent* physics = (PhysicsComponent*)params.ecs->get_component(params.id, COMPONENT_PHYSICS_LIST_ID);
    PickupComponent* pickup = (PickupComponent*)params.ecs->get_component(params.id, COMPONENT_PICKUP_LIST_ID);

    m_chai.set_global(chaiscript::var(params.params), "params");
    m_chai.set_global(chaiscript::var(params.ecs), "ecs");
    m_chai.set_global(chaiscript::var(params.id), "self");
    m_chai.set_global(chaiscript::var(physics),"physics");
    m_chai.set_global(chaiscript::var(pickup), "pickup");

    try {
        get_script(file_name)();
    } catch (const std::exception& e) {
        Logger::writeErrorToLog("Chaiscript error while running " + file_name + ": "
                                + e.what());
    }
}