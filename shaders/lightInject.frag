#version 430

#extension GL_NV_shader_atomic_float : require
#extension GL_NV_shader_atomic_fp16_vector : require
#extension GL_NV_gpu_shader5 : require

layout(rgba16f ,location = 0) uniform image3D LPVGridR;
layout(rgba16f ,location = 1) uniform image3D LPVGridG;
layout(rgba16f ,location = 2) uniform image3D LPVGridB;
layout(early_fragment_tests )in;//turn on early depth tests

uniform vec3 v_gridDim;
uniform float f_cellSize;
uniform vec3 v_min; //min corner of the volume

flat in ivec3 v_volumeCellIndex;
in vec3 v_posFromRSM;

void main()
{
	//	output.cellIndex = float4( int3( (posWorld - lpv_minCorner) / lpv_cellSize + 0.5 * normalWorld ), 1.0 );

	imageAtomicAdd(LPVGridR,v_volumeCellIndex,f16vec4(v_posFromRSM,1.0));
	imageAtomicAdd(LPVGridG,v_volumeCellIndex,f16vec4(0.0,1.0,0.0,1.0));
	imageAtomicAdd(LPVGridB,v_volumeCellIndex,f16vec4(0.0,0.0,1.0,1.0));
}