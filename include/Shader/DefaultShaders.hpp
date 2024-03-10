#pragma once

#include <string>

namespace simview {
namespace shader {

class DefaultModelShader {
 public:
  // clang-format off
  inline static const char* UNIFORM_NAME_MV_MAT                     = "u_mvMat";
  inline static const char* UNIFORM_NAME_MVP_MAT                    = "u_mvpMat";
  inline static const char* UNIFORM_NAME_NORM_MAT                   = "u_normMat";
  inline static const char* UNIFORM_NAME_LIGHT_MAT                  = "u_lightMat";
  inline static const char* UNIFORM_NAME_LIGHT_POS                  = "u_lightPos";
  inline static const char* UNIFORM_NAME_SHININESS                  = "u_shininess";
  inline static const char* UNIFORM_NAME_AMBIENT_INTENSITY          = "u_ambientIntensity";
  inline static const char* UNIFORM_NAME_AMBIENT_COLOR              = "u_ambientColor";
  inline static const char* UNIFORM_NAME_DIFFUSE_COLOR              = "u_diffuseColor";
  inline static const char* UNIFORM_NAME_SPECULAR_COLOR             = "u_specularColor";
  inline static const char* UNIFORM_NAME_WIRE_FRAME_MODE            = "u_wireFrameMode";
  inline static const char* UNIFORM_NAME_WIRE_FRAME_COLOR           = "u_wireFrameColor";
  inline static const char* UNIFORM_NAME_WIRE_FRAME_WIDTH           = "u_wireFrameWidth";
  inline static const char* UNIFORM_NAME_RENDER_TYPE                = "u_renderType";
  inline static const char* UNIFORM_NAME_BUMP_MAP                   = "u_bumpMap";
  inline static const char* UNIFORM_NAME_LIGHT_MVP_MAT              = "u_lightMvpMat";
  inline static const char* UNIFORM_NAME_SHADOW_MAPPING             = "u_shadowMapping";

  inline static const char* UNIFORM_NAME_AMBIENT_TEXTURE            = "u_ambientTexture";
  inline static const char* UNIFORM_NAME_DIFFUSE_TEXTURE            = "u_diffuseTexture";
  inline static const char* UNIFORM_NAME_SPECULAR_TEXTURE           = "u_specularTexture";
  inline static const char* UNIFORM_NAME_NORMAL_MAP                 = "u_normalMap";
  inline static const char* UNIFORM_NAME_AMBIENT_TEXTURE_FLAG       = "u_hasAmbientTexture";
  inline static const char* UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG       = "u_hasDiffuseTexture";
  inline static const char* UNIFORM_NAME_SPECULAR_TEXTURE_FLAG      = "u_hasSpecularTexture";
  inline static const char* UNIFORM_NAME_DEPTH_TEXTURE              = "u_depthTexture";
  // clang-format on

  inline static const std::string VERT_SHADER =
      "#version 460\n"
      "\n"
      "layout(location = 0) in vec3 in_position;\n"
      "layout(location = 1) in vec3 in_color;\n"
      "layout(location = 2) in vec3 in_normal;\n"
      "layout(location = 3) in vec3 in_bary;\n"
      "layout(location = 4) in vec2 in_uv;\n"
      "layout(location = 5) in float in_id;\n"
      "\n"
      "uniform mat4 u_mvpMat;\n"
      "uniform mat4 u_mvMat;\n"
      "uniform mat4 u_normMat;\n"
      "uniform mat4 u_lightMat;\n"
      "uniform vec3 u_lightPos;\n"
      "uniform mat4 u_lightMvpMat;\n"
      "\n"
      "out vec2 f_uv;\n"
      "out vec3 f_worldPos;\n"
      "out vec3 f_color;\n"
      "out vec3 f_normal;\n"
      "out vec3 f_barycentric;\n"
      "out float f_id;\n"
      "out vec3 f_positionCameraSpace;\n"
      "out vec3 f_normalCameraSpace;\n"
      "out vec3 f_lightPosCameraSpace;\n"
      "out vec4 f_positionLightScreenSpace;\n"
      "\n"
      "void main() {\n"
      "    gl_Position = u_mvpMat * vec4(in_position, 1.0);\n"
      "\n"
      "    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;\n"
      "    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;\n"
      "    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;\n"
      "    f_positionLightScreenSpace = u_lightMvpMat * vec4(in_position, 1.0);\n"
      "\n"
      "    f_worldPos = in_position;\n"
      "    f_color = in_color;\n"
      "    f_normal = in_normal;\n"
      "    f_barycentric = in_bary;\n"
      "    f_uv = in_uv;\n"
      "    f_id = in_id;\n"
      "}\n";

  inline static const std::string FRAG_SHADER =
      "#version 460\n"
      "\n"
      "in vec2 f_uv;\n"
      "in vec3 f_worldPos;\n"
      "in vec3 f_color;\n"
      "in vec3 f_normal;\n"
      "in vec3 f_barycentric;\n"
      "in float f_id;\n"
      "in vec3 f_positionCameraSpace;\n"
      "in vec3 f_normalCameraSpace;\n"
      "in vec3 f_lightPosCameraSpace;\n"
      "in vec4 f_positionLightScreenSpace;\n"
      "\n"
      "uniform float u_renderType;\n"
      "uniform float u_bumpMap;\n"
      "uniform float u_wireFrameMode;\n"
      "uniform vec3 u_wireFrameColor;\n"
      "uniform float u_wireFrameWidth;\n"
      "uniform float u_ambientIntensity;\n"
      "uniform vec3 u_ambientColor;\n"
      "uniform vec3 u_diffuseColor;\n"
      "uniform vec3 u_specularColor;\n"
      "uniform float u_shininess;\n"
      "uniform float u_shadowMapping;\n"
      "\n"
      "uniform sampler2D u_ambientTexture;\n"
      "uniform sampler2D u_diffuseTexture;\n"
      "uniform sampler2D u_specularTexture;\n"
      "uniform sampler2D u_normalMap;\n"
      "uniform sampler2D u_depthTexture;\n"
      "\n"
      "uniform float u_hasAmbientTexture;\n"
      "uniform float u_hasDiffuseTexture;\n"
      "uniform float u_hasSpecularTexture;\n"
      "\n"
      "out vec4 out_color;\n"
      "\n"
      "vec3 getAmbientColor() {\n"
      "    if(u_hasAmbientTexture > 0.5) {\n"
      "        return texture(u_ambientTexture, f_uv).xyz;\n"
      "    }\n"
      "    return u_ambientColor;\n"
      "}\n"
      "\n"
      "vec3 getDiffuseColor() {\n"
      "    if(u_hasDiffuseTexture > 0.5) {\n"
      "        return texture(u_diffuseTexture, f_uv).xyz;\n"
      "    }\n"
      "    return u_diffuseColor;\n"
      "}\n"
      "\n"
      "vec3 getSpecularColor() {\n"
      "    if(u_hasSpecularTexture > 0.5) {\n"
      "        return texture(u_specularTexture, f_uv).xyz;\n"
      "    }\n"
      "    return u_specularColor;\n"
      "}\n"
      "\n"
      "vec3 calcLocalNormal() {\n"
      "    vec3 dPdx = dFdx(f_worldPos);\n"
      "    vec3 dPdy = dFdy(f_worldPos);\n"
      "\n"
      "    vec2 dUdx = dFdx(f_uv); /* (du/dx, dv/dx) */\n"
      "    vec2 dUdy = dFdy(f_uv); /* (du/dy, dv/dy) */\n"
      "\n"
      "    float det = abs(dUdx[0] * dUdy[1] - dUdx[1] * dUdy[0]);\n"
      "\n"
      "    float invMat00 = dUdy[1] / det;\n"
      "    float invMat01 = -dUdx[1] / det;\n"
      "    float invMat10 = -dUdy[0] / det;\n"
      "    float invMat11 = dUdx[0] / det;\n"
      "\n"
      "    vec3 localX = normalize(invMat00 * dPdx + invMat01 * dPdy);\n"
      "    vec3 localY = normalize(invMat10 * dPdx + invMat11 * dPdy);\n"
      "    vec3 localZ = cross(localX, localY);\n"
      "\n"
      "    vec4 normalFromMap = texture(u_normalMap, f_uv) * 2.0 - 1.0;\n"
      "\n"
      "    return normalize(normalFromMap.x * localX + normalFromMap.y * localY + normalFromMap.z * localZ);\n"
      "}\n"
      "\n"
      "float calcShadow(vec4 fragPosLightSpace) {\n"
      "    // perform perspective divide\n"
      "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
      "    // transform to [0,1] range\n"
      "    projCoords = projCoords * 0.5 + 0.5;\n"
      "    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)\n"
      "    float closestDepth = texture(u_depthTexture, projCoords.xy).r; \n"
      "    // get depth of current fragment from light's perspective\n"
      "    float currentDepth = projCoords.z;\n"
      "    // check whether current frag pos is in shadow\n"
      "    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;\n"
      "\n"
      "    return shadow;\n"
      "}\n"
      "\n"
      "vec4 shading(\n"
      "    vec3 diffuseColor,\n"
      "    vec3 specularColor,\n"
      "    vec3 ambientColor,\n"
      "    vec3 N\n"
      ") {\n"
      "    vec3 V = normalize(-f_positionCameraSpace);\n"
      "    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);\n"
      "    vec3 H = normalize(V + L);\n"
      "\n"
      "    float ndotl = max(0.0, dot(N, L));\n"
      "    float ndoth = max(0.0, dot(N, H));\n"
      "\n"
      "    vec3 diffuse = diffuseColor * ndotl;\n"
      "    vec3 specular = specularColor * pow(ndoth, u_shininess);\n"
      "    vec3 ambient = u_ambientIntensity * ambientColor;\n"
      "\n"
      "    float shadow = u_shadowMapping > 0.5 ? calcShadow(f_positionLightScreenSpace) : 0.0;\n"
      "\n"
      "    return vec4((1.0 - shadow) * (diffuse + specular) + ambient, 1.0);\n"
      "}\n"
      "\n"
      "void main() {\n"
      "    // ================================================================================================================================\n"
      "    // Wire frame\n"
      "    // ================================================================================================================================\n"
      "    if(u_wireFrameMode > 0.5 && u_wireFrameMode < 1.5) {\n"
      "        // Wire frame\n"
      "        if(f_barycentric.x < u_wireFrameWidth || f_barycentric.y < u_wireFrameWidth || f_barycentric.z < u_wireFrameWidth) {\n"
      "            out_color = vec4(u_wireFrameColor, 1.0);\n"
      "            return;\n"
      "        }\n"
      "    } else if(u_wireFrameMode > -1.5 && u_wireFrameMode < -0.5) {\n"
      "        // Wire frame only\n"
      "        if(f_barycentric.x < u_wireFrameWidth || f_barycentric.y < u_wireFrameWidth || f_barycentric.z < u_wireFrameWidth) {\n"
      "            out_color = vec4(u_wireFrameColor, 1.0);\n"
      "        } else {\n"
      "            out_color = vec4(u_wireFrameColor, 0.0);\n"
      "        }\n"
      "        return;\n"
      "    }\n"
      "\n"
      "    // ================================================================================================================================\n"
      "    // Normal\n"
      "    // ================================================================================================================================\n"
      "    vec3 N;\n"
      "    if(u_bumpMap > 0.5) {\n"
      "        N = calcLocalNormal();\n"
      "    } else {\n"
      "        N = normalize(f_normalCameraSpace);\n"
      "    }\n"
      "\n"
      "    // ================================================================================================================================\n"
      "    // Render\n"
      "    // ================================================================================================================================\n"
      "    if(u_renderType > 3.5 && u_renderType < 4.5) {\n"
      "        // Material shading\n"
      "        out_color = shading(u_diffuseColor, u_specularColor, u_ambientColor, N);\n"
      "    } else if(u_renderType > 2.5 && u_renderType < 3.5) {\n"
      "        // Texture with shading\n"
      "        out_color = shading(getDiffuseColor(), getSpecularColor(), getAmbientColor(), N);\n"
      "    } else if(u_renderType > 1.5 && u_renderType < 2.5) {\n"
      "        // Color with shading\n"
      "        out_color = shading(f_color, vec3(1.0), f_color, N);\n"
      "    } else if(u_renderType > 0.5 && u_renderType < 1.5) {\n"
      "        // Texture\n"
      "        out_color = texture(u_diffuseTexture, f_uv);\n"
      "    } else if(u_renderType > -0.5 && u_renderType < 0.5) {\n"
      "        // Color\n"
      "        out_color = vec4(f_color, 1.0);\n"
      "    } else if(u_renderType > -1.5 && u_renderType < -0.5) {\n"
      "        // Face Normal\n"
      "        vec3 dfdx = dFdx(f_worldPos);\n"
      "        vec3 dfdy = dFdy(f_worldPos);\n"
      "        vec3 color = normalize(cross(dfdx, dfdy));\n"
      "        color = (color + 1.0) / 2.0;\n"
      "        out_color = vec4(color, 1.0);\n"
      "    } else if(u_renderType > -2.5 && u_renderType < -1.5) {\n"
      "        // Mask\n"
      "        out_color = vec4(1.0, 1.0, 1.0, 1.0);\n"
      "    } else if(u_renderType > -3.5 && u_renderType < -2.5) {\n"
      "        // Vertex normal\n"
      "        out_color = vec4(f_normal, 1.0);\n"
      "    }\n"
      "}\n";
};

class DefaultDepthShader {
 public:
  inline static const char* UNIFORM_NAME_LIGHT_MVP_MAT = "u_lightMvpMat";

  inline static const std::string VERT_SHADER =
      "#version 460\n"
      "\n"
      "layout(location = 0) in vec3 in_position;\n"
      "layout(location = 1) in vec3 in_color;\n"
      "layout(location = 2) in vec3 in_normal;\n"
      "layout(location = 3) in vec3 in_bary;\n"
      "layout(location = 4) in vec2 in_uv;\n"
      "layout(location = 5) in float in_id;\n"
      "\n"
      "uniform mat4 u_lightMvpMat;\n"
      "\n"
      "void main() {\n"
      "    gl_Position = u_lightMvpMat * vec4(in_position, 1.0);\n"
      "    // gl_Position = mat4(1.0f) * vec4(in_position, 1.0);\n"
      "}\n";

  inline static const std::string FRAG_SHADER =
      "#version 460\n"
      "\n"
      "layout(location = 0) out float fragmentdepth;\n"
      "\n"
      "void main() {\n"
      "    fragmentdepth = gl_FragCoord.z;\n"
      "}\n";
};

class DefaultDepthQuadShader {
 public:
  inline static const std::string VERT_SHADER =
      "#version 460\n"
      "\n"
      "layout(location = 0) in vec3 in_position;\n"
      "layout(location = 1) in vec3 in_color;\n"
      "layout(location = 2) in vec3 in_normal;\n"
      "layout(location = 3) in vec3 in_bary;\n"
      "layout(location = 4) in vec2 in_uv;\n"
      "layout(location = 5) in float in_id;\n"
      "\n"
      "out vec2 f_uv;\n"
      "\n"
      "void main() {\n"
      "    gl_Position = vec4(in_position, 1.0);\n"
      "    f_uv = in_uv;\n"
      "}\n";

  inline static const std::string FRAG_SHADER =
      "#version 460\n"
      "\n"
      "in vec2 f_uv;\n"
      "\n"
      "uniform sampler2D u_diffuseTexture;\n"
      "\n"
      "out vec4 out_color;\n"
      "\n"
      "void main() {\n"
      "    float depthValue = texture(u_diffuseTexture, f_uv).r;\n"
      "    depthValue = 1.0 - (1.0 - depthValue) * 25.0;\n"
      "    out_color = vec4(depthValue);\n"
      "    // out_color = vec4(vec3(depthValue), 1.0);\n"
      "}\n";
};

}  // namespace shader
}  // namespace simview