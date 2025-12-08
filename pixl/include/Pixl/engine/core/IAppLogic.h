#ifndef __IAPPLOGIC__
#define __IAPPLOGIC__

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct SceneObject {
    std::string meshId;
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation {0.0f, 0.0f, 0.0f}; 
    glm::vec3 scale    {1.0f, 1.0f, 1.0f};
};

class IAppLogic {

public:
    virtual ~IAppLogic() = default;

    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void draw() = 0;
    virtual void cleanup() = 0;

    virtual std::vector<SceneObject>& getSceneObjects() = 0;
    virtual int& getSelectedObjectIndex() = 0;
};

#endif
