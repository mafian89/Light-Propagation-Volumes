#version 430

in vec4 pos;
in vec3 eyeNormal;

layout(location=0) out vec4 worldPos;
layout(location=1) out vec4 normal;
layout(location=2) out float fragmentdepth;

void main() {
	worldPos = pos;
	normal = vec4(eyeNormal,1.0);
	fragmentdepth = gl_FragCoord.z;
}