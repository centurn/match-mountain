#pragma once

#include "asg_base.h"
#include "asg_render_predef.h"
#include "attrib_buffer.h"
#include <memory>

namespace asg{


enum class DrawType{
  Points = 0x0000 //GL_POINTS
, Lines = 0x0001 //GL_LINES
, LineLoop = 0x0002 //GL_LINE_LOOP
, Strip = 0x0003 //GL_LINE_STRIP
, Triangles = 0x0004 //GL_TRIANGLES
, TriangleStrip = 0x0005 //GL_TRIANGLE_STRIP
, TriangleFan = 0x0006 //GL_TRIANGLE_FAN
};

struct DrawDescr{
    DrawType draw_type;
    Rsize count;
    std::shared_ptr<AttribBuffer> index_buffer;
    ptrdiff_t offset;
};

}
