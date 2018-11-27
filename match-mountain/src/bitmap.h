#pragma once

#include <memory>

namespace asg{

// Bitmap image loaded from file
class Bitmap{
public:
    // Attempt loading resource. Throws on failure
    Bitmap(const char* source);
    ~Bitmap();

    const uint8_t* getData() const{
        return data.get();
    }
    int getWidth() const{
        return width;
    }
    int getHeight() const{
        return height;
    }
private:
    // Buffer is allocated by stb, so custom deleter is needed
    // Separate functor to avoid silly indirection of pointer-to-function
    struct StbDeleter{
        void operator()(void* data);
    };
    std::unique_ptr<uint8_t, StbDeleter> data;
    int width;
    int height;
};

}
