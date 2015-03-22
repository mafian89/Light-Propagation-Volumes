#version 430

#extension GL_NV_shader_atomic_float : require
#extension GL_NV_shader_atomic_fp16_vector : require
#extension GL_NV_gpu_shader5 : require

layout(rgba16f ,location = 0) uniform image3D LPVGridR;
layout(rgba16f ,location = 1) uniform image3D LPVGridG;
layout(rgba16f ,location = 2) uniform image3D LPVGridB;
layout(early_fragment_tests )in;//turn on early depth tests

uniform vec3 v_gridDim;


flat in ivec3 v_volumeCellIndex;
in vec3 v_posFromRSM;
in vec3 v_normalFromRSM;
in vec4 v_fluxFromRSM;

void main()
{

	/*if( length( v_normalFromRSM ) < 0.01 ) {
		discard;
	}*/

	imageAtomicAdd(LPVGridR,v_volumeCellIndex,f16vec4(v_posFromRSM,1.0));
	imageAtomicAdd(LPVGridG,v_volumeCellIndex,f16vec4(0.0,1.0,0.0,1.0));
	imageAtomicAdd(LPVGridB,v_volumeCellIndex,f16vec4(0.0,0.0,1.0,1.0));
}