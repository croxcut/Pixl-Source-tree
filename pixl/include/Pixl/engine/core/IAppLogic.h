#ifndef __IAPPLOGIC__
#define __IAPPLOGIC__

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "flow.h"
#include "../util/types.h"

struct SceneObject {
    u64 meshId;
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation {0.0f, 0.0f, 0.0f}; 
    glm::vec3 scale    {1.0f, 1.0f, 1.0f};
};

class IAppLogic : public Flow{

public:
    virtual ~IAppLogic() = default;
};

#endif
