#pragma once

#if !defined(__BRAVE_GLAD__)
#define __BRAVE_GLAD__


#include <Dac/Logger.hpp>

#ifndef __glad_h_
#include <glad/glad.h>
#endif

/// GL_ERRORS :: PreProcesor func wrapper
#define GL_ASSERT(funcToCheck) \
  glErrClear();                \
  funcToCheck;                 \
  glAssert(__FILE__, __LINE__);

/// GL_ERRORS :: Avoid fake error
void glErrClear() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

/// GL_ERRORS :: Parse real errors after gl* func is executed
void glAssert(const char* file, int line) {
  if (auto glError = glGetError()) {
    const char* errStr;
    switch (glError) {
      case GL_INVALID_ENUM: errStr = "Invalid enum"; break;
      case GL_INVALID_VALUE: errStr = "Invalid value"; break;
      case GL_INVALID_OPERATION: errStr = "Invalid operation"; break;
      default: errStr = "Unrecognised GLenum"; break;
    }
    DacLog_ERR("{} ({}): {} ({})", file, line, errStr, glError);
  }
}


#endif // __BRAVE_GLAD__
