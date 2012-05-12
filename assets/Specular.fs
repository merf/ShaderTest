varying vec3 normal;
varying vec3 eyedir;
varying vec4 pos;
varying vec4 pix_vel;

//uniform float shininess;
uniform float fresnel_power;
//uniform float specular_intensity;
uniform float fresnel_intensity;

void main() 
{

	vec4 diffuse_color = gl_FrontMaterial.diffuse;
	vec4 ambient_color = gl_FrontMaterial.ambient;

	vec4 matspec = gl_FrontMaterial.specular;
	float shininess = gl_FrontMaterial.shininess;
	vec4 lightspec = gl_LightSource[0].specular;

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
	float fresnel_term = pow(1.0-clamp(dot(normal, eyedir), 0.0, 1.0), fresnel_power);
	

	gl_FragData[0] = ambient_color + diffuse_term * diffuse_color + 
				specular_term * matspec * matspec.a * lightspec +
				fresnel_term * fresnel_intensity * lightspec;
	//gl_FragColor = vec4(fresnel_term, fresnel_term, fresnel_term, 1);
	//gl_FragColor = vec4(eyedir, 1);
	//gl_FragColor = vec4(color);

	//gl_FragData[1] = vec4(pix_vel.r, 0.0, 0.0, 1.0);
	gl_FragData[1] = pix_vel;

	gl_FragData[1] = vec4(pix_vel.xy, 0.0, 1.0);
//	gl_FragData[1].z = 0.0;
}