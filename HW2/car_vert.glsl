#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 ambiant_color;
layout (location = 3) in vec3 reflective_color;

out vec3 Normal;
out vec3 Position;
out vec3 Ambiant_Color;
out vec3 Reflective_Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Ambiant_Color = ambiant_color;
    Reflective_Color = reflective_color;
    Normal = mat3(transpose(inverse(model))) * normal;
    Position = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1.0);
}