#include <nta/Path.h>

#include "ChaiManager.h"

using namespace std;
using namespace glm;
using namespace nta;

void EffectComponent::use(const UpdateParams& params) {
    if (!utils::Path(m_effect).is_file()) return;

    ChaiParams cParams;
    cParams.id = m_system->get_owner(m_id);
    cParams.ecs = m_system;
    cParams.params = params;

    ChaiManager::eval_script(m_effect, cParams);
}