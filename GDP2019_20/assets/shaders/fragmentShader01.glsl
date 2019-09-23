#version 420

in vec3 color;
in vec4 vertWorld;

uniform float cRed;
uniform float cGreen;
uniform float cBlue;

uniform vec3 lightPosition;
uniform float linearAtten;

void main()
{
	float dist = distance(lightPosition, vertWorld.xyz);

	float atten = 1.0 / (linearAtten * dist);

    gl_FragColor = vec4(cRed, cGreen, cBlue, 1.0);
	gl_FragColor *= atten;
}