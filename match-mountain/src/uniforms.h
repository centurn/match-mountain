#pragma once

#include "asg_render_predef.h"
#include "texture.h"
#include <vector>
#include <variant>

namespace asg{

class UniformHandler;

struct UniformData{
    UniformData(const std::string& name)
        : name(name)
    {}
    std::string name;
    std::variant<float
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
    bool update_gl = false;
};

class UniformHandler{
private: friend UniformStorage;
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
