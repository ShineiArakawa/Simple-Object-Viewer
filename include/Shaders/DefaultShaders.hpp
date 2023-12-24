#pragma once
#ifndef _DEFAULT_SHADERS_H_
#define _DEFAULT_SHADERS_H_

#include <string>

class DefaultShaders {
 public:
  inline static const std::string FRAG_SHADER =
      "#version 330\n"
      "\n"
      "in vec2 f_uv;\n"
      "in vec3 f_worldPos;\n"
      "in vec3 f_color;\n"
      "in vec3 f_normal;\n"
      "in float f_id;\n"
      "\n"
      "uniform float u_toUseTexture;\n"
      "uniform sampler2D u_texture;\n"
      "uniform vec3 u_position;\n"
      "\n"
      "out vec4 out_color;\n"
      "\n"
      "void main() {\n"
      "    if(u_toUseTexture > 0.5 && u_toUseTexture < 1.5) {\n"
      "        out_color = texture(u_texture, f_uv);\n"
      "    } else if(u_toUseTexture > -0.5 && u_toUseTexture < 0.5) {\n"
      "        out_color = vec4(f_color, 1.0);\n"
      "    } else if(u_toUseTexture > -1.5 && u_toUseTexture < -0.5) {\n"
      "        vec3 dfdx = dFdx(f_worldPos);\n"
      "        vec3 dfdy = dFdy(f_worldPos);\n"
      "        vec3 color = normalize(cross(dfdx, dfdy));\n"
      "        color = (color + 1.0) / 2.0;\n"
      "        out_color = vec4(color, 1.0);\n"
      "    } else if(u_toUseTexture > -2.5 && u_toUseTexture < -1.5) {\n"
      "        out_color = vec4(1.0, 1.0, 1.0, 1.0);\n"
      "    } else if(u_toUseTexture > -3.5 && u_toUseTexture < -2.5) {\n"
      "        out_color = vec4(f_normal, 1.0);\n"
      "    }\n"
      "}\n";
  inline static const std::string VERT_SHADER =
      "#version 330\n"
      "\n"
      "layout(location = 0) in vec3 in_position;\n"
      "layout(location = 1) in vec3 in_color;\n"
      "layout(location = 2) in vec3 in_normal;\n"
      "layout(location = 3) in vec2 in_uv;\n"
      "layout(location = 4) in float in_id;\n"
      "\n"
      "uniform mat4 u_mvpMat;\n"
      "\n"
      "out vec2 f_uv;\n"
      "out vec3 f_worldPos;\n"
      "out vec3 f_color;\n"
      "out vec3 f_normal;\n"
      "out float f_id;\n"
      "\n"
      "void main() {\n"
      "    gl_Position = u_mvpMat * vec4(in_position, 1.0);\n"
      "\n"
      "    f_worldPos = in_position;\n"
      "    f_color = in_color;\n"
      "    f_normal = in_normal;\n"
      "    f_uv = in_uv;\n"
      "    f_id = in_id;\n"
      "}\n";
};

#endif
