#ifndef _CORE_GPU_GL_GL_UTIL_H_
#define _CORE_GPU_GL_GL_UTIL_H_

#include <numeric>
#include <stdint.h>

namespace core::gl::util
{

unsigned char* ToBuffer(uint32_t textureId, int width, int height);

}

#endif