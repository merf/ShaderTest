varying vec3 normal;
varying vec4 pos;
varying vec4 rawpos;
varying vec3 eyedir;

void main() {
	normal = gl_NormalMatrix * gl_Normal;
	gl_Position = ftransform();
	pos = gl_ModelViewMatrix * gl_Vertex;
	rawpos = gl_Vertex;
	eyedir = -pos.xyz;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}