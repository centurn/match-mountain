#pragma once

#include "mam_log.h"
#include <exception>
#include <string>

using uint = unsigned int;

// Move it to our namespace just for convenience
// Or should I place it in std:: directly?
#define TCB_SPAN_NAMESPACE_NAME asg

#include "span.hpp"

// Say 'goodbye' and die!
#define PANIC(...){\
    log_e(__VA_ARGS__);\
    std::abort();\
}

// Hide it into cpp?
class GenericException
    : public std::exception
{
public:
    explicit GenericException(const char* reason) noexcept
        : reason(reason)
    {
    }

    virtual ~GenericException() noexcept override;
    virtual const char* what() const noexcept override;
private:
    std::string reason;
};

// Just to signify the intentional absence of 'explicit' qualifier
#define IMPLICIT

// Actual deleters will be declared in specializations of this class
template <class T>
struct DeleteTraits{
};

// This macro should be used in the same .cpp where
// the destructor of enclosing class is defined
// (and GL.h is included - we don't want it to be included in headers)
#define DEFINE_GL_DELETER(Class, DeleteFunc)\
    template <>\
    struct DeleteTraits<Class>{\
        static void destroy(uint id){\
            DeleteFunc(id);\
        }\
    };

#define DEFINE_GL_ARRAY_DELETER(Class, DeleteFunc)\
    template <>\
    struct DeleteTraits<Class>{\
        static void destroy(int count, uint* id){\
            DeleteFunc(count, id);\
        }\
    };

// Holder of OpenGL 'names' (IDs) to guarantee proper delete order
// ...As much as possible, since there's no 'automatic'  way to guarantee
// that destructor is called while the context is bound
template <typename Deleter>
struct ResourceGL{
    ~ResourceGL(){
        DeleteTraits<Deleter>::destroy(id);
    }
    uint id = 0;
};

// Same as above, but for arrays
template <int Count, typename Deleter>
struct ResourceArrayGL{
    ~ResourceArrayGL(){
        DeleteTraits<Deleter>::destroy(Count, id);
    }
    uint id[Count] = {};
};
