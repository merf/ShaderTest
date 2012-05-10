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

void subScatterVS(in vec4 ecVert)
{
	lightVec = LightPosition - ecVert.xyz;
	eyeVec = -ecVert.xyz;
	vertPos = ecVert.xyz;
	lightPos = gl_LightSource[0].position.xyz;
}

////////////////
//  MAIN LOOP //
////////////////

void main()
{
	worldNormal = gl_NormalMatrix * gl_Normal;
	
	vec4 ecPos = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// Call function to set varyings for subscatter FS
	subScatterVS(ecPos);
	
	//Transform vertex by modelview and projection matrices
	gl_Position = ecPos;

	//Forward current texture coordinates after applying texture matrix
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}