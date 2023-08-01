#pragma once
#ifndef _SHADER_COMPILER_H_
#define _SHADER_COMPILER_H_

#include <OpenGL.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class ShaderCompiler {
 private:
  // nothing
 protected:
  // nothing
 public:
  inline static const std::string SHADER_ROOT_DIRECTORY = "../Shaders/";
  inline const static std::string VERT_SHADER_FILE = SHADER_ROOT_DIRECTORY + "render.vert";
  inline const static std::string FRAG_SHADER_FILE = SHADER_ROOT_DIRECTORY + "render.frag";

 private:
  // nothing
 protected:
  // nothing

 public:
  static GLuint compile(const std::string& filename, GLuint type);
  static GLuint buildShaderProgram(const std::string& vShaderFile, const std::string& fShaderFile);
};
#endif
