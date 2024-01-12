#version 450 core
layout( location = 0 ) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 uPM;
uniform mat4 uVM;

void main()
{
	TexCoords	= aPos;
	gl_Position = uPM * uVM * vec4( aPos, 1.0 );
}