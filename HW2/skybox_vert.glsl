#version 460 core

layout (location = 0) in vec3 pos;

out vec3 coord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 centerPos;

void main()
{
    coord = pos;
    gl_Position = projectionMatrix * viewMatrix * vec4(pos + centerPos, 1.0);
}