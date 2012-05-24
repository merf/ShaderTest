uniform sampler2D tex0;
uniform sampler2D velocity_tex;
uniform float time;
uniform float MotionBlurAmount;

const int blur_samples = 30;

void main()
{
	vec2 tc = gl_TexCoord[0].xy;

	float blur_size = 0.5 / float(blur_samples);
	
	vec2 velocity = (texture2D(velocity_tex, tc).xy);// * -blur_size;
	velocity -= vec2(0.5);
	velocity *= MotionBlurAmount;
//	velocity *= 0.1;

	vec4 color = texture2D(tex0, tc, 0.0);

	float f_total = 1.0;

	for(int i=0; i<blur_samples; ++i)
	{
		float f = pow(1.0 - (float(i)/float(blur_samples)), 4.0);
		//f = 1;
		tc += velocity;
		color += texture2D(tex0, tc, 0.0) * f;

		f_total += f;
	}

	color /= f_total;

	gl_FragColor = color;

	return;


	/*






	c = texture2D(tex0, tc + velocity, 0.0).rgb;
	
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
	gl_FragColor = vec4(c, 1.0);
	//gl_FragColor = vec4(1,1,1,1);

	*/
}
