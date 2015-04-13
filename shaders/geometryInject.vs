#version 430

layout(location = 0) in vec2 vPosition;

uniform sampler2D rsm_world_space_coords_tex;
uniform sampler2D rsm_normal_tex;

uniform mat4 m_lightView;
uniform int i_RSMsize;
uniform float f_cellSize;
uniform vec3 v_min; //min corner of the volume
uniform vec3 v_lightPos;
uniform float f_tanFovXHalf;
uniform float f_tanFovYHalf;
uniform float f_texelAreaModifier;

flat out ivec3 v_volumeCellIndex;
out vec3 v_posFromRSM;
out vec3 v_normalFromRSM;
out float surfelArea;

ivec3 convertPointToGridIndex(vec3 pos) {
	return ivec3((pos - v_min) / f_cellSize - vec3( 0.5, 0.5, 0.5 )); //shift by half a cell
}

//Sample from camera
float calculateSurfelAreaLightViewM(vec3 viewPos) {
	return (4.0 * viewPos.z * viewPos.z * f_tanFovXHalf * f_tanFovYHalf)/(i_RSMsize * i_RSMsize);
}

//Sample from light
float calculateSurfelAreaLight(vec3 lightPos) {
	return (4.0 * lightPos.z * lightPos.z * f_tanFovXHalf * f_tanFovYHalf)/(i_RSMsize * i_RSMsize);
}

//This function and function above gives the same results - because I have 90 degree fov -> tan(45.0) = 1;
float calculateSurfelAreaLightOrtho(vec3 lightPos) {
	return (4.0 * lightPos.z * lightPos.z)/(i_RSMsize * i_RSMsize);
}

//#define LIGHTVIEW

void main()
{
	ivec2 v_RSMCoords = ivec2(gl_VertexID % i_RSMsize, gl_VertexID / i_RSMsize);

	v_posFromRSM = texelFetch(rsm_world_space_coords_tex, v_RSMCoords,0).rgb;
	v_normalFromRSM = texelFetch(rsm_normal_tex, v_RSMCoords,0).rgb;

	#ifdef LIGHTVIEW
		vec4 viewPos = m_lightView * vec4(v_posFromRSM,1.0);
		surfelArea = calculateSurfelAreaLightViewM(viewPos.xyz) *  f_texelAreaModifier;
	#else
		surfelArea = calculateSurfelAreaLight(v_lightPos) * f_texelAreaModifier;
		//surfelArea = calculateSurfelAreaLightOrtho(v_lightPos)* f_texelAreaModifier;
	#endif

	v_volumeCellIndex = convertPointToGridIndex(v_posFromRSM);

	gl_Position =  vec4(vPosition,0.0,1.0);
}