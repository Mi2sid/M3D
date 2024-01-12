#version 450

layout( location = 0 ) out vec4 fragColor;

in vec4 Color;

uniform float uLight;

void main() {

	fragColor = Color * uLight;

}
