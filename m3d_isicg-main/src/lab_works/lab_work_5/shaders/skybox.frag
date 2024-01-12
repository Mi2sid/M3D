#version 450 core
out vec4 FragColor;

in vec3 TexCoords;

layout( binding = 0 ) uniform samplerCube uSkybox;

void main() 
{ 
	FragColor = texture( uSkybox, TexCoords ); }