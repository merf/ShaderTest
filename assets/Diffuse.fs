varying vec3 normal;
varying vec4 pos;

float Lambert(vec3 v1, vec3 v2, float f)
{
	return max(dot(v1, v2), 0.0) * f + (1.0 -f);
}

void main() 
{
/*
	vec4 light_pos = gl_LightSource[0].position;
	vec4 light_dir = -normalize(pos - light_pos); 

	float d = Lambert(normal, light_dir.xyz, 1.0);

	gl_FragColor = vec4(d);

	vec4 diffuse  = max(0.0, dot(normal, light_dir.xyz));
	gl_FragColor = diffuse;

	return;
	*/
	vec4 color = gl_FrontMaterial.diffuse;
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
    
	vec4 diffuse  = color * max(0.0, dot(n, s.xyz)) * gl_LightSource[0].diffuse;
	vec4 specular;
	if (shininess != 0.0) 
	{
		specular = lightspec * matspec * pow(max(0.0, dot(r, v)), shininess);
	} 
	else 
	{
		specular = vec4(0.0, 0.0, 0.0, 0.0);
	}

	gl_FragColor = diffuse;// + specular;
	//gl_FragColor = vec4(lpos.xyz, 1.0);
}