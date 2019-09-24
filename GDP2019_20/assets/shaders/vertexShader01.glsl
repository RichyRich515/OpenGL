#version 420

uniform mat4 matModel;		// Model or World
uniform mat4 matView;		// View or Camera
uniform mat4 matProjection; // Projection

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

	mat4 matMVP = matProjection * matView * matModel;
	
    gl_Position = matMVP * vec4(vertPosition.xyz, 1.0f);

	// Calculate vertex in world space
	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0f);

	fColour = vColour;
	fNormal = vNormal;
	fUVx2 = vUVx2;
}