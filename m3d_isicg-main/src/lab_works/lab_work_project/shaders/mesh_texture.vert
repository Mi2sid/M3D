#version 450

#define SIZE 7

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
// layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uMVPMatrix;
uniform mat3 uLights[ SIZE ];

out vec3 FragNormal;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 PosTangent;
out vec3 LightTangent[ SIZE ];
out vec3 LightPos[ SIZE ];

void main()
{
	TexCoords = aVertexTexCoords;
    FragNormal = normalize((uNormalMatrix * vec4(aVertexNormal, 1.f)).xyz);

    // Tangent space
	vec3 T = normalize( uNormalMatrix * vec4( aVertexTangent, 1.f )).xyz ;
	vec3 N = FragNormal; 
	T = normalize( T - dot( T, N ) * N ); // On veut T orthogonal a N
	vec3 B = cross( N, T );

	mat3 inv_TBN = transpose( mat3( T, B, N ) );
        
	FragPos = ( uMVMatrix * vec4( aVertexPosition, 1.f ) ).xyz;
	PosTangent = inv_TBN * FragPos;

	for ( int i = 0; i < SIZE; ++i )
	{
		LightPos[ i ] = ( uMVMatrix * vec4( uLights[ i ][ 0 ], 1.f ) ).xyz;
		LightTangent[i] = inv_TBN * LightPos[i];
	}

    // Clip
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.f);

}
