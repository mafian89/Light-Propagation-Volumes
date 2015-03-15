#version 430
//#extension GL_AMD_vertex_shader_layer : require

layout(location = 0) in vec2 vPosition;


void main()
{
	gl_Position =  vec4(vPosition,0.0,1.0);
	//gl_Layer = 0;
}