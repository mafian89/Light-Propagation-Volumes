#version 430

in vec4 pos;
in vec3 eyeNormal;
in vec2 uv;

uniform sampler2D matTex;

layout(location=0) out vec4 worldPos;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 flux;
//layout(location=3) out float fragmentdepth;

void main() {
	float uniformFlux = 1.0f;
	float matRefl = 0.01;
	float resFlux = uniformFlux*matRefl;
	vec3 diffuse = texture(matTex,uv).rgb;

	worldPos = pos;
	normal = vec4(eyeNormal,1.0);
	flux = vec4(diffuse,resFlux);
	//fragmentdepth = gl_FragCoord.z;
}