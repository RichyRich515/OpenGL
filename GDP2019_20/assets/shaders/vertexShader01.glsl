#version 420

uniform mat4 matModel;		// Model or World
uniform mat4 matView;		// View or Camera
uniform mat4 matProjection; // Projection

in vec3 vCol;
in vec3 vPos;

out vec3 color;
out vec4 vertWorld;			// Location of the vertex in the world

void main()
{
	vec3 vertPosition = vPos;

	mat4 matMVP = matProjection * matView * matModel;
	
    gl_Position = matMVP * vec4(vertPosition, 1.0);

	// Calculate vertex in world space
	vertWorld = matModel * vec4(vertPosition, 1.0);

	color = vCol;
}