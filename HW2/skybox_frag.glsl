#version 460 core
out vec4 color;

in vec3 coord;

uniform samplerCube skybox;

void main()
{    
    color = texture(skybox, coord);
}