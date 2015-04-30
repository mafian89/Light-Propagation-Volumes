#version 430

layout(location = 0) in vec2 vPosition;

uniform sampler2D rsm_world_space_coords_tex;
uniform sampler2D rsm_normal_tex;
uniform sampler2D rsm_flux_tex;

uniform int i_RSMsize;
uniform float f_cellSize;
uniform vec3 v_min; //min corner of the volume
uniform vec3 v_gridDim;

flat out ivec3 v_volumeCellIndex;
out vec3 v_posFromRSM;
out vec3 v_normalFromRSM;
out vec4 v_fluxFromRSM;


ivec3 convertPointToGridIndex(vec3 pos, vec3 normal) {
	return ivec3((pos - v_min) / f_cellSize + 0.5 * normal);
}

void main()
{
	ivec2 v_RSMCoords = ivec2(gl_VertexID % i_RSMsize, gl_VertexID / i_RSMsize);

	v_posFromRSM = texelFetch(rsm_world_space_coords_tex, v_RSMCoords,0).rgb;
	v_normalFromRSM = texelFetch(rsm_normal_tex, v_RSMCoords,0).rgb;
	v_fluxFromRSM = texelFetch(rsm_flux_tex, v_RSMCoords,0);

	v_volumeCellIndex = convertPointToGridIndex(v_posFromRSM,v_normalFromRSM);

	//gl_Position =  vec4((v_volumeCellIndex.x/v_gridDim.x)*2 - 1.0,(v_volumeCellIndex.y/v_gridDim.y)*2 - 1.0,0.0,1.0);
	vec2 screenPos = (vec2(v_volumeCellIndex.xy) + 0.5) / v_gridDim.xy * 2.0 - 1.0;
	gl_Position = vec4(screenPos , 0.0,1.0);
}