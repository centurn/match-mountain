#include "mam_base.h"


GenericException::~GenericException() noexcept
{
}

const char *GenericException::what() const noexcept
{
    return reason.c_str();
}
