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
void ChaiManager::init() {
    Logger::writeToLog("Initializing ChaiManager...");
    Logger::indent();

    add(chaiscript::fun(&Agent::getHealth), "getHealth");
    add(chaiscript::fun(&Agent::applyDamage), "applyDamage");
    // this isn't strictly necessary but makes error messages nicer
    add(chaiscript::user_type<Agent>(), "Agent");

    add(chaiscript::fun(&Object::getObjectType), "getObjectType");
    add(chaiscript::fun(&Object::getCenter), "getCenter");
    add(chaiscript::fun(&Object::getOrientation), "getOrientation");
    add(chaiscript::fun(&Object::getVelocity), "getVelocity");
    add(chaiscript::fun(&Object::getTopLeft), "getTopLeft");
    add(chaiscript::fun(&Object::getExtents), "getExtents");
    //add(chaiscript::user_type<Object>(), "Object"); 

    Logger::unindent();
    Logger::writeToLog("Initialized ChaiManager");
}