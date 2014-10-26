#version 430

uniform mat4 mvp;
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUv;

out vec2 uv;

void main()
{
	gl_Position = mvp * vec4(vPosition,1.0);
	uv = vUv;
}