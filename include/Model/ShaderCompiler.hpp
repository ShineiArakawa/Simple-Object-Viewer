#pragma once
#ifndef _SHADER_COMPILER_H_
#define _SHADER_COMPILER_H_

#include <OpenGL.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ShaderCompiler {
 private:
  // nothing
 protected:
  // nothing
 public:
  // nothing

 private:
  // nothing
 protected:
  // nothing

 public:
  static std::string readCodesFromFIle(const std::string& filename);
  static GLuint compile(const std::string& code, GLuint type);
  static GLuint buildShaderProgram(const std::string& vertexShaderCode,
                                   const std::string& fragmentShaderCode);
};
#endif
