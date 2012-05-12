varying vec3 normal;
varying vec3 eyedir;
varying vec4 pos;
varying vec4 pix_vel;

uniform float time;
uniform float explode;

uniform mat4 previous_model_view;
uniform mat4 model_view;

void main() 
{
	normal = gl_NormalMatrix * gl_Normal;
	//gl_Position = ftransform();
	pos = gl_Vertex;

	const float model_begin = 1.2;
	const float model_height = 4.0;
	float tmp = (pos.y + model_begin + (cos(pos.x*10.0+pos.z*7.3)*0.1 * cos(pos.x*143.0)*0.1))/model_height;
	float offsettime = explode - tmp;
	
	if(tmp < explode) 
	{
		pos = vec4(0,0,0,0);
	}
	
	
	gl_Position =  gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	pos = gl_ModelViewMatrix * gl_Vertex;

	vec4 prev_pos = gl_ProjectionMatrix * previous_model_view * pos;
	
	float vel_scale = 1.0;

	vec4 p1 = (gl_ProjectionMatrix * model_view * pos);
	vec4 p2 = (gl_ProjectionMatrix * previous_model_view * pos);

	pix_vel = p1-p2;

	pix_vel += vec4(0.5, 0.5, 0.0, 0.0);

	//normal = vec3(length(pos.xyz));

	eyedir = -pos.xyz;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	gl_FrontColor = gl_Color;
}
