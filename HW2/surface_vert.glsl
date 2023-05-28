#version 460 core

out vec2 texCoor;

uniform vec3 centerPos;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 text;
void main(){
    texCoor = text; 
    gl_Position = projectionMatrix * viewMatrix *  vec4(position, 1.0);
}