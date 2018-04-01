#include <nta/IOManager.h>

#include "ChaiManager.h"
#include "Enemy.h"

using namespace std;
using namespace chaiscript;
using namespace nta;

ChaiScript ChaiManager::m_chai;
map<string, string> ChaiManager::m_scripts;

ChaiScript& ChaiManager::get_chai() {
    return m_chai;
}

string ChaiManager::get_script(crstring file_name) {
    if (m_scripts.find(file_name) == m_scripts.end()) {
        IOManager::readFileToBuffer(file_name, m_scripts[file_name]);
    }
    return m_scripts[file_name];
}

void ChaiManager::eval_snippet(crstring snippet) {
    m_chai.eval(snippet);
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

    m_chai.add(chaiscript::base_class<Object, Enemy>());
    m_chai.add(chaiscript::base_class<Agent, Enemy>());

    add(chaiscript::fun(&UpdateParams::planet), "planet");
    add(chaiscript::fun(&UpdateParams::world), "world");
    add(chaiscript::user_type<UpdateParams>(), "UpdateParams");

    add(chaiscript::fun(&glm::vec2::x), "x");
    add(chaiscript::fun(&glm::vec2::y), "y");
    add(chaiscript::constructor<glm::vec2(float, float)>(), "vec2");
    add(chaiscript::constructor<glm::vec3(float, float, float)>(), "vec3");
    add(chaiscript::constructor<glm::vec4(float, float, float, float)>(), "vec4");

    add_global_const(chaiscript::const_var(AGENT_SLOW_ACCELERATION), "AGENT_SLOW_ACCELERATION");
    add_global_const(chaiscript::const_var(AGENT_NORMAL_ACCELERATION), "AGENT_NORMAL_ACCELERATION");
    add_global_const(chaiscript::const_var(AGENT_FAST_ACCELERATION), "AGENT_FAST_ACCELERATION");

    Logger::unindent();
    Logger::writeToLog("Initialized ChaiManager");
}