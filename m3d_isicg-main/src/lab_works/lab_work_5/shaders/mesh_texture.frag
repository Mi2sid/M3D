#version 450

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

uniform vec3  uAmbiantColor;
uniform vec3  uDiffuseColor;
uniform vec3  uSpecularColor;
uniform float uShininess;


in vec3 LightPos;
in vec3 FragNormal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 PosTangent;
in vec3 LightTangent;

void main()
{
	vec4 _diffuse;
	vec3 _ambiant, _specular;

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

	// Eclairage Diffus - Phong
	vec3 _lightDir, _viewDir, _light, _pos;

	vec3 _normal;
	if ( uHasNormalMap )
	{
		_normal = texture( uNormalMap, TexCoords ).xyz ;
		_normal	= normalize( _normal * 2.f - 1.f ) ;
		_light	= LightTangent;
		_pos	= PosTangent;
	}
	else
	{
		_normal = FragNormal;
		_light	= LightPos;
		_pos	= FragPos;
	}
	_lightDir = normalize( _light - _pos );
	_viewDir  = normalize( - _pos );


	float _intensite = max( dot( _normal, _lightDir ), 0.f )  ; 
	float _shininess;

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


	_diffuse.xyz *= _intensite;

	if ( dot( _normal, _lightDir ) < 0.f )
	{
		_normal *= -1;
	}

	// Eclairage Speculaire - Blinn Phong
	_specular
		*= pow( max( dot( _normal, normalize( _lightDir + _viewDir ) ), 0.f ), _shininess );

	fragColor = vec4( _specular + _diffuse.xyz + _ambiant, 1.f );
}
