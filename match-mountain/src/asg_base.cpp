#include "asg_base.h"

namespace asg{
GenericException::~GenericException() noexcept
{
}

const char *GenericException::what() const noexcept
{
    return reason.c_str();
}

}
