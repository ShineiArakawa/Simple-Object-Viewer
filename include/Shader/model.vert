#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_bary;
layout(location = 4) in vec2 in_uv;
layout(location = 5) in float in_id;

uniform mat4 u_mvpMat;
uniform mat4 u_mvMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;
uniform vec3 u_lightPos;
uniform mat4 u_lightMvpMat;

out vec2 f_uv;
out vec3 f_worldPos;
out vec3 f_color;
out vec3 f_normal;
out vec3 f_barycentric;
out float f_id;
out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;
out vec3 f_positionLightScreenSpace;

void main() {
    // gl_Position = u_mvpMat * vec4(in_position, 1.0);
    gl_Position = u_lightMvpMat * vec4(in_position, 1.0);

    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
    f_positionLightScreenSpace = (u_lightMvpMat * vec4(in_position, 1.0)).xyz;

    f_worldPos = in_position;
    f_color = in_color;
    f_normal = in_normal;
    f_barycentric = in_bary;
    f_uv = in_uv;
    f_id = in_id;
}