#version 420

in vec4 fColour;
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

// Normal
//out vec4 outColour;

// Deferred
layout(location = 0) out vec4 colourBuffer;
layout(location = 1) out vec4 worldNormalBuffer;			// if W is 0 run light, if W is 1, do not run light
layout(location = 2) out vec4 worldVertexPositionBuffer;	// ignore W for now
layout(location = 3) out vec4 specularBuffer;				// rgb Colour and w power

const float VERTEX_IS_NOT_LIT = 0.0f;
const float VERTEX_IS_LIT = 1.0f;

uniform float passCount;

// x: blur
// y: Night vision
// z:
// w:
uniform vec4 secondPassParams00;

uniform sampler2D secondPassColourSamp;
uniform sampler2D secondPassWorldNormalSamp;			// if W is 0 do not light, if 1 then run lighting
uniform sampler2D secondPassWorldVertexPositionSamp;	// ignore W for now
uniform sampler2D secondPassSpecularSamp;				// rgb Colour and w power

uniform sampler2D fullScreenOverlaySamp;

uniform vec4 eyeLocation;

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


// Texture samplers
uniform sampler2D textSamp00;
uniform sampler2D textSamp01;
uniform sampler2D textSamp02;
uniform sampler2D textSamp03;

uniform vec4 heightparams;
uniform sampler2D heightSamp;

uniform vec4 discardparams;
uniform sampler2D discardSamp;

uniform vec4 macrovariantParams;
uniform sampler2D macrovariantSamp;


uniform samplerCube skyboxSamp00;


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

const int NUMBEROFLIGHTS = 6;
uniform sLight lights[NUMBEROFLIGHTS];

vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular);

// two pass kernal, do horizontal then do vertical
// This example is a kernel calculated at http://dev.theomader.com/gaussian-kernel-calculator/
// Note: the weight is halved because it is run twice
// sigma = 1.0
// size = 7
const int BLUR_KERNEL_SIZE = 7;

// x: offset
// y: offset
// z:
// w: weight
const vec4 BLUR_KERNEL[BLUR_KERNEL_SIZE] =
{
	vec4(-3.0, -3.0, 0.0, 0.00598  * 0.5),
	vec4(-2.0, -2.0, 0.0, 0.060626 * 0.5),
	vec4(-1.0, -2.0, 0.0, 0.241843 * 0.5),
	vec4( 0.0,  0.0, 0.0, 0.383103 * 0.5),
	vec4( 1.0,  2.0, 0.0, 0.241843 * 0.5),
	vec4( 2.0,  2.0, 0.0, 0.060626 * 0.5),
	vec4( 3.0,  3.0, 0.0, 0.00598  * 0.5),
};

void main()  
{
	if (passCount == 3)
	{
		// shader map


		return
	}
	if (passCount == 2)
	{
		// deferred render
		vec3 ambience = ambientColour.rgb;

		// Blur?
		if (secondPassParams00.x != 0.0)
		{
			// loops get unwrapped by compiler
			for (int horizontal = 1; horizontal >= 0; --horizontal)
			{
				for (int i = 0; i < BLUR_KERNEL_SIZE; ++i)
				{
					float s = (gl_FragCoord.x + (horizontal != 0 ? BLUR_KERNEL[i].x : 0)) / 1920.0;
					float t = (gl_FragCoord.y + (horizontal != 0 ? 0 : BLUR_KERNEL[i].y)) / 1080.0;

					vec2 st = vec2(s, t);

					vec4 vertexColour = texture(secondPassColourSamp, st);
					vec4 vertexWorldNormal = texture(secondPassWorldNormalSamp, st);
					vec4 vertexWorldPosition = texture(secondPassWorldVertexPositionSamp, st);
					vec4 vertexSpecular = texture(secondPassSpecularSamp, st);

					if (vertexWorldNormal.w != VERTEX_IS_NOT_LIT)
					{
						vec4 lightContribution = calculateLightContrib(vec3(1.0, 1.0, 1.0), vertexWorldNormal.xyz, vertexWorldPosition.xyz, vertexSpecular);
						if (length(lightContribution.xyz) < length(ambience))
						{
							colourBuffer.rgb += vertexColour.rgb * ambience * BLUR_KERNEL[i].w;
						}
						else
						{
							colourBuffer.rgb += vertexColour.rgb * lightContribution.rgb * BLUR_KERNEL[i].w;
						}
					}
					else
					{
						colourBuffer.rgb += vertexColour.rgb * BLUR_KERNEL[i].w;
					}
				}
			}
		}
		else
		{
			// TODO: pass viewport size
			float s = gl_FragCoord.x / 1920.0f;
			float t = gl_FragCoord.y / 1080.0f;
			vec2 st = vec2(s, t);
			vec4 vertexColour = texture(secondPassColourSamp, st);
			vec4 vertexWorldNormal = texture(secondPassWorldNormalSamp, st);
			vec4 vertexWorldPosition = texture(secondPassWorldVertexPositionSamp, st);
			vec4 vertexSpecular = texture(secondPassSpecularSamp, st);

			if (secondPassParams00.z != 0)
			{
				// Edge detection derived from 
				// https://computergraphics.stackexchange.com/questions/2450/opengl-detection-of-edges?noredirect=1&lq=1
				// https://cdn.imgtec.com/sdk-documentation/Edge+Detection.Whitepaper.pdf
				// convert to hsv https://stackoverflow.com/questions/867653/how-to-implement-grayscale-rendering-in-opengl
				vec3 grayscaleVertColour = mix(vec3(dot(vertexColour.rgb, vec3(0.2125, 0.7154, 0.0721))), vertexColour.rgb, 0.5);
				float lum = dot(grayscaleVertColour, vec3(0.2125, 0.7154, 0.0721));
				float lumGrad = fwidth(lum);

				vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPosition.xyz);
				float normDot = dot(vertexWorldNormal.xyz, eyeVector);
				float normGrad = fwidth(normDot);

				bool isEdge = normGrad > secondPassParams00.z || lumGrad > secondPassParams00.z;
				if (isEdge)
				{
					colourBuffer = vec4(1.0, 1.0f, 1.0f, 1.0);
					return;
				}
			}

			if (vertexWorldNormal.w != VERTEX_IS_NOT_LIT)
			{
				vec4 lightContribution = calculateLightContrib(vec3(1.0, 1.0, 1.0), vertexWorldNormal.xyz, vertexWorldPosition.xyz, vertexSpecular);
				if (length(lightContribution.xyz) < length(ambience))
				{
					colourBuffer.rgb = vertexColour.rgb * ambience;
				}
				else
				{
					colourBuffer.rgb = vertexColour.rgb * lightContribution.rgb;
				}
			}
			else
			{
				colourBuffer.rgb = vertexColour.rgb;
			}
		}

		// Night vision
		if (secondPassParams00.y != 0)
		{
			colourBuffer.rgb = vec3(0.1, 1.0f, 0.1f) * (length(colourBuffer.rgb) * (colourBuffer.g * 0.5 + 0.5));
		}

		// don't blur overlay
		float s = gl_FragCoord.x / 1920.0f;
		float t = gl_FragCoord.y / 1080.0f;
		vec2 st = vec2(s, t);
		colourBuffer.rgb *= texture(fullScreenOverlaySamp, st).g;
		colourBuffer.a = 1.0f;
		return;
	}

	worldNormalBuffer.xyz = fNormal.xyz;
	worldNormalBuffer.w = params1.z;
	worldVertexPositionBuffer.xyz = fVertWorldLocation.xyz; 
	worldVertexPositionBuffer.w = 0.0; // w nothing for now
	specularBuffer = specularColour;

	if (params2.x != 0.0) // Skybox
	{
		colourBuffer.rgb = texture(skyboxSamp00, -fNormal.xyz).rgb;
		colourBuffer.a = 1.0;
		return;
	}

	if (params1.w != 0.0) // normals as color
	{
		colourBuffer.rgb = fNormal.xyz;
		colourBuffer.a = 1.0;
		return;
	}

	if (params1.z == 0.0) // do not light
	{
		colourBuffer = diffuseColour;
		colourBuffer.a = 1.0;
		return;
	}

	vec3 color = diffuseColour.rgb;

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

	if (discardparams.w != 0) // discard mode
	{
		vec3 disc = texture(discardSamp, fUVx2.st * discardparams.w + discardparams.xy).rgb;
		if (disc.r < discardparams.z)
			discard;
	}

	if (textparams00.w != 0.0) // texture
	{
		vec3 textCol = texture(textSamp00, fUVx2.st * textparams00.w + textparams00.xy).rgb;
		vec3 textured = textCol.rgb * textparams00.z;

		if (textparams01.w != 0.0)
		{
			textCol = texture(textSamp01, fUVx2.st * textparams01.w + textparams01.xy).rgb;
			textured += textCol.rgb * textparams01.z;

			if (textparams02.w != 0.0)
			{
				textCol = texture(textSamp02, fUVx2.st * textparams02.w + textparams02.xy).rgb;
				textured += textCol.rgb * textparams02.z;
				if (textparams03.w != 0.0)
				{
					textCol = texture(textSamp03, fUVx2.st * textparams03.w + textparams03.xy).rgb;
					textured += textCol.rgb * textparams03.z;
				}
			}
		}

		colourBuffer.rgb = textured.rgb;
		colourBuffer.a = diffuseColour.a;

		if (macrovariantParams.w != 0)
		{
			// just sample red value, arbitrary choice
			float v = texture(macrovariantSamp, fUVx2.st * macrovariantParams.w + macrovariantParams.xy).r; 

			// 1.0 +- z
			v *= macrovariantParams.z * 2;
			v += 1.0;

			// Modulate into final colour
			colourBuffer.rgb *= v;
		}
	}
	else
	{
		colourBuffer = diffuseColour;
	}
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

		// To simplify, we are NOT using the light specular value, just the object�s.
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
