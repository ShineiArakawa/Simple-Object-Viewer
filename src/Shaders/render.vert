#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;
layout(location = 4) in float in_id;

uniform mat4 u_mvpMat;

out vec2 f_uv;
out vec3 f_worldPos;
out vec3 f_color;
out vec3 f_normal;
out float f_id;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_worldPos = in_position;
    f_color = in_color;
    f_normal = in_normal;
    f_uv = in_uv;
    f_id = in_id;
}