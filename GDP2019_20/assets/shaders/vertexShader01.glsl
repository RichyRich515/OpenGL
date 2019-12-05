#version 420

uniform mat4 matModel;					// Model or World
uniform mat4 matModelInverseTranspose;	// inverse transpose
uniform mat4 matView;					// View or Camera
uniform mat4 matProjection;				// Projection

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;
in vec4 vUVx2;

out vec4 fColour;
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;

// x: offsetX
// y: offsetY
// z:
// w: tiling
uniform vec4 heightparams;
uniform sampler2D heightSamp;

void main()
{
	vec4 vertPosition = vPosition;

	mat4 matMVP = matProjection * matView * matModel;
	

	// Height map
	if (heightparams.w != 0.0)
	{
		vec4 samp = texture(heightSamp, vUVx2.st * heightparams.w + heightparams.xy);
		vertPosition.y += (samp.r + samp.g + samp.b - 0.5) / 5.0;
	}

    gl_Position = matMVP * vec4(vertPosition.xyz, 1.0f);

	// Calculate vertex in world space
	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0f);
	
	fNormal.xyz = mat3(matModelInverseTranspose) * normalize(vNormal.xyz).xyz; // TODO: dont convert to mat3?
	fNormal.w = 1.0;

	fColour = vColour;
	fUVx2 = vUVx2;
}