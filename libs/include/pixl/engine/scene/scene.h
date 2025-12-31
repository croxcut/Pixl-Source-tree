#ifndef SCENE_H
#define SCENE_H

#include <unordered_map>

#include "../renderer/renderer.h"
#include "../renderer/render_objects.h"

struct SceneObject {
    std::vector<Shader> shader;
    std::vector<Mesh> mesh;
};

class Scene {
public:
    virtual void init() = 0;
    virtual void tick(const f32& dt) = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
};

#endif