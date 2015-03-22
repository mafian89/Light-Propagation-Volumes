#version 430
//#extension GL_AMD_vertex_shader_layer : require

layout(location = 0) in vec2 vPosition;

uniform int i_RSMsize;
flat out ivec3 v_volumeCellIndex;

void main()
{
	ivec2 v_RSMCoords = ivec2(gl_VertexID % i_RSMsize, gl_VertexID / i_RSMsize);
	v_volumeCellIndex = ivec3(v_RSMCoords,0.0); //JUST TEST VALUE FOR NOW!!!
	gl_Position =  vec4(vPosition,0.0,1.0);
}