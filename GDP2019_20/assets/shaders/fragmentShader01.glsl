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
// w:
uniform vec4 params1;

// x: is skybox 
// y: 
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

uniform sampler2D heightSamp;

uniform samplerCube skyboxSamp00;

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
	if (params1.z == 0.0) // do not light
	{
		pixelColour = diffuseColour;
		pixelColour.a = 1.0;
		return;
	}
	
	if (params2.x != 0.0)
	{
		vec3 skyboxColor = texture(skyboxSamp00, -fNormal.xyz).rgb;
		//pixelColour.rgb = fNormal.xyz;
		pixelColour.rgb = skyboxColor;
		pixelColour.a = 1.0;
		return;
	}

	vec4 lightColoured = calculateLightContrib(diffuseColour.rgb, fNormal.xyz, fVertWorldLocation.xyz, specularColour);
	if (textparams00.w != 0.0) // has texture
	{
		vec3 textCol = texture(textSamp00, fUVx2.st * textparams00.w + textparams00.xy).rgb;
		vec3 textured = (textCol + textCol.rgb) * textparams00.z;

		if (textparams01.w != 0.0)
		{
			textCol = texture(textSamp01, fUVx2.st * textparams01.w + textparams01.xy).rgb;
			textured += (textCol + textCol.rgb) * textparams01.z;

			if (textparams02.w != 0.0)
			{
				textCol = texture(textSamp02, fUVx2.st * textparams02.w + textparams02.xy).rgb;
				textured += (textCol + textCol.rgb) * textparams02.z;
			}
		}
		vec3 lightTex = textured * lightColoured.rgb;
		if (length(lightColoured.rgb) < length(ambientColour.rgb))
			lightTex = textured * ambientColour.rgb;

		pixelColour.rgb = lightTex;
	}
	else // no texture
	{
		pixelColour.rgb = lightColoured.rgb + (diffuseColour.rgb * ambientColour.rgb);
	}

	pixelColour.a = diffuseColour.a;
}


vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular)
{
	vec3 norm = normalize(vertexNormal);
	
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
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
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
		
		// lightSpecularContrib = pow(max(0.0f, dot(eyeVector, reflectVector)), objectSpecularPower) * vertexSpecular.rgb;	//* lights[lightIndex].Specular.rgb
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