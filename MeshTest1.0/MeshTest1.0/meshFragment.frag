#version 410 core

in vec2 v2fTexCoord;

out vec4 outColor;

uniform int _Pick;
uniform sampler2D _Diffuse;

void main()
{
	outColor = texture(_Diffuse, v2fTexCoord);
	//outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	if(_Pick != 0)
	{
		outColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}