#version 460 core

layout (location = 0) in vec3 position;

out vec3 worldPosition;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix;

void main(){
	worldPosition = vec3(modelingMatrix * vec4(position, 1.0));
	gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(position, 1.0);
}