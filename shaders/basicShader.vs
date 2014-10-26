#version 430

uniform mat4 mvp;
layout(location = 0) in vec3 vPosition;

void main()
{
	gl_Position = mvp * vec4(vPosition,1.0);
}