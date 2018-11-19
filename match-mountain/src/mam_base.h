#pragma once

#include "mam_log.h"
#include <exception>
#include <string>

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
