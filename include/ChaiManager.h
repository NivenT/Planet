#ifndef CHAIMANAGER_H_INCLUDED
#define CHAIMANAGER_H_INCLUDED

#include <chaiscript/chaiscript.hpp>

#include <nta/Logger.h>

class ChaiManager {
private:
    static chaiscript::ChaiScript m_chai;
    static std::map<std::string, std::string> m_scripts;
public:
    static void init();
    template<typename T>
    static void add(T data, crstring name);
    static chaiscript::ChaiScript& get_chai();
    static std::string get_script(crstring file_name);
    template<typename T>
    static void eval_script(crstring file_name, T self);
    static void eval_snippet(crstring snippet);
    static void destroy();
};

template<typename T>
void ChaiManager::add(T data, crstring name) {
    nta::Logger::writeToLog("Adding " + name + " to ChaiManager");
    m_chai.add(data, name);
}

template<typename T>
void ChaiManager::eval_script(crstring file_name, T self) {
    m_chai.add(self, "self");
    eval_snippet(get_script(file_name));
}

#endif // CHAIMANAGER_H_INCLUDED