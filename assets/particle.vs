varying vec3 normal;
varying vec4 pos;
varying vec3 eyedir;

uniform float explode;

void main() {
	normal = gl_NormalMatrix * gl_Normal;
	//gl_Position = ftransform();
	pos = gl_Vertex;

	const float model_begin = 1.2;
	const float model_height = 4.0;
	float tmp = (pos.y + model_begin + (cos(pos.x*10.0+pos.z*7.3)*0.1 * cos(pos.x*143.0)*0.1))/model_height;
	float offsettime = explode - tmp;
	
	if(tmp < explode) 
	{
		vec3 disperse;
		//disperse.z = pos.z * sin(offsettime);
		if(explode < 1.0)
		{
			disperse.x = (1.3 * cos(262.0 * gl_Vertex.z))*sin(offsettime);
			disperse.z = (1.3 * cos(362.0 * gl_Vertex.z))*sin(offsettime);
			disperse.y = -2.5 * sin(offsettime*2.5) * offsettime;
		}
		else
		{
			disperse.x = (1.3 * cos(262.0 * gl_Vertex.z))*sin(1.0-tmp)*(explode);
			disperse.z = (1.3 * cos(362.0 * gl_Vertex.z))*sin(1.0-tmp)*(explode);
			disperse.y = -2.5 * sin((1.0-tmp)*2.5) * offsettime;		
		}
		//disperse.x = pos.x * offsettime * sin(offsettime);
		//disperse.y = -pow(offsettime*0.5, 2.0);
        pos += vec4(disperse,0);
		
		//pos = vec4(0,0,0,0);
    }
	else
	{
		//pos = vec4(0,0,0,0);
	}

	pos = vec4(0,0,0,0);
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
	pos = gl_ModelViewMatrix * pos;

	eyedir = -pos.xyz;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	//gl_FrontColor = gl_Color;
}
