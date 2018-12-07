#pragma once

#include "asg_render_predef.h"
#include "texture.h"
#include "glm/mat4x4.hpp"
#include <vector>
#include <variant>

namespace asg{

using glm::mat4;
using glm::vec3;

class UniformHandler;

struct UniformData{
    UniformData(const std::string& name)
        : name(name)
    {}
    std::string name;
    std::variant<float
               , mat4
               , std::shared_ptr<Texture>> data;
    Rint gl_id = -1;
};

class UniformStorage{
public:
    void apply(Ruint program);
    UniformHandler add(const std::string& name);

private: friend UniformHandler;
    void prepareGL(Ruint program);

    std::vector<UniformData> data;
    bool update_gl = true;
};

class UniformHandler{
public:
    UniformHandler()
        : location(0)
        , storage(nullptr)
    {}
    UniformHandler(size_t location, UniformStorage* storage)
        : location(location)
        , storage(storage)
    { }
public:
    template<typename T>
    void set(T&& value){
        storage->data[location].data = value;
    }
private:
    size_t location;
    UniformStorage* storage;
};

}
