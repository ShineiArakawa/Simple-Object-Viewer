#pragma once
#ifndef _SHADER_COMPILER_H_
#define _SHADER_COMPILER_H_

#include <OpenGL.hpp>
#include <Util/Logging.hpp>
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
  inline static const std::string KEY_SHADER = "Shader";
  inline static const std::string KEY_SHADER_VERT_SHADER_PATH = "VertShaderPath";
  inline static const std::string KEY_SHADER_FRAG_SHADER_PATH = "FragShaderPath";

 private:
  // nothing
 protected:
  // nothing

 public:
  static std::string readCodesFromFile(const std::string &filename);

  static GLuint compile(const std::string &code, GLuint type);

  static GLuint buildShaderProgram(const std::string &vertexShaderCode, const std::string &fragmentShaderCode);
};

#endif