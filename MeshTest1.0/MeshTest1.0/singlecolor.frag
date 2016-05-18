#version 410 core

in vec4 v2fColor;

out vec4 outColor;

uniform int _Pick;

void main()
{
	outColor = v2fColor;
	if(_Pick != 0)
	{
		outColor += vec4(0.2f, 0.0f, 0.2f, 1.0f);
	}
}