#version 430

layout(location=0) out vec4 c;
in vec4 color;

void main()
{
	c = color;
}