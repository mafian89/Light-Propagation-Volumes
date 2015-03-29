#version 430

layout(location = 0) in vec2 vPosition;

uniform sampler2D rsm_world_space_coords_tex;
uniform sampler2D rsm_normal_tex;

uniform mat4 m_lightView;
uniform int i_RSMsize;
uniform float f_cellSize;
uniform vec3 v_min; //min corner of the volume
uniform vec3 v_lightPos;
uniform float f_tanLightFovXHalf;
uniform float f_tanLightFovYHalf;

flat out ivec3 v_volumeCellIndex;
out vec3 v_posFromRSM;
out vec3 v_normalFromRSM;
out float surfelArea;

ivec3 convertPointToGridIndex(vec3 pos, vec3 normal) {
	return ivec3((pos - v_min) / f_cellSize - vec3( 0.5, 0.5, 0.5 )); //shift by half a cell
}

//Sample from camera
float calculateSurfelAreaCamera(vec3 viewPos) {
	return (4.0 * viewPos.z * viewPos.z * f_tanLightFovXHalf * f_tanLightFovYHalf)/(i_RSMsize * i_RSMsize);
}

//Sample from light
float calculateSurfelAreaLight(vec3 lightPos) {
	return (4.0 * lightPos.z * lightPos.z * f_tanLightFovXHalf * f_tanLightFovYHalf)/(i_RSMsize * i_RSMsize);
}

float calculateSurfelAreaLightDEBUG(vec3 lightPos) {
	return (4.0 * lightPos.z * lightPos.z)/(i_RSMsize * i_RSMsize);
}

#define LIGHT

void main()
{
	ivec2 v_RSMCoords = ivec2(gl_VertexID % i_RSMsize, gl_VertexID / i_RSMsize);

	v_posFromRSM = texelFetch(rsm_world_space_coords_tex, v_RSMCoords,0).rgb;
	v_normalFromRSM = texelFetch(rsm_normal_tex, v_RSMCoords,0).rgb;

	#ifndef LIGHT
		vec4 viewPos = m_lightView * vec4(v_posFromRSM,1.0);
		surfelArea = calculateSurfelAreaCamera(viewPos.xyz);
	#else
		surfelArea = calculateSurfelAreaLight(v_lightPos);
		//surfelArea = calculateSurfelAreaLightDEBUG(v_lightPos);
	#endif

	v_volumeCellIndex = convertPointToGridIndex(v_posFromRSM,v_normalFromRSM);

	gl_Position =  vec4(vPosition,0.0,1.0);
}