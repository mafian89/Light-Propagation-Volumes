#version 430
/*
#extension GL_NV_shader_atomic_float : require
#extension GL_NV_shader_atomic_fp16_vector : require
#extension GL_NV_gpu_shader5 : require

layout(rgba16f ,location = 0) uniform image3D LPVGridR;
layout(rgba16f ,location = 1) uniform image3D LPVGridG;
layout(rgba16f ,location = 2) uniform image3D LPVGridB;
layout(early_fragment_tests )in;//zapneme early fragment testy
*/

layout(location=0) out vec4 c;
in vec4 color;

/*
vec3 max = vec3(27.0283,21.4704,16.5955);
vec3 min = vec3(-28.7844,-1.86771,-17.7279);
vec3 volSize = max - min;
float cellSize = 0.872074;*/

void main()
{
	/*int x = int((gl_FragCoord.x - min.x) / cellSize);
	int y = int((gl_FragCoord.y - min.y) / cellSize);
	int z = int((gl_FragCoord.z - min.z) / cellSize);

	//imageAtomicAdd(LPVGridR,ivec3(x,y,z),1);*/

	/*imageAtomicAdd(LPVGridR,ivec3(0,0,0),f16vec4(1.0,0.0,0.0,1.0));
	imageAtomicAdd(LPVGridG,ivec3(0,0,0),f16vec4(0.0,1.0,0.0,1.0));
	imageAtomicAdd(LPVGridB,ivec3(0,0,0),f16vec4(0.0,0.0,1.0,1.0));*/

	//imageStore(LPVGridR,ivec3(0,0,0), imageLoad(LPVGridR, ivec3(0,0,0)) + vec4(1.0,0.0,0.0,1.0));

	c = color;
}