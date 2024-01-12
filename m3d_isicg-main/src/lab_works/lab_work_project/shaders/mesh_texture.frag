#version 450

#define N 7

layout( location = 0 ) out vec4 fragColor;

layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 0 ) uniform sampler2D uAmbiantMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

uniform bool uHasDiffuseMap;
uniform bool uHasAmbiantMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;
uniform bool uIsDay;
uniform bool uColorLight;


uniform vec3  uAmbiantColor;
uniform vec3  uDiffuseColor;
uniform vec3  uSpecularColor;
uniform float uShininess;
uniform mat3  uLights[ N ];

in vec3 LightPos[ N ];
in vec3 FragNormal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 PosTangent;
in vec3 LightTangent[ N ];

void main()
{
	vec4 _diffuse;
	vec3 _ambiant, _specular;
	float _shininess;
	if ( uHasDiffuseMap )
	{
		if ( float( texture( uDiffuseMap, TexCoords ).a ) < 0.5f )
		{
			discard;
		}
		_diffuse = texture( uDiffuseMap, TexCoords ).xyzw;
	}
	else
		_diffuse = vec4(uDiffuseColor, 1.f);

	if ( uHasAmbiantMap )
		_ambiant = texture( uAmbiantMap, TexCoords ).xyz;
	else
		_ambiant = uAmbiantColor;

	if ( uHasSpecularMap )
		_specular = texture( uSpecularMap, TexCoords ).xxx;
	else
		_specular = uSpecularColor;

	if ( uHasShininessMap )
		_shininess = texture( uShininessMap, TexCoords ).x;
	else
		_shininess = uShininess;

	vec3 _lightDir, _viewDir, _light, _pos;
	vec3 _normal;

	if ( uHasNormalMap )
	{
		_normal = texture( uNormalMap, TexCoords ).xyz;
		_normal = normalize( _normal * 2.f - 1.f );
		_pos	= PosTangent;
	}
	else
	{
		_normal = FragNormal;
		_pos	= FragPos;
	}

	fragColor = vec4( _ambiant, 1.f );
	float _intensite;
	// Eclairage Diffus - Phong
	vec4 _tmp_diffuse;
	vec3 _tmp_specular;
	for ( int i = 0; i < N; ++i )
	{
		if (!uIsDay) {
			if ( i == 5 )
				continue;
		}
		else {
			if ( i == 6 )
				continue;
		}

		if ( !uColorLight )
		{
			if ( i > 0 && i < 5 )
				continue;
		}


		_tmp_diffuse = _diffuse;
		_tmp_specular = _specular;
		_light = uHasNormalMap ? LightTangent[ i ] : LightPos[i];
	

		_lightDir = normalize( _light - _pos );
		_viewDir  = normalize( -_pos );

		_intensite = max( dot( _normal, _lightDir ), 0.f );

		_tmp_diffuse.xyz *= _intensite * uLights[ i ][ 1 ];

		/* if ( dot( _normal, _lightDir ) < 0.f )
		{
			_normal *= -1;
		}*/

		// Eclairage Speculaire - Blinn Phong
		_tmp_specular
			*= pow( max( dot( _normal, normalize( _lightDir + _viewDir ) ), 0.f ), _shininess ) * uLights[ i ][ 1 ];

		fragColor += vec4( ( _tmp_specular + _tmp_diffuse.xyz )
							 * ( max( ( uLights[ i ][ 2 ].x - length( _pos - _light ) ) / uLights[ i ][ 2 ].x, 0 ) ),
						  0.f );
	}
	fragColor = vec4( min( fragColor.x, 1.f ), min( fragColor.y, 1.f ), min( fragColor.z, 1.f ), 1.f );


}
