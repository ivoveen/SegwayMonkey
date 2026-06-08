#version 310 es
precision mediump float;
// very simple for lines and tris
in vec3 a_position;
in vec3 a_colour;
uniform mat4 Matrix; // eiter mvp or vp
out vec4 v_colour;

void main()
{
	gl_Position = Matrix * vec4(a_position, 1.0);
	v_colour = vec4(a_colour, 0.5);

}

