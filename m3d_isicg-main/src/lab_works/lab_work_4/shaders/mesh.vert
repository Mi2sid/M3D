#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uMVPMatrix;

out vec3 FragNormal;
out vec3 FragPos;

void main()
{
    //View Space
    FragPos = (uMVMatrix * vec4(aVertexPosition, 1.f)).xyz;
    FragNormal = normalize((uNormalMatrix * vec4(aVertexNormal, 1.f)).xyz);

	//Clip Space
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.f);

}
