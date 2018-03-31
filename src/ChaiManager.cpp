#include "ChaiManager.h"

using namespace std;
using namespace chaiscript;
using namespace nta;

ChaiScript ChaiManager::m_chai;
map<string, string> m_scripts;

void ChaiManager::init() {
}

ChaiScript& ChaiManager::get_chai() {
    return m_chai;
}

string ChaiManager::get_script(crstring file_name) {
}

void ChaiManager::eval_snippet(crstring snippet) {
}

void ChaiManager::destroy() {
}