#ifndef CHAIMANAGER_H_INCLUDED
#define CHAIMANAGER_H_INCLUDED

#include <chaiscript/chaiscript.hpp>

#include <nta/Logger.h>

class ChaiManager {
private:
    static chaiscript::ChaiScript m_chai;
    static std::map<std::string, std::function<void()>> m_scripts;
public:
    static void init();
    template<typename T>
    static void add(T data, crstring name);
    template<typename T>
    static void add_global_const(T data, crstring name);
    template<typename T>
    static void add_global(T data, crstring name);
    static chaiscript::ChaiScript& get_chai();
    /// Script must end in the name of a function
    static std::function<void()> get_script(crstring file_name);
    template<typename T, typename S>
    static void eval_script(crstring file_name, T self, S params);
    template<typename T>
    static T eval_snippet(crstring snippet);
    static void destroy();
};

template<typename T>
void ChaiManager::add(T data, crstring name) {
    nta::Logger::writeToLog("Adding " + name + " to ChaiManager");
    m_chai.add(data, name);
}

template<typename T>
void ChaiManager::add_global(T data, crstring name) {
    nta::Logger::writeToLog("Adding global " + name + " to ChaiManager");
    m_chai.add_global(data, name);
}

template<typename T>
void ChaiManager::add_global_const(T data, crstring name) {
    nta::Logger::writeToLog("Adding const global " + name + " to ChaiManager");
    m_chai.add_global_const(data, name);
}

template<typename T>
T ChaiManager::eval_snippet(crstring snippet) {
    try {
        return m_chai.eval<T>(snippet);
    } catch (const std::exception& e) {
        nta::Logger::writeErrorToLog("Chaiscript error: " + std::string(e.what()));
    }
}

template<typename T, typename S>
void ChaiManager::eval_script(crstring file_name, T self, S params) {
    m_chai.set_global(self, "self");
    m_chai.set_global(params, "params");
    try {
        get_script(file_name)();
    } catch (const std::exception& e) {
        nta::Logger::writeErrorToLog("Chaiscript error while running " + file_name + ": " + 
                                     std::string(e.what()));
    }
}


#endif // CHAIMANAGER_H_INCLUDED