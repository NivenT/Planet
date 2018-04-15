#include <nta/IOManager.h>
#include <nta/InputManager.h>

#include "ChaiManager.h"
#include "Enemy.h"
#include "Item.h"
#include "Player.h"

using namespace std;
using namespace chaiscript;
using namespace nta;

ChaiScript ChaiManager::m_chai;
map<string, function<void()>> ChaiManager::m_scripts;

ChaiScript& ChaiManager::get_chai() {
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

    add(chaiscript::fun(&Enemy::m_max_speed), "m_max_speed");
    add(chaiscript::fun(&Enemy::m_max_speed), "max_speed");
    // this isn't strictly necessary but makes error messages nicer
    add(chaiscript::user_type<Enemy>(), "Enemy");

    add(chaiscript::fun(&Agent::getHealth), "getHealth");
    add(chaiscript::fun(&Agent::applyDamage), "applyDamage");
    add(chaiscript::fun(&Agent::m_is_standing), "m_is_standing");
    add(chaiscript::fun(&Agent::m_is_standing), "is_standing");
    add(chaiscript::user_type<Agent>(), "Agent");

    add(chaiscript::fun(&Object::getObjectType), "getObjectType");
    add(chaiscript::fun(&Object::getCenter), "getCenter");
    add(chaiscript::fun(&Object::getOrientation), "getOrientation");
    add(chaiscript::fun(&Object::getVelocity), "getVelocity");
    add(chaiscript::fun(&Object::getTopLeft), "getTopLeft");
    add(chaiscript::fun(&Object::getExtents), "getExtents");
    add(chaiscript::fun(&Object::getMass), "getMass");
    add(chaiscript::fun(&Object::setVelocity), "setVelocity");
    add(chaiscript::fun(&Object::applyForce), "applyForce");
    add(chaiscript::fun(&Object::m_body), "m_body");
    //add(chaiscript::user_type<Object>(), "Object"); 

    m_chai.add(chaiscript::base_class<Object, Agent>());
    m_chai.add(chaiscript::base_class<Object, Enemy>());
    m_chai.add(chaiscript::base_class<Agent, Enemy>());

    add(chaiscript::fun(&Item::m_equipped), "m_equipped");
    add(chaiscript::fun(&Item::m_equipped), "equipped");
    add(chaiscript::fun(&Item::m_owner), "m_owner");
    add(chaiscript::fun(&Item::m_owner), "owner");
    add(chaiscript::fun(&Item::m_planet), "m_planet");
    add(chaiscript::fun(&Item::m_planet), "planet");
    add(chaiscript::user_type<Item>(), "Item");

    m_chai.add(chaiscript::base_class<Object, Item>());

    m_chai.add(chaiscript::user_type<Player>(), "Player");
    m_chai.add(chaiscript::base_class<Object, Player>());
    m_chai.add(chaiscript::base_class<Agent, Player>());

    // really gotta figure out this snake case v. camel case thing
    add(chaiscript::fun(&Planet::getCoord), "getCoord");
    add(chaiscript::fun(&Planet::getTile), "getTile");
    add(chaiscript::fun(&Planet::remove_tile), "remove_tile");
    add(chaiscript::user_type<Planet>(), "Planet");

    add(chaiscript::fun(&UpdateParams::planet), "planet");
    add(chaiscript::fun(&UpdateParams::world), "world");
    add(chaiscript::user_type<UpdateParams>(), "UpdateParams");

    add(chaiscript::fun(&Tile::active), "active");
    add(chaiscript::user_type<Tile>(), "Tile");

    add(chaiscript::fun(&InputManager::isPressed), "isPressed");
    add(chaiscript::fun(&InputManager::justPressed), "justPressed");
    add(chaiscript::fun(&InputManager::justReleased), "justReleased");
    add(chaiscript::user_type<InputManager>(), "InputManager");

    add(chaiscript::fun(&glm::vec2::x), "x");
    add(chaiscript::fun(&glm::vec2::y), "y");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return a-b;}), "-");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return a+b;}), "+");
    add(chaiscript::constructor<glm::vec2(float, float)>(), "vec2");
    add(chaiscript::constructor<glm::vec2(const glm::vec2&)>(), "vec2");
    add(chaiscript::fun(&glm::ivec2::x), "x");
    add(chaiscript::fun(&glm::ivec2::y), "y");
    add(chaiscript::constructor<glm::vec2(int, int)>(), "ivec2");
    add(chaiscript::fun([](crvec2 a, crvec2 b){return glm::dot(a,b);}), "dot");

    add_global_const(chaiscript::const_var(AGENT_SLOW_ACCELERATION), "AGENT_SLOW_ACCELERATION");
    add_global_const(chaiscript::const_var(AGENT_NORMAL_ACCELERATION), "AGENT_NORMAL_ACCELERATION");
    add_global_const(chaiscript::const_var(AGENT_FAST_ACCELERATION), "AGENT_FAST_ACCELERATION");
    add_global_const(chaiscript::const_var(PLAYER_EXTENTS), "PLAYER_EXTENTS");
    add_global_const(chaiscript::const_var(TILE_SIZE), "TILE_SIZE");

    Logger::unindent();
    Logger::writeToLog("Initialized ChaiManager");
}