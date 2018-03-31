#ifndef CHAIMANAGER_H_INCLUDED
#define CHAIMANAGER_H_INCLUDED

#include <chaiscript/chaiscript.hpp>

#include <nta/MyEngine.h>

class ChaiManager {
private:
    static chaiscript::ChaiScript m_chai;
    static std::map<std::string, std::string> m_scripts;
public:
    static void init();
    static chaiscript::ChaiScript& get_chai();
    static std::string get_script(crstring file_name);
    template<typename T>
    static void eval_script(crstring file_name, T self);
    static void eval_snippet(crstring snippet);
    static void destroy();
};

template<typename T>
void ChaiManager::eval_script(crstring file_name, T self) {
}

#endif // CHAIMANAGER_H_INCLUDED