#version 430

#define USESAMPLER3D

#ifndef USESAMPLER3D
	#extension GL_NV_shader_atomic_float : require
	#extension GL_NV_shader_atomic_fp16_vector : require
	#extension GL_NV_gpu_shader5 : require
#endif

#ifdef USESAMPLER3D
	//uniform sampler3D AccumulatorLPV;
	uniform sampler3D RAccumulatorLPV;
	uniform sampler3D GAccumulatorLPV;
	uniform sampler3D BAccumulatorLPV;
#else
	//layout(rgba16f ,location = 0) uniform image3D AccumulatorLPV;
	layout(rgba16f ,location = 0) uniform image3D RAccumulatorLPV;
	layout(rgba16f ,location = 1) uniform image3D GAccumulatorLPV;
	layout(rgba16f ,location = 2) uniform image3D BAccumulatorLPV;
	layout(early_fragment_tests )in;//turn on early depth tests
#endif



//For MRT attachment 0
layout(location = 0) out vec4 final_color;
layout(location = 1) out vec4 normals;
in vec2 uv;
in vec3 eyePosition,eyeNormal,eyeLightPos;
in vec3 worldPos, worldNorm;
in vec4 shadowCoord;

uniform sampler2D tex;
uniform sampler2DShadow depthTexture;
uniform vec3 v_gridDim;
uniform float f_cellSize;
uniform vec3 v_min; //min corner of the volume
uniform float f_indirectAttenuation;

/*Spherical harmonics coefficients - precomputed*/
#define SH_C0 0.282094792f // 1 / 2sqrt(pi)
#define SH_C1 0.488602512f // sqrt(3/pi) / 2

// no normalization
vec4 evalSH_direct( vec3 direction ) {	
	return vec4( SH_C0, -SH_C1 * direction.y, SH_C1 * direction.z, -SH_C1 * direction.x );
}

// ch - channel - 0 - red, 1 - green, 2 - blue 
ivec3 getTextureCoordinatesForGrid(ivec3 c, int ch) {
	if(ch > 2 || ch < 0)
		return ivec3(0,0,0);

	return ivec3(c.x, c.y + ch * v_gridDim.y, c.z);
}
void main()
{
	float shadow = 1.0;
	vec4 coord = shadowCoord / shadowCoord.w;
	if (shadowCoord.w > 0.0) {
		shadow = texture(depthTexture, vec3(coord.xyz)-vec3(0.00001));
		//textureProj does perspective division for me
		//shadow = textureProj(depthTexture, shadowCoord);
		shadow = (shadow > 0) ? 1.0 : 0.1;
	}
	float distance = length(eyeLightPos.xyz-eyePosition.xyz);
	float att=1.0/(0.0005+0.009*distance+0.00035*distance*distance);

	vec3 ld = vec3(1.0,1.0,1.0);
	vec3 ls = vec3(1.0,1.0,1.0);
	vec3 la = vec3(1.0,1.0,1.0);

	vec3 kd = texture(tex, uv).rgb;
	vec3 ks = vec3(0.8,0.8,0.8);
	vec3 ka = vec3(0.1,0.1,0.1);
	vec3 tmpNormal = normalize(eyeNormal);
	vec3 s = normalize(vec3(eyeLightPos - eyePosition));
	vec3 v = normalize(-eyePosition);
	vec3 r = reflect (-s,tmpNormal); 
	float sDotN = max(dot(s, tmpNormal),0.0);
	vec3 diffuse = ld * kd * sDotN; // * kd
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0) {
	    spec = ls * ks *pow(max(dot(r,v),0.0),8);
	}
	vec3 ambient = la * ka;
	


	vec4 SHintensity = evalSH_direct( -worldNorm );
	#ifdef USESAMPLER3D
		vec3 lpvCellCoords = (worldPos - v_min) / f_cellSize / v_gridDim; //<0,1>
		//lpvCellCoords = clamp(lpvCellCoords, 0.0, 1.0);
		vec3 lpvIntensity = vec3( 
			dot( SHintensity, texture( RAccumulatorLPV, lpvCellCoords) ),
			dot( SHintensity, texture( GAccumulatorLPV, lpvCellCoords ) ),
			dot( SHintensity, texture( BAccumulatorLPV, lpvCellCoords ) )
		);
	#else
		vec3 lpvCellCoords = (worldPos - v_min) / f_cellSize;// / v_gridDim;
		vec3 lpvIntensity = vec3( 
			dot( SHintensity, imageLoad( RAccumulatorLPV, ivec3(lpvCellCoords) ) ),
			dot( SHintensity, imageLoad( GAccumulatorLPV, ivec3(lpvCellCoords) ) ),
			dot( SHintensity, imageLoad( BAccumulatorLPV, ivec3(lpvCellCoords) ) )
		);
	#endif

	vec3 finalLPVRadiance = f_indirectAttenuation*  max( lpvIntensity, 0 ); //* 4 / f_cellSize / f_cellSize

	//vec3 lightIntesity =  shadow*(ambient + diffuse + spec)*att;
	vec3 lightIntesity =  sDotN * la * kd * shadow + la * spec * shadow + kd * finalLPVRadiance;
	final_color = vec4(lightIntesity,1.0);

	normals = vec4(tmpNormal,1.0);

    //final_color = texture2D(texture, uv);
	//final_color = vec4(1.0,0.0,0.0,1.0);
}