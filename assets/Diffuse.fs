uniform vec2 resolution;
uniform float time;
uniform sampler2D tex0;

varying vec3 WorldNormal;
varying vec3 WorldVertexPos;
varying vec3 EyeVec;
varying vec3 LightVec;

float halfLambert(in vec3 vect1, in vec3 vect2)
{
	float product = dot(vect1,vect2);
	return product * 0.5 + 0.5;
}

void main()
{
	vec3 light_dir = normalize(LightVec);
	vec3 normal = normalize(WorldNormal);

	vec4 diffuse = vec4(halfLambert(light_dir, normal));

    gl_FragColor = diffuse;
}