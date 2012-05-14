////////////////////////////
// SUB-SURFACE SCATTER FS //
////////////////////////////

/* --------------------------
SubScatter Fragment Shader:

Fake sub-surface scatter lighting shader by InvalidPointer 2008.
Found at
http://www.gamedev.net/community/forums/topic.asp?topic_id=481494

HLSL > GLSL translation
toneburst 2008
-------------------------- */

// Variables for lighting properties
uniform float MaterialThickness;
uniform vec3 ExtinctionCoefficient; // Will show as X Y and Z ports in QC, but actually represent RGB values.
uniform vec4 LightColor;
//uniform vec4 BaseColor;
//uniform vec4 SpecColor;
//uniform float SpecPower;

uniform float RimScalar;
//uniform sampler2D Texture;

// Varying variables to be sent to Fragment Shader
varying vec3 worldNormal, eyeVec, lightVec, vertPos, lightPos;

float halfLambert(in vec3 vect1, in vec3 vect2)
{
	float product = dot(vect1,vect2);
	return product * 0.5 + 0.5;
}

float blinnPhongSpecular(in vec3 normalVec, in vec3 lightVec, in float specPower)
{
	vec3 halfAngle = normalize(normalVec + lightVec);
	return pow(clamp(0.0,1.0,dot(normalVec,halfAngle)), specPower);
}

// Main fake sub-surface scatter lighting function

vec4 subScatterFS()
{
	float attenuation = 10.0 * (1.0 / distance(lightPos, vertPos));
	vec3 eVec = normalize(eyeVec);
	vec3 lVec = normalize(lightVec);
	vec3 wNorm = normalize(worldNormal);
	
	vec4 dotLN = vec4(halfLambert(lVec,wNorm) * attenuation);
	//dotLN *= texture2D(Texture, gl_TexCoord[0].xy);
	dotLN *= gl_FrontMaterial.diffuse;

	//return dotLN;

	vec3 indirectLightComponent = vec3(MaterialThickness * max(0.0,dot(-wNorm,lVec)));
	indirectLightComponent += MaterialThickness * halfLambert(-eVec,lVec);
	indirectLightComponent *= attenuation;
	indirectLightComponent.r *= ExtinctionCoefficient.r;
	indirectLightComponent.g *= ExtinctionCoefficient.g;
	indirectLightComponent.b *= ExtinctionCoefficient.b;
	
	//return vec4(indirectLightComponent, 0.0) ;
	
	vec3 rim = vec3(1.0 - max(0.0,dot(wNorm,eVec)));
	rim *= rim;
	rim *= max(0.0,dot(wNorm,lVec)) * gl_FrontMaterial.specular.rgb;
	
	vec4 finalCol = dotLN + vec4(indirectLightComponent,1.0);
	//return finalCol;
	
	finalCol.rgb += (rim * RimScalar * attenuation * finalCol.a);
	finalCol.rgb += vec3(blinnPhongSpecular(wNorm,lVec,gl_FrontMaterial.shininess) * attenuation * gl_FrontMaterial.specular.rgb * finalCol.a * 0.05);
	finalCol.rgb *= LightColor.rgb;
	
	return finalCol;
}

////////////////
//  MAIN LOOP //
////////////////

void main()
{
	gl_FragColor = subScatterFS();
	
	//gl_FragColor = vec4(eyeVec, 1);
}