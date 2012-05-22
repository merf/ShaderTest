//////////////////////////////////////////////////////////////////////////
// SUB-SURFACE SCATTER VS												//
// http://www.gamedev.net/community/forums/topic.asp?topic_id=481494	//
//////////////////////////////////////////////////////////////////////////

//varying vec3 EyeVec;
//varying vec3 LightPos;
//varying vec3 LightVec;

varying vec3 Normal;
varying vec4 VertPos;
varying vec4 RawPos;

varying vec4 LightPos0;
varying vec4 LightPos1;
varying vec4 EyeVec;

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

	//Forward current texture coordinates after applying texture matrix
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}