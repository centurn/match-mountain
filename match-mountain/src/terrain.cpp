#include "terrain.h"

#include "import_hgt.h"
#include "attrib_buffer.h"
#include "asg_perf.h"
#include "asg_storage.h"

#include "glm/gtc/matrix_transform.hpp"

using namespace asg;
using namespace geo;

Terrain::Terrain(const char* image_filename)
{
    initialize(image_filename);
    // TODO: should I handle the object state after fail of init with default data
    // ...or perhaps just guarantee it never happens?
}

Terrain::~Terrain()
{
}

void Terrain::initialize(const char *image_filename)
{
    previous_filename = image_filename;
    try{
        auto new_image = std::make_unique<BackgroundImage>(image_filename);
        Position pos = new_image->getTexture()->getBitmap().getLocation();
        VALIDATE(pos.isValid());
        auto new_terra = std::make_unique<Heightmap>(pos);

        // Should be no more exceptions after this point. Swap to new state

        ref_image = std::move(new_image);
        terra = std::move(new_terra);
        initial_vfov = ref_image->getTexture()->getBitmap().getVfov();
        vfov = initial_vfov;
        eye_pos = terra->initialPos();
        ref_image->fitScreen(float(width)/height);
    }catch(const std::exception e){
        log_e("Failed to switch to new image. Reason: %s\n", e.what());
    }
}

void Terrain::resize(glm::ivec2 size)
{
    AppletBase::resize(size);
    ref_image->fitScreen(float(width)/height);
}

void Terrain::render()
{
    mat4 projection = glm::perspective(vfov
                                      , float(width)/height
                                      , 2.0f
                                      , float(Heightmap::min_extent)*1.5f);
    mat4 viewproj = projection * getCamRotation() * glm::translate(mat4(1), -eye_pos);
    terra->render(viewproj, animateLight());
    if(ref_image_enabled){
        ref_image->render();
    }
}

void Terrain::mouseWheel(glm::ivec2 delta)
{
    vfov *= 1.0f + (-delta.y * 0.02f);
    vfov = glm::clamp(vfov, glm::radians(5.f), glm::radians(120.f));
}

void Terrain::mouseMove(glm::ivec2 pos, glm::ivec2 delta, uint pressed_mask)
{
    if(pressed_mask & 1){// Left Mouse Button
        rotation_cam += glm::vec2(hfov()*(float(-delta.x)/width)
                             , vfov*(float(-delta.y)/height));
    }
    if((pressed_mask & (1 << 2)) && ref_image_enabled ){// Right Mouse button
        //Rotate image around center
        vec2 tangent = glm::ivec2(width/2, height/2) - pos;
        tangent = glm::normalize(vec2{-tangent.y, tangent.x});
        float rotation = glm::dot(tangent, vec2(delta)) * glm::pi<float>() / width;
        ref_image->setRotation(ref_image->getRotation() + rotation);
    }
}

void Terrain::keyDown(int virtual_keycode)
{
//    log_d("%c", virtual_keycode);
    constexpr float v = 10;
    auto direction = [this](){
        auto cam = glm::inverse(getCamRotation()) * glm::vec4(0, 0, -1, 0);
        return glm::normalize(vec3(cam.x, cam.y, 0));
    };
    switch(virtual_keycode){
    case 'e':// Up
        eye_pos += vec3(0, 0, v);
        break;
    case 'q':// Down
        eye_pos += vec3(0, 0, -v);
        break;
    case 'd':{// Strafe right
        auto dir = direction();
        eye_pos += vec3(dir.y, -dir.x, 0) * v;
        break;
    }
    case 'a':{// Strafe left
        auto dir = direction();
        eye_pos += vec3(-dir.y, dir.x, 0) * v;
        break;
    }
    case 'w':// Forward
        eye_pos += direction() * v;
        break;
    case 's':// Back
        eye_pos += -direction() * v;
        break;
    case 'i':// Toggle display of reference image
        ref_image_enabled = !ref_image_enabled;
        break;
    case 'o':// Toggle blending of reference image
        ref_image_blend = !ref_image_blend;
        ref_image->enableBlending(ref_image_blend);
        break;
    case '\r':// Enter - reset to initial position
        eye_pos = terra->initialPos();
        rotation_cam = {0, -glm::radians(90.f)};
        vfov = initial_vfov;
        ref_image->setRotation(0);
        break;
    case 'l':
        light_rotation_enabled = !light_rotation_enabled;
        break;
    case 'v':{
        const char * filters[]  = {"*.jpg","*.png"};
        auto filename = fileOpenDialog(previous_filename.c_str(), filters, "Jpeg image files");
        if(!filename.empty()){
            initialize(filename.c_str());
        }
    }break;
    }
}

glm::mat4 Terrain::getCamRotation() const
{
    auto ry = glm::rotate(mat4(1)
                          , rotation_cam.x
                          , vec3(0,0,1));
    auto rx = glm::rotate(mat4(1)
                          , rotation_cam.y
                          , vec3(1,0,0));
    return rx*ry;
}

float Terrain::hfov() const
{
    return vfov * width / height;
}

glm::vec3 Terrain::animateLight()
{
    vec4 light_dir = glm::normalize(vec4(0., 5.0, 5.0, 0.0));
    if(light_rotation_enabled){
        light_rotation = glm::mod(light_rotation + 0.01f, glm::radians(360.f));
    }
    auto M = glm::rotate(mat4(1.0f), light_rotation, vec3(0.f, 1.f, 0.f));
    light_dir = M*light_dir;
    return vec3(light_dir);
}

