#version 460 core

out vec4 fColor;

uniform vec3 centerPos;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 ambiant;

void main(){
    if(position.y > 0){
        fColor = vec4(ambiant, 1.0);
    }
    else{
        fColor = vec4(1, 1, 1, 1) - vec4(ambiant, 1.0);
    }
    gl_Position = projectionMatrix * viewMatrix *  vec4(position, 1.0);
}