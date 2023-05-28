#version 460 core
out vec4 fColor;

in vec3 Normal;
in vec3 Position;
in vec3 Ambiant_Color;
in vec3 Reflective_Color;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{    
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 texture_data = texture(skybox, R).rgb;
    vec3 texture_color = vec3(texture_data.x * Reflective_Color.x, texture_data.y * Reflective_Color.y, texture_data.z * Reflective_Color.z);
    fColor = vec4(clamp(texture_color + Ambiant_Color, 0.0, 1.0), 1);
}