#version 450

layout( location = 0 ) out vec4 fragColor;

uniform vec3  uAmbiantColor;
uniform vec3  uDiffuseColor;
uniform vec3  uSpecularColor;
uniform float uShininess;
uniform vec3  uLightPos;
uniform vec4  uStaticLightPos;

in vec3 FragNormal;
in vec3 FragPos;

void main()
{
	// Eclairage Diffus - Phong
	vec3 _lightDir = normalize( uLightPos - FragPos );

	vec3 _FragNormal = FragNormal;
	if ( dot( _FragNormal, _lightDir ) < 0.f )
	{ // normal et lumiere incidente ont des directions opposees
		_FragNormal *= -1;
	}

	float _intensite = max( dot( _FragNormal, _lightDir ), 0.f );
	vec3  _diffuse	 = uDiffuseColor * _intensite;

		// Eclairage Speculaire - Blinn Phong
	vec3 _specular
		= uSpecularColor * pow( max( dot( _FragNormal, normalize( _lightDir - FragPos ) ), 0.f ), uShininess );

	fragColor = vec4( _specular + _diffuse + uAmbiantColor, 1.f );
}
