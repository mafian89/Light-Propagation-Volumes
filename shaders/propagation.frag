#version 430
#extension GL_NV_shader_atomic_float : require
#extension GL_NV_shader_atomic_fp16_vector : require
#extension GL_NV_gpu_shader5 : require

layout(rgba16f ,location = 0) uniform image3D AccumulatorLPV;
layout(rgba16f ,location = 1) uniform image3D LightGrid;
layout(rgba16f ,location = 2) uniform image3D LightGridForNextStep;
layout(early_fragment_tests )in;//turn on early depth tests

uniform bool b_firstPropStep;
flat in ivec3 cellIndex;

void main()
{
	vec4 Rchannel = imageLoad(LightGrid, cellIndex);
	imageAtomicAdd(AccumulatorLPV, cellIndex,f16vec4(Rchannel));
}