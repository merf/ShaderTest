uniform float time;

uniform vec4 LightPosition;

varying vec3 WorldNormal;
varying vec3 WorldVertexPos;
varying vec3 EyeVec;
varying vec3 LightVec;
 
void main()
{
	// Transforming The Normal To ModelView-Space
    WorldNormal = gl_NormalMatrix * gl_Normal;

	// Transform vertex to clip space.
	gl_Position =  gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

	// Store world space vertex pos
	WorldVertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;

    // Calculating The Vector From The Vertex Position To The Light Position
    LightVec = vec3(LightPosition.xyz - WorldVertexPos);
 
	EyeVec = -WorldVertexPos;

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	gl_FrontColor = gl_Color;
}