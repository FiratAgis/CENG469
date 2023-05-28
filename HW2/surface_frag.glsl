#version 460 core

out vec4 color;

in vec2 texCoor;

uniform sampler2D tex;


void main()
{    
    color = texture(tex, texCoor);
}