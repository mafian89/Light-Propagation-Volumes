#version 430
#extension GL_NV_shader_atomic_float : require
#extension GL_NV_shader_atomic_fp16_vector : require
#extension GL_NV_gpu_shader5 : require

layout(rgba16f ,location = 0) uniform image3D AccumulatorLPV;
layout(rgba16f ,location = 1) uniform image3D LightGrid;
layout(rgba16f ,location = 2) uniform image3D LightGridForNextStep;
layout(early_fragment_tests )in;//turn on early depth tests

uniform bool b_firstPropStep;
uniform vec3 v_gridDim; //Resolution of the grid
flat in ivec3 cellIndex;

const float directFaceSubtendedSolidAngle = 0.03188428; // 0.4006696846f / 4Pi;
const float sideFaceSubtendedSolidAngle = 0.03369559; // 0.4234413544f / 4Pi;

#define PI 3.1415926f

/*Spherical harmonics coefficients - precomputed*/
#define SH_C0 0.282094792f // 1 / 2sqrt(pi)
#define SH_C1 0.488602512f // sqrt(3/pi) / 2

/*Cosine lobe coeff*/
#define SH_cosLobe_C0 0.886226925f // sqrt(pi)/2 
#define SH_cosLobe_C1 1.02332671f // sqrt(pi/3) 

// SH_C0 * SH_cosLobe_C0 = 0.25000000007f
// SH_C1 * SH_cosLobe_C1 = 0.5000000011f

// no normalization
vec4 evalSH_direct( vec3 direction ) {	
	return vec4( SH_C0, -SH_C1 * direction.y, SH_C1 * direction.z, -SH_C1 * direction.x );
}

// no normalization
vec4 evalCosineLobeToDir_direct( vec3 direction ) {
	return vec4( SH_cosLobe_C0, -SH_cosLobe_C1 * direction.y, SH_cosLobe_C1 * direction.z, -SH_cosLobe_C1 * direction.x );
}


bool isInside(ivec3 i) {
	if(i.x < 0 || i.x > int(v_gridDim.x))
		return false;
	if(i.y < 0 || i.y > int(v_gridDim.y))
		return false;
	if(i.z < 0 || i.z > int(v_gridDim.z))
		return false;
	return true;
}

const ivec3 propDirections[6] = {
	//+Z
	ivec3(0,0,1),
	//-Z
	ivec3(0,0,-1),
	//+X
	ivec3(1,0,0),
	//-X
	ivec3(-1,0,0),
	//+Y
	ivec3(0,1,0),
	//-Y
	ivec3(0,-1,0)
};

// ch - channel - 0 - red, 1 - green, 2 - blue 
ivec3 getTextureCoordinatesForGrid(ivec3 c, int ch) {
	if(ch > 2 || ch < 0)
		return ivec3(0,0,0);

	return ivec3(c.x, c.y + ch * v_gridDim.y, c.z);
}

//Sides of the cell - right, top, left, bottom
ivec2 cellSides[4] = { ivec2( 1.0, 0.0 ), ivec2( 0.0, 1.0 ), ivec2( -1.0, 0.0 ), ivec2( 0.0, -1.0 ) };

//Get side direction
vec3 getEvalSideDirection( int index, ivec3 orientation ) {
	const float smallComponent = 0.4472135; // 1 / sqrt(5)
	const float bigComponent = 0.894427; // 2 / sqrt(5)
	
	const ivec2 side = cellSides[ index ];
	vec3 tmp = vec3( side.x * smallComponent, side.y * smallComponent, bigComponent );
	return vec3(orientation.x * tmp.x, orientation.y * tmp.y, orientation.z * tmp.z);
}

vec3 getReprojSideDirection( int index, ivec3 orientation ) {
	const ivec2 side = cellSides[ index ];
	return vec3( orientation.x*side.x, orientation.y*side.y, 0 );
}

struct contribution {
	vec4 R,G,B;
};

void propagate() {
	contribution c;
	c.R = vec4(0.0);
	c.G = vec4(0.0);
	c.B = vec4(0.0);

	for(int neighbour = 0; neighbour < 6; neighbour++) {
		vec4 RSHcoeffsNeighbour = vec4(0.0);
		vec4 GSHcoeffsNeighbour = vec4(0.0);
		vec4 BSHcoeffsNeighbour = vec4(0.0);
		//Get main direction
		ivec3 mainDirection = propDirections[neighbour]; 
		//get neighbour cell indexindex
		ivec3 neighbourCellIndex = cellIndex - mainDirection;
		//Load sh coeffs - CHECK THIS ONCE AGAIN!!!
		//if(isInside(neighbourCellIndex)) {
			RSHcoeffsNeighbour = imageLoad(LightGrid, getTextureCoordinatesForGrid(neighbourCellIndex, 0));
			GSHcoeffsNeighbour = imageLoad(LightGrid, getTextureCoordinatesForGrid(neighbourCellIndex, 1));
			BSHcoeffsNeighbour = imageLoad(LightGrid, getTextureCoordinatesForGrid(neighbourCellIndex, 2));
		/*}else{
			break;
		}*/

		float occlusionValue = 1.0; // no occlusion
		//TODO: Occlusion!!!!

		float occludedDirectFaceContribution = occlusionValue * directFaceSubtendedSolidAngle;

		vec4 mainDirectionCosineLobeSH = evalCosineLobeToDir_direct( mainDirection );
		vec4 mainDirectionSH = evalSH_direct( mainDirection );
		c.R += occludedDirectFaceContribution * dot( RSHcoeffsNeighbour, mainDirectionSH ) * mainDirectionCosineLobeSH;
		c.G += occludedDirectFaceContribution * dot( GSHcoeffsNeighbour, mainDirectionSH ) * mainDirectionCosineLobeSH;
		c.B += occludedDirectFaceContribution * dot( BSHcoeffsNeighbour, mainDirectionSH ) * mainDirectionCosineLobeSH;

		//Now we have contribution for the neighbour's cell in the main direction -> need to do reprojection 
		//Reprojection will be made only onto 4 faces (acctually we need to take into account 5 faces but we already have the one in main direction)

		for(int face = 0; face < 4; face++) {
			//Get the direction to the face
			vec3 evalDirection = getEvalSideDirection( face, mainDirection );
			//Reprojected direction
			vec3 reprojDirection = getReprojSideDirection( face, mainDirection );

			//TODO: Occlusion!!!!

			float occludedSideFaceContribution = occlusionValue * sideFaceSubtendedSolidAngle;
			
			//Get sh coeff
			vec4 reprojDirectionCosineLobeSH = evalCosineLobeToDir_direct( reprojDirection );
			vec4 evalDirectionSH = evalSH_direct( evalDirection );
			
			c.R += occludedSideFaceContribution * dot( RSHcoeffsNeighbour, evalDirectionSH ) * reprojDirectionCosineLobeSH;
			c.G += occludedSideFaceContribution * dot( GSHcoeffsNeighbour, evalDirectionSH ) * reprojDirectionCosineLobeSH;
			c.B += occludedSideFaceContribution * dot( BSHcoeffsNeighbour, evalDirectionSH ) * reprojDirectionCosineLobeSH;

		}
	}

	//REWRITE
	imageAtomicAdd(AccumulatorLPV, getTextureCoordinatesForGrid(cellIndex, 0),f16vec4(c.R));
	imageAtomicAdd(AccumulatorLPV, getTextureCoordinatesForGrid(cellIndex, 1),f16vec4(c.G));
	imageAtomicAdd(AccumulatorLPV, getTextureCoordinatesForGrid(cellIndex, 2),f16vec4(c.B));
}

void main()
{
	propagate();
	//vec4 Rchannel = imageLoad(LightGrid, cellIndex);
	//imageAtomicAdd(AccumulatorLPV, cellIndex,f16vec4(Rchannel));
}