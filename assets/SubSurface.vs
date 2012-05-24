//////////////////////////////////////////////////////////////////////////
// SUB-SURFACE SCATTER VS												//
// http://www.gamedev.net/community/forums/topic.asp?topic_id=481494	//
//////////////////////////////////////////////////////////////////////////

uniform mat4 PrevModelView;
uniform mat4 ModelView;

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
	Normal = gl_NormalMatrix * gl_Normal;

	RawPos = gl_Vertex;

	//Transform vertex by modelview and projection matrices
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	VertPos = gl_ModelViewMatrix * gl_Vertex;
	LightPos0 = gl_ModelViewMatrix * gl_LightSource[0].position;
	LightPos1 = gl_ModelViewMatrix * gl_LightSource[1].position;

	EyeVec = -VertPos;

	PixelVelocity = CalcPixVel(gl_Vertex);

	//Forward current texture coordinates after applying texture matrix
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}