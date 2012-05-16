varying vec3 Normal;
varying vec4 VertexPos;
//varying vec4 RawVertexPos;
varying vec4 LightVec;


float Lambert(vec3 v1, vec3 v2, float f)
{
	return max(dot(v1, v2), 0.0) * f + (1.0 -f);
}

void main() 
{
	vec4 light_dir = normalize(LightVec);

	float d = Lambert(Normal, light_dir.xyz, 1.0);

	vec4 color = d * gl_FrontMaterial.diffuse;
	gl_FragColor = vec4(color);
}