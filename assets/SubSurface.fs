//////////////////////////////////////////////////////////////////////////
// SUB-SURFACE SCATTER FS												//
// http://www.gamedev.net/community/forums/topic.asp?topic_id=481494	//
//////////////////////////////////////////////////////////////////////////

uniform vec4 LightPosition;
uniform float MaterialThickness;
uniform vec3 ExtinctionCoefficient;

uniform float RimScalar;
uniform float RimPower;

uniform float SpecularIntensity;
uniform float SpecularPower;

varying vec3 Normal;
varying vec4 VertPos;
varying vec4 RawPos;

varying vec4 LightPos0;
varying vec4 LightPos1;
varying vec4 EyeVec;

varying vec4 PixelVelocity;

float HalfLambert(in vec3 vect1, in vec3 vect2)
{
	//float product = clamp(dot(vect1, vect2), 0.0, 1.0);
	float product = dot(vect1, vect2);
	return product * 0.5 + 0.5;
}

float BlinnPhongSpecular(in vec3 normalVec, in vec3 lightVec, in float specPower)
{
	vec3 halfAngle = normalize(normalVec + lightVec);
	return pow(clamp(0.0,1.0,dot(normalVec, halfAngle)), specPower);
}

float Specular(in vec3 normalVec, in vec3 lightVec, in vec3 eyeVec, in float specPower)
{
	vec3 r = -reflect(lightVec, normalVec);
	return pow(clamp(dot(r, eyeVec), 0.0, 1.0), specPower);
}


vec4 SubSurface(vec3 light_pos)
{
	float attenuation = (2.0 / distance(light_pos, VertPos.xyz));

	vec3 light_dir = normalize(light_pos - VertPos.xyz);
	vec3 eye_dir = normalize(EyeVec.xyz);

	float diffuse = HalfLambert(light_dir, Normal) * attenuation;

	vec3 indirect_component = vec3(MaterialThickness * max(0.0,dot(-Normal, light_dir)));
	indirect_component += MaterialThickness * HalfLambert(-eye_dir, light_dir);
	indirect_component *= attenuation;
	indirect_component.r *= ExtinctionCoefficient.r;
	indirect_component.g *= ExtinctionCoefficient.g;
	indirect_component.b *= ExtinctionCoefficient.b;

	vec3 rim = vec3(1.0 - max(0.0, dot(Normal, eye_dir)));
	//rim = pow(rim, RimPower);
	rim *= rim;
	rim *= max(0.0, dot(Normal, light_dir)) * gl_FrontMaterial.specular;

	//float specular = BlinnPhongSpecular(Normal, light_dir, SpecularPower);
	float specular = Specular(Normal, light_dir, eye_dir, SpecularPower);
	
	
	vec4 final_colour = diffuse * gl_FrontMaterial.diffuse;
	final_colour += vec4(indirect_component, 1.0);

	final_colour.rgb += (rim * RimScalar * attenuation * final_colour.a);
	final_colour.rgb += specular * attenuation * gl_FrontMaterial.specular * final_colour.a * SpecularIntensity;

//	final_colour = vec4(specular);

	return final_colour;
}

void main()
{
	gl_FragData[0] = SubSurface(LightPos0) * gl_LightSource[0].diffuse;
	gl_FragData[0] += SubSurface(LightPos1) * gl_LightSource[1].diffuse;

	float dist = length(EyeVec.xyz);

	gl_FragData[1] = PixelVelocity;
	//gl_FragData[1] = vec4(0.5 + (2 / dist) * 0.5, 0.5, 0.0, 1.0);
	//gl_FragData[1] = vec4(0.5, 0.5, 0.0, 1.0);
	//gl_FragData[1] = vec4(1.0, 0.5, 0.0, 1.0);
}