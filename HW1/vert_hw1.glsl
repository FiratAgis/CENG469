#version 460 core

mat4 Z_x = mat4(0.0, 0.33, 0.66, 1.0,
				0.0, 0.33, 0.66, 1.0,
				0.0, 0.33, 0.66, 1.0,
				0.0, 0.33, 0.66, 1.0);

mat4 Z_y = mat4(0.00, 0.00, 0.00, 0.00,
				0.33, 0.33, 0.33, 0.33,
				0.66, 0.66, 0.66, 0.66,
				1.00, 1.00, 1.00, 1.00);
				
mat4 M_b = mat4(-1.0, 3.0, -3.0, 1.0,
				3.0, -6.0, 3.0, 0.0,
				-3.0, 3.0, 0.0, 0.0,
				1.0, 0.0, 0.0, 0.0);

uniform vec3 lightPos[5];
uniform vec3 I[5];
uniform int lightNo;

uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;
uniform vec3 eyePos;
out vec4 color;
uniform float scale;
uniform ivec2 curveAmount;

uniform mat4 Z_z00;
uniform mat4 Z_z01;
uniform mat4 Z_z02;
uniform mat4 Z_z03;
uniform mat4 Z_z04;
uniform mat4 Z_z05; 

uniform mat4 Z_z10;
uniform mat4 Z_z11;
uniform mat4 Z_z12;
uniform mat4 Z_z13;
uniform mat4 Z_z14;
uniform mat4 Z_z15; 

uniform mat4 Z_z20;
uniform mat4 Z_z21;
uniform mat4 Z_z22;
uniform mat4 Z_z23;
uniform mat4 Z_z24;
uniform mat4 Z_z25; 

uniform mat4 Z_z30;
uniform mat4 Z_z31;
uniform mat4 Z_z32;
uniform mat4 Z_z33;
uniform mat4 Z_z34;
uniform mat4 Z_z35; 

uniform mat4 Z_z40;
uniform mat4 Z_z41;
uniform mat4 Z_z42;
uniform mat4 Z_z43;
uniform mat4 Z_z44;
uniform mat4 Z_z45;

uniform mat4 Z_z50;
uniform mat4 Z_z51;
uniform mat4 Z_z52;
uniform mat4 Z_z53;
uniform mat4 Z_z54;
uniform mat4 Z_z55;

layout(location=0) in ivec2 curveIndex;
layout(location=1) in vec2 t;

out vec3 norm;
out vec3 pos;

mat4 getZ_z(int x, int y){
	switch(x){
		case 0:
			switch(y){
				case 0:
					return Z_z00;
				case 1:
					return Z_z01;
				case 2:
					return Z_z02;
				case 3:
					return Z_z03;
				case 4:
					return Z_z04;
				case 5:
					return Z_z05;
			}
			break;
		case 1:
			switch(y){
				case 0:
					return Z_z10;
				case 1:
					return Z_z11;
				case 2:
					return Z_z12;
				case 3:
					return Z_z13;
				case 4:
					return Z_z14;
				case 5:
					return Z_z15;
			}
			break;
		case 2:
			switch(y){
				case 0:
					return Z_z20;
				case 1:
					return Z_z21;
				case 2:
					return Z_z22;
				case 3:
					return Z_z23;
				case 4:
					return Z_z24;
				case 5:
					return Z_z25;
			}
			break;
		case 3:
			switch(y){
				case 0:
					return Z_z30;
				case 1:
					return Z_z31;
				case 2:
					return Z_z32;
				case 3:
					return Z_z33;
				case 4:
					return Z_z34;
				case 5:
					return Z_z35;
			}
			break;
		case 4:
			switch(y){
				case 0:
					return Z_z40;
				case 1:
					return Z_z41;
				case 2:
					return Z_z42;
				case 3:
					return Z_z43;
				case 4:
					return Z_z44;
				case 5:
					return Z_z45;
			}
			break;
		case 5:
			switch(y){
				case 0:
					return Z_z50;
				case 1:
					return Z_z51;
				case 2:
					return Z_z52;
				case 3:
					return Z_z53;
				case 4:
					return Z_z54;
				case 5:
					return Z_z55;
			}
			break;
	}
	return Z_z00;
}

void main(void){
	float dim = scale / float(max(1, max(curveAmount.x, curveAmount.y)));
	vec2 startPos = vec2(-scale / 2.0, -scale / 2.0);
	vec2 curveStartPos = startPos + dim * curveIndex;
	
	mat4 Z_xNew = curveStartPos.x + Z_x * dim;
	mat4 Z_yNew = curveStartPos.y + Z_y * dim;
	
	vec4 S = vec4(pow(t.x, 3), pow(t.x, 2), t.x, 1);
	vec4 T = vec4(pow(t.y, 3), pow(t.y, 2), t.y, 1);
	
	vec4 dS = vec4(3*pow(t.x, 2), 2*t.x, 1, 0);
	vec4 dT = vec4(3*pow(t.y, 2), 2*t.y, 1, 0);
	
	mat4 temp = getZ_z(curveIndex.x, curveIndex.y);
	
	vec3 inVertex;

	
	inVertex.x = dot(S * M_b * Z_xNew * transpose(M_b), T);
	inVertex.y = dot(S * M_b * Z_yNew * transpose(M_b), T);
	inVertex.z = dot(S * M_b *  temp * transpose(M_b), T);
	
	vec3 inNormalS;
	vec3 inNormalT;
	vec3 inNormal;
	
	inNormalS.x = dot(dS * M_b * Z_xNew * transpose(M_b), T);
	inNormalS.y = dot(dS * M_b * Z_yNew * transpose(M_b), T);
	inNormalS.z = dot(dS * M_b * temp * transpose(M_b), T);
	
	inNormalT.x = dot(S * M_b * Z_xNew * transpose(M_b), dT);
	inNormalT.y = dot(S * M_b * Z_yNew * transpose(M_b), dT);
	inNormalT.z = dot(S * M_b * temp * transpose(M_b), dT);
	
	inNormal = cross(inNormalS, inNormalT);
	
	vec4 pWorld = modelingMatrix * vec4(inVertex, 1);
	vec3 nWorld = inverse(transpose(mat3x3(modelingMatrix))) * inNormal;
	
	pos = vec3(pWorld);
	norm = nWorld;

    gl_Position = projectionMatrix * viewingMatrix * pWorld;
}
