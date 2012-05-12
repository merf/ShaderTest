uniform sampler2D tex0;
uniform sampler2D velocity_tex;
uniform float time;

const int blur_samples = 10;

void main()
{
	vec2 tc = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);

	vec4 c = vec4(0.0);

	float blur_size = 0.5 / float(blur_samples);

	vec2 velocity = (texture2D(velocity_tex, tc).xy - vec2(0.5)) * -blur_size;

	float t = 0.0;
	for(int i=0; i<blur_samples; ++i)
	{
		float f = pow(1.0 - (float(i)/float(blur_samples)), 2.0);
		c += texture2D(tex0, tc) * f;
		tc+=velocity;	
		t += f;
	}

	c /= t;

	//c = vec4(0, velocity, 1);

	gl_FragColor = gl_Color * vec4(c);
	//gl_FragColor = vec4(1,1,1,1);
}
