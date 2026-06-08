#version 310 es

precision mediump float;
out vec4 FragColor;
in vec4 v_colour;

void main()
{
	FragColor = v_colour;
}

