#version 420

uniform vec4 eyeLocation;

in vec4 fColour;
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

uniform vec4 ambientColour;
uniform vec4 diffuseColour;
uniform vec4 specularColour;

// x: delta Time
// y: total Time 
// z: do not light
// w: show normals only
uniform vec4 params1;

// x: is skybox 
// y: reflect/refract skybox 
// z: 
// w: 
uniform vec4 params2;

// x: offsetX
// y: offsetY
// z: blend ratio
// w: tiling
uniform vec4 textparams00;
uniform vec4 textparams01;
uniform vec4 textparams02;
uniform vec4 textparams03;
uniform vec4 textparams04;
uniform vec4 textparams05;

// Texture samplers
uniform sampler2D textSamp00;
uniform sampler2D textSamp01;
uniform sampler2D textSamp02;
uniform sampler2D textSamp03;
uniform sampler2D textSamp04;
uniform sampler2D textSamp05;

uniform vec4 heightparams;
uniform sampler2D heightSamp;

// x: offsetX
// y: offsetY
// z: cutoff
// w: tiling
uniform vec4 discardparams;
uniform sampler2D discardSamp;

uniform samplerCube skyboxSamp00;
uniform samplerCube skyboxSamp01;


uniform sampler2D secondPassSamp;
uniform float passCount;

uniform bool daytime;

uniform vec2 waterOffset;

uniform sampler2D noiseSamp;
uniform sampler2D scopeSamp;

out vec4 pixelColour; // RGB A (0 to 1) 

// Fragment shader
struct sLight
{
	vec4 position;
	vec4 diffuse;
	vec4 specular;	// rgb = highlight colour, w = power POWER IS 1.0f to 10,000.0f
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 20;
uniform sLight lights[NUMBEROFLIGHTS];

vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular);
	 
void main()  
{
	if (passCount == 2)
	{
		// HACK TV has weird UV
		vec2 newST = vec2(fUVx2.s * 1.45f, fUVx2.t);
		pixelColour = texture(secondPassSamp, newST);
		pixelColour.rgb *= texture(scopeSamp, newST).g;
		if (texture(noiseSamp, newST + waterOffset).r < 0.5)
		{
			pixelColour.rgb *= 0.75;
		}

		return;
	}
	else if (passCount == 3)
	{

		// TODO: pass viewport size
		float s = gl_FragCoord.x / 1920;
		float t = gl_FragCoord.y / 1080;
		pixelColour = texture(secondPassSamp, vec2(s, t));
	}

	vec3 norm = normalize(fNormal.xyz);
	if (params1.w != 0.0) // normals as color
	{
		pixelColour.rgb = norm;
		pixelColour.a = 1.0;
		return;
	}

	if (params1.z == 0.0) // do not light
	{
		pixelColour = diffuseColour;
		pixelColour.a = 1.0;
		return;
	}
	
	if (params2.x != 0.0) // Skybox
	{
		if (daytime || fVertWorldLocation.y <= 27.5f)
		{
			vec3 skyboxColor = texture(skyboxSamp00, -norm).rgb;
			pixelColour.rgb = skyboxColor;
		}
		else
		{
			if (fVertWorldLocation.y <= 32.5f)
			{
				vec3 skyboxColor = texture(skyboxSamp00, -norm).rgb;
				vec3 skyboxColor2 = texture(skyboxSamp01, -norm).rgb;
				float ratio = (fVertWorldLocation.y - 27.5f) / 5.0f;
				pixelColour.rgb = mix(skyboxColor, skyboxColor2, ratio);
			}
			else
			{
				vec3 skyboxColor = texture(skyboxSamp01, -norm).rgb;
				pixelColour.rgb = skyboxColor;
			}
		}
		pixelColour.a = 1.0;
		return;
	}
	
	vec3 color = diffuseColour.rgb;

	// terrain mesh
	if (params2.y != 0.0)
	{
		vec3 eyeVector = normalize(eyeLocation.xyz - fVertWorldLocation.xyz);
		
		if (params2.y > 0.0)
		{
			vec3 reflectVector = reflect(eyeVector, fNormal.xyz);
			color.rgb = texture(skyboxSamp00, reflectVector.xyz).rgb;
		}
		else
		{
			// TODO: index of refraction
			vec3 refractVector = refract(eyeVector, fNormal.xyz, 1.4f);
			color.rgb = texture(skyboxSamp00, refractVector.xyz).rgb;
		}
	}
	
	vec2 textparams00xy = textparams00.xy;
	vec4 specular = specularColour;

	if (discardparams.w != 0) // discard mode
	{
		vec3 disc = texture(discardSamp, fUVx2.st * discardparams.w + discardparams.xy).rgb;
		if (disc.r < discardparams.z)
			discard;
	}

	vec4 lightColoured = calculateLightContrib(color, norm, fVertWorldLocation.xyz, specular);
	if (textparams00.w != 0.0) // texture
	{
		vec3 textCol = texture(textSamp00, fUVx2.st * textparams00.w + textparams00xy).rgb;
		vec3 textured = textCol.rgb * textparams00.z;

		if (textparams01.w != 0.0)
		{
			textCol = texture(textSamp01, fUVx2.st * textparams01.w + textparams01.xy).rgb;
			textured += textCol.rgb * textparams01.z;

			if (textparams02.w != 0.0)
			{
				textCol = texture(textSamp02, fUVx2.st * textparams02.w + textparams02.xy).rgb;
				textured += textCol.rgb * textparams02.z;
			}
		}
		vec3 lightTex = textured * lightColoured.rgb;
		if (length(lightColoured.rgb) < length(ambientColour.rgb))
			lightTex = textured * ambientColour.rgb;

		pixelColour.rgb = lightTex;
	}
	else // no texture
	{
		pixelColour.rgb = lightColoured.rgb + (color * ambientColour.rgb);
	}

	pixelColour.a = diffuseColour.a;
}


vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 norm, vec3 vertexWorldPos, vec4 vertexSpecular)
{
	vec4 finalObjectColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (int index = 0; index < NUMBEROFLIGHTS; index++)
	{	
		if (lights[index].param2.x == 0.0f)
			continue;
		
		// Cast to an int (note with c'tor)
		int intLightType = int(lights[index].param1.x);
		
		// We will do the directional light here... 
		if (intLightType == DIRECTIONAL_LIGHT_TYPE)		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO:
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = lights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot(-lights[index].direction.xyz, norm); // -1 to 1

			dotProduct = max(0.0f, dotProduct); // 0 to 1
		
			lightContrib *= dotProduct;
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * lights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			continue;
		}
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = lights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	

		if (distanceToLight > lights[index].atten.w)
			continue; // beyond light max range

		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, norm);	 
		
		dotProduct = max(0.0f, dotProduct);	
		
		vec3 lightDiffuseContrib = dotProduct * lights[index].diffuse.rgb;
		
		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
		
		vec3 reflectVector = reflect(-lightVector, norm);

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
		lightSpecularContrib = pow(max(0.0f, dot(eyeVector, reflectVector)), objectSpecularPower) * lights[index].specular.rgb;
		
		// Attenuation
		float attenuation = 1.0f / 
				(lights[index].atten.x + 										
				 lights[index].atten.y * distanceToLight +						
				 lights[index].atten.z * distanceToLight * distanceToLight);  	
		
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;

		// But is it a spot light
		if (intLightType == SPOT_LIGHT_TYPE)
		{
			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - lights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle = dot(vertexToLight.xyz, lights[index].direction.xyz);
				
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(lights[index].param1.z));
			float innerConeAngleCos = cos(radians(lights[index].param1.y));
							
			// Is it completely outside of the spot?
			if (currentLightRayAngle < outerConeAngleCos)
			{
				// So it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if (currentLightRayAngle < innerConeAngleCos)
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb) + (vertexSpecular.rgb * lightSpecularContrib.rgb);
	} // For loop lights

	return finalObjectColour;
}
