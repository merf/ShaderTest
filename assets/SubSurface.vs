//////////////////////////////////////////////////////////////////////////
// SUB-SURFACE SCATTER VS												//
// http://www.gamedev.net/community/forums/topic.asp?topic_id=481494	//
//////////////////////////////////////////////////////////////////////////
#extension GL_EXT_gpu_shader4 : enable

uniform mat4 PrevModelView;
uniform mat4 ModelView;
uniform mat4 BoneTransforms[3];
uniform mat4 BindPose;

uniform float time;
uniform float JiggleResolution;

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

void main()
{
	//Normal = gl_NormalMatrix * gl_Normal;

	RawPos = gl_Vertex;

	float f = 100 + RawPos.y * JiggleResolution + sin(time) * 0.5;

	int i = int(f);

	int bone_index_0 = i % 3;
	int bone_index_1 = (bone_index_0 + 1) % 3;

	float interp = smoothstep(0.0, 1.0, f - float(i));

	vec3 in_vertex_pos = gl_Vertex.xyz;
	vec3 in_normal = gl_Normal;

	vec3 p0 = (vec4(in_vertex_pos, 1.0) * BoneTransforms[bone_index_0]).xyz;
	vec3 p1 = (vec4(in_vertex_pos, 1.0) * BoneTransforms[bone_index_1]).xyz;

	vec3 n0 = (vec4(in_normal, 0.0) * BoneTransforms[bone_index_0]).xyz;
	vec3 n1 = (vec4(in_normal, 0.0) * BoneTransforms[bone_index_1]).xyz;
		
	vec4 pos = vec4(mix(p0, p1, interp), 1.0);

	vec3 normal = mix(n0, n1, interp);

	Normal = gl_NormalMatrix * normal;

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