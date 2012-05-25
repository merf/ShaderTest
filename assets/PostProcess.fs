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

	gl_FragColor = texture2D(tex0, gl_TexCoord[0].xy);

	return;
}
