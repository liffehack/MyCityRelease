#version 330 core

uniform samplerCube tex;

in vec3 TexCoord;
out vec4 FragColor;

void main (void)
{
	FragColor = texture(tex, TexCoord);
}