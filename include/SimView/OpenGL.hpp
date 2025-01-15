#pragma once

// clang-format off
#ifndef _INCLUDE_GL_

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#if defined(__APPLE__)
#define SIMVIEW_SHADER_VERSION "#version 410"
#define SIMVIEW_OPENGL_VERSION_MAJOR 4
#define SIMVIEW_OPENGL_VERSION_MINOR 1
#else
#define SIMVIEW_SHADER_VERSION "#version 460"
#define SIMVIEW_OPENGL_VERSION_MAJOR 4
#define SIMVIEW_OPENGL_VERSION_MINOR 6
#endif

#define _INCLUDE_GL_
#endif
// clang-format on
