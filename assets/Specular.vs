varying vec3 normal;
varying vec3 eyedir;
varying vec4 pos;
varying vec4 pix_vel;

uniform float time;
uniform float explode;

uniform mat4 previous_model_view;
uniform mat4 model_view;

uniform bool particle;

void main() 
{
	normal = gl_NormalMatrix * gl_Normal;
	pos = gl_Vertex;

	const float model_begin = 1.2;
	const float model_height = 4.0;
	float tmp = (pos.y + model_begin + (cos(pos.x*10.0+pos.z*7.3)*0.1 * cos(pos.x*143.0)*0.1))/model_height;
	float offsettime = explode - tmp;
	
	if(tmp < explode) 
	{
		if(particle)
		{
			vec3 disperse;

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

			pos += vec4(disperse,0);
		}
		else
		{
			pos = vec4(0,0,0,0);
		}
	}
	else
	{
		if(particle)
		{
			pos = vec4(0,0,0,0);
		}
	}
	
	
	gl_Position =  gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	pos = gl_Vertex;

	float vel_scale = 1.0;

	vec4 p1 = (gl_ProjectionMatrix * model_view * pos);
	vec4 p2 = (gl_ProjectionMatrix * previous_model_view * pos);

	pix_vel = ((p1-p2) + vec4(0.5, 0.5, 0.0, 1.0));// * 10.0;
	
	pos = gl_ModelViewMatrix * gl_Vertex;

	eyedir = -pos.xyz;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	gl_FrontColor = gl_Color;
}
