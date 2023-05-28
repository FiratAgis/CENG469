#version 460 core

vec3 Iamb = vec3(0.8, 0.8, 0.8);
vec3 kd = vec3(0.8, 0.8, 0.8);
vec3 ka = vec3(0.3, 0.3, 0.3);
vec3 ks = vec3(0.8, 0.8, 0.8);

uniform vec3 eyePos;
uniform vec3 I[5];
uniform vec3 lightPos[5];
uniform int lightNo;

uniform mat4 modelingMatrix;

in vec3 norm;
in vec3 pos;
out vec4 fragColor;

void main(void)
{
	vec3 ver = eyePos - pos;
	vec3 V = normalize(ver);
	vec3 N = normalize(norm);

	vec3 diffuseColor = vec3(0, 0, 0);
	vec3 specularColor = vec3(0, 0, 0);
	
	for(int i = 0; i < lightNo; i++){
		vec3 dir = vec3(modelingMatrix * vec4(lightPos[i], 1)) - pos;
		//vec3 dir = lightPos[i] - vec3(pWorld);
		vec3 L = normalize(dir);
		vec3 H = normalize(L + V);

		float dist = (pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2));

		float NdotL = abs(dot(N, L)); // for diffuse component
		float NdotH = abs(dot(N, H)); // for specular component

		diffuseColor += (I[i]/pow(0.5 + dist, 2.0)) * kd * max(NdotL,0);
		specularColor += (I[i]/pow(0.5 + dist, 2.0)) * ks * max(pow(NdotH, 400), 0);
	}
	vec3 ambientColor = Iamb * ka;

	fragColor = vec4(clamp(diffuseColor + specularColor + ambientColor, 0.0, 1.0), 1);
}
