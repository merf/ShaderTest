varying vec3 normal;
varying vec3 eyedir;
varying vec4 pos;

void main() 
{
	const float specular_strength = 0.25;
	const float fresnel_strength = 0.2;

	vec4 color = vec4(0.1, 0.5, 0.7, 1.0);
	vec4 ambient_color = vec4(0.0, 0.1, 0.1, 1.0);

	vec4 matspec = vec4(1,1,1,1);
	float shininess = 20.0;
	float fresnel_power = 5.0;
	vec4 lightspec = vec4(1,1,1,1);

	/*
	vec4 color = gl_FrontMaterial.diffuse;
	vec4 matspec = gl_FrontMaterial.specular;
	float shininess = gl_FrontMaterial.shininess;
	vec4 lightspec = gl_LightSource[0].specular;
	*/
	vec4 lpos = gl_LightSource[0].position;
	vec4 s = -normalize(pos-lpos); 

	vec3 light = s.xyz;
	vec3 n = normalize(normal);
	vec3 r = -reflect(light, n);
	r = normalize(r);
	vec3 v = -pos.xyz;
	v = normalize(v);

	float diffuse_term = clamp(dot(normal, s.xyz), 0.0, 1.0);

	float specular_term = pow(clamp(dot(r, v), 0.0, 1.0), shininess);
	//float fresnel_term = pow(1.0-clamp(dot(normal, eyedir), 0.0, 1.0), fresnel_power);
	float fresnel_term = pow(1.0-clamp(dot(normal, eyedir), 0.0, 1.0), fresnel_power);
	
	gl_FragColor = ambient_color + diffuse_term * color + 
				specular_term * specular_strength +
				fresnel_term * fresnel_strength;

	//gl_FragColor = vec4(fresnel_term, fresnel_term, fresnel_term, 1);
	//gl_FragColor = vec4(eyedir, 1);
}