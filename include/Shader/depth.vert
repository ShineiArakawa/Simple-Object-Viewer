#version 330

layout(location = 0) in vec3 in_position;

uniform mat4 u_lightMvpMat;    

void main() {
                  
    
    
    
    gl_Position = u_lightMvpMat * vec4(in_position, 1.0);

}