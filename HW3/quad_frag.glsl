#version 460 core

in vec3 worldPosition;

out vec4 fragColor;

uniform samplerCube cubemap;
uniform vec3 cameraPosition;
uniform float rayMarchDistance;
uniform int rayMarchCount;
uniform int turbulanceCount;
uniform float maxY;
uniform float minY;

uniform uint gaussEnabled;

vec3 gradients[22] = {
	vec3(1, 1, 0),
	vec3(-1, 1, 0),
	vec3(1, -1, 0),
	vec3(-1, -1, 0),
	vec3(1, 0, 1),
	vec3(-1, 0, 1),
	vec3(1, 0, -1),
	vec3(-1, 0, -1),
	vec3(0, 1, 1),
	vec3(0, -1, 1),
	vec3(0, 1, -1),
	vec3(0, -1, -1),
	vec3(1, 1, 0),
	vec3(-1, 1, 0),
	vec3(0, -1, 1),
	vec3(0, -1, -1),
	vec3(2, 0, 0),
	vec3(-2, 0, 0),
	vec3(0, 2, 0),
	vec3(0, -2, 0),
	vec3(0, 0, 2),
	vec3(0, 0, -2)
};

int table[22] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
};

vec3 getGradient(vec3 coord){
	int idx;
	idx = table[abs(int(coord.z)) % 22];
	idx = table[abs(int(coord.y) + idx) % 22];
	idx = table[abs(int(coord.x) + idx) % 22];
	return gradients[idx];
}

vec3 cloudColor = vec3(1.0, 1.0, 1.0);

float f(float x){
	float tx = abs(x);
	if(tx < 1){
		return -6 * pow(tx, 5) + 15 * pow(tx, 4) - 10 * pow(tx, 3) + 1;
	}
	else{
		return 0;
	}
}

float perlin(vec3 coord){
	vec3 downBase = floor(coord);
    vec3 downFrac = coord - downBase;
    float retVal = 0;
    for (int i = 0; i < 8; i++){
        vec3 coordNew = vec3(downBase.x + i % 2, downBase.y + int(i / 2) % 2, downBase.z + int(i / 4) % 2);
        vec3 g = getGradient(coordNew);
        vec3 frag = (coord - coordNew);
        retVal += f(frag.x) * f(frag.y) * f(frag.z) * dot(g, frag);
	}
    return abs(retVal) / 2.0;
}

float turbulence(vec3 coord){
	float retVal = 0;
	for(int i = 0; i < turbulanceCount; i++){
		float pFact = pow(2, i);
		retVal += perlin(vec3(coord.x * pFact, coord.y * pFact, coord.z * pFact)) / pFact;
	}
	return retVal;
}

void main(){
	vec3 direction = normalize(worldPosition - cameraPosition);
	vec3 skyColor = texture(cubemap, direction).rgb;
	vec3 pos = worldPosition + (rayMarchCount - 1) * rayMarchDistance * direction;

	for(int i = 0; i < rayMarchCount; i++)
	{
		float alpha;
		int layerVar = rayMarchCount - i;
		float dist;

		if(pos.y > minY && pos.y < maxY)
		{
			dist = 1.0;
		}
		else if(pos.y < minY)
		{
			dist = pow(max(minY - pos.y, 1.0), 2);
		}
		else
		{
			dist =  pow(max(pos.y - maxY, 1.0), 2);
		}

		if(turbulanceCount > 0)
		{
			alpha = turbulence(pos) / (layerVar * dist);
		}
		else
		{
			alpha = perlin(pos) / (layerVar * dist);
		}

		skyColor = alpha * cloudColor + (1 - alpha) * skyColor;
		pos = pos - rayMarchDistance * direction;
	}

	fragColor = vec4(skyColor, 1.0);
}