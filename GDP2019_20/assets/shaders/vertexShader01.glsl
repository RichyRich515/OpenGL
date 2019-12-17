#version 420

uniform mat4 matModel;					// Model or World
uniform mat4 matModelInverseTranspose;	// inverse transpose
uniform mat4 matView;					// View or Camera
uniform mat4 matProjection;				// Projection

uniform float offset;

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;
in vec4 vUVx2;

out vec4 fColour;
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;

void main()
{
	vec4 vertPosition = vPosition;
	vertPosition += vNormal * offset;

	mat4 matMVP = matProjection * matView * matModel;
	
    gl_Position = matMVP * vec4(vertPosition.xyz, 1.0f);

	// Calculate vertex in world space
	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0f);
	fNormal.xyz = mat3(matModelInverseTranspose) * normalize(vNormal.xyz).xyz; // TODO: dont convert to mat3?
	fNormal.w = 1.0;

	fColour = vColour;
	fUVx2 = vUVx2;
}