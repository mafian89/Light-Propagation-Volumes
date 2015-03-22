#version 430
//#extension GL_AMD_vertex_shader_layer : require

layout(location = 0) in vec2 vPosition;

uniform sampler2D rsm_world_space_coords_tex;
uniform sampler2D rsm_normal_tex;
uniform sampler2D rsm_flux_tex;

uniform int i_RSMsize;
flat out ivec3 v_volumeCellIndex;
out vec3 v_posFromRSM;

void main()
{
	ivec2 v_RSMCoords = ivec2(gl_VertexID % i_RSMsize, gl_VertexID / i_RSMsize);
	v_volumeCellIndex = ivec3(v_RSMCoords,0.0); //JUST TEST VALUE FOR NOW!!!
	v_posFromRSM = texelFetch(rsm_world_space_coords_tex, v_RSMCoords,0).rgb;
	//v_posFromRSM = texelFetch(rsm_world_space_coords_tex, ivec2(5,7),0).rgb;
	gl_Position =  vec4(vPosition,0.0,1.0);
}