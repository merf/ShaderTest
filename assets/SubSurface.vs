//////////////////////////////////////////////////////////////////////////
// SUB-SURFACE SCATTER VS												//
// http://www.gamedev.net/community/forums/topic.asp?topic_id=481494	//
//////////////////////////////////////////////////////////////////////////

uniform mat4 PrevModelView;
uniform mat4 ModelView;
uniform mat4 BoneTransforms[3];
uniform mat4 BindPose;

varying vec3 Normal;
varying vec4 VertPos;
varying vec4 RawPos;

varying vec4 LightPos0;
varying vec4 LightPos1;
varying vec4 EyeVec;

varying vec4 PixelVelocity;

vec4 CalcPixVel(in vec4 ws_vertex_pos)
{
	vec4 curr_pos_ss = (gl_ProjectionMatrix * ModelView * ws_vertex_pos);
	vec4 prev_pos_ss = (gl_ProjectionMatrix * PrevModelView * ws_vertex_pos);

	//return vec4(length(curr_pos_ss-prev_pos_ss));// / 10.0;
	return ((curr_pos_ss-prev_pos_ss) + vec4(0.5, 0.5, 0.0, 1.0));// * 10.0;
}

float dot_product(in vec3 first, in vec3 second)
{
   return first.x * second.x + first.y * second.y +
         first.z * second.z;
}

mat4 orthoNormalInverse( in mat4 Matrix)
{
   mat4 temp = Matrix;
   
   //(xx, yx, zx, 0,
   //xy, yy, zy, 0,
   //xz, yz, zz, 0,
   //-(wx * xx + wy * xy + wz * xz),
   //-(wx * yx + wy * yy + wz * yz),
   //-(wx * zx + wy * zy + wz * zz), 1);
   
   vec3 wAxis = vec3(Matrix[0][3], Matrix[1][3], Matrix[2][3]);
   vec3 xAxis = vec3(Matrix[0][0], Matrix[1][0], Matrix[2][0]);
   vec3 yAxis = vec3(Matrix[0][1], Matrix[1][1], Matrix[2][1]);
   vec3 zAxis = vec3(Matrix[0][2], Matrix[1][2], Matrix[2][2]);

   //wx
   //wy
   //wz
   temp[3][0] = -dot_product(wAxis, xAxis);   
   temp[3][1] = -dot_product(wAxis, yAxis);
   temp[3][2] = -dot_product(wAxis, zAxis);
   temp[3][3] = 1.0;

   return temp;
}

vec3 VectorByMatrixMult(in vec3 Vector, in mat4 Matrix)
{
   vec3 outVec;
	outVec.x = Vector.x * Matrix[0][0] + Vector.y * Matrix[0][1] + Vector.z * Matrix[0][2] + Matrix[0][3];
	outVec.y = Vector.x * Matrix[1][0] + Vector.y * Matrix[1][1] + Vector.z * Matrix[1][2] + Matrix[1][3];
	outVec.z = Vector.x * Matrix[2][0] + Vector.y * Matrix[2][1] + Vector.z * Matrix[2][2] + Matrix[2][3];
   return outVec;
}

void main()
{
	Normal = gl_NormalMatrix * gl_Normal;

	RawPos = gl_Vertex;

	mat4 BoneTransform = BoneTransforms[0];
	float weight = 0.001;

	vec3 in_vertex_pos = gl_Vertex.xyz;

	vec3 p1 = vec3(0.0);
	//pos = in_vertex_pos;

	mat4 bindPoseInv = orthoNormalInverse(BindPose);

	vec3 BindTimesVert = VectorByMatrixMult(in_vertex_pos, bindPoseInv);
	
	for(int i=0; i<2; ++i)
	{
		vec3 CurrentTimesBindTimesVert = VectorByMatrixMult(BindTimesVert, BoneTransforms[i]);
		CurrentTimesBindTimesVert = CurrentTimesBindTimesVert * 0.5;
		p1 += CurrentTimesBindTimesVert;
	}
		
	vec4 pos = vec4(p1, 1.0);
	
	//Transform vertex by modelview and projection matrices
	gl_Position = gl_ModelViewProjectionMatrix * pos;
	
	VertPos = gl_ModelViewMatrix * pos;


	LightPos0 = gl_ModelViewMatrix * gl_LightSource[0].position;
	LightPos1 = gl_ModelViewMatrix * gl_LightSource[1].position;

	EyeVec = -VertPos;

	PixelVelocity = CalcPixVel(gl_Vertex);

	//Forward current texture coordinates after applying texture matrix
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}