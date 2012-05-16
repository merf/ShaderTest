////////////////////////////
// SUB-SURFACE SCATTER VS //
////////////////////////////

/* --------------------------
SubScatter Vertex Shader:

Fake sub-surface scatter lighting shader by InvalidPointer 2008.
Found at
http://www.gamedev.net/community/forums/topic.asp?topic_id=481494

HLSL > GLSL translation
toneburst 2008
-------------------------- */

// Set light-position
uniform vec3 LightPosition;

// Varying variables to be sent to Fragment Shader
varying vec3 worldNormal, eyeVec, lightVec, vertPos, lightPos;

varying vec3 normal;
varying vec4 pos;

void subScatterVS(in vec4 ecVert)
{
	lightPos = LightPosition;
	lightVec = lightPos - pos;
	eyeVec = -pos;
	vertPos = pos;
}

////////////////
//  MAIN LOOP //
////////////////

void main()
{
	normal = gl_NormalMatrix * gl_Normal;
	pos = gl_Vertex;

	worldNormal = gl_NormalMatrix * gl_Normal;
	
	vec4 ecPos = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// Call function to set varyings for subscatter FS
	subScatterVS(ecPos);
	
	//Transform vertex by modelview and projection matrices
	gl_Position = ecPos;

	//Forward current texture coordinates after applying texture matrix
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}