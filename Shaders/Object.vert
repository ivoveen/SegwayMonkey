#version 310 es

precision mediump float;     
in vec3 a_position;   
in vec2 a_texCoord;   
in vec3 a_normal;
uniform mat4 VP;   
uniform mat4 model;
uniform vec3 cameraPosition;
uniform float radius;

//uniform mat4 model;
out vec2 v_texCoord; 
out vec3 v_normal;
out vec3 model_v_pos;
out float dist;

void main()
{
	//float radius = 2000.0f;
	v_texCoord = a_texCoord; 
	v_normal = mat3(transpose(inverse(model))) * a_normal;  

	model_v_pos = vec3(model * vec4(a_position, 1.0));
	dist = length(cameraPosition.xz - model_v_pos.xz);
	model_v_pos.y += sqrt(-(dist*dist) + (radius*radius)) - radius; //I got this line of code from Damy

	//model_v_pos.y += dist;

	gl_Position =  VP * vec4(model_v_pos,1);
	
}
