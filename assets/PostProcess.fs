uniform sampler2D tex0;
uniform sampler2D velocity_tex;
uniform float time;

const int blur_samples = 1;

void main()
{
	vec2 tc = gl_TexCoord[0].xy;
	gl_FragColor = texture2D(tex0, tc);
	
	return;
	
	vec3 c = vec3(0.0);
	
	float blur_size = 0.5 / float(blur_samples);
	
	vec2 velocity = (texture2D(velocity_tex, tc).xy);// * -blur_size;
	//velocity = vec2(0.3, 0.5);
	
	velocity -= vec2(0.5);
	velocity *= 0.01;
	//velocity += vec2(0.5);
	
	//velocity.y = 0.0;
	//velocity *= 0.01;
	
	c = texture2D(tex0, gl_TexCoord[0].xy + velocity, 0.0).rgb;
	
	if(true)
	{
		//c = vec4(velocity, 0.0, 1.0).rgb;
	}
	else
	{
		float t = 0.0;
		for(int i=0; i<blur_samples; ++i)
		{
			float f = pow(1.0 - (float(i)/float(blur_samples)), 2.0);
			c += texture2D(tex0, tc).rgb * f;
			tc+=velocity;	
			t += f;
		}
		c /= t;
	}
	//c.a = 1.0;
	
	//c = vec4(0, velocity, 1);
	
	//c = texture2D(tex0, gl_TexCoord[0].xy);
	gl_FragColor = gl_Color * vec4(c, 1.0);
	//gl_FragColor = vec4(1,1,1,1);
}
