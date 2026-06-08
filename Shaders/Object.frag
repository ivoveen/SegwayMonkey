#version 310 es

precision mediump float;  
out vec4 FragColor;
in vec2 v_texCoord;       
in vec3 v_normal;
in vec3 model_v_pos;
in float dist;
uniform sampler2D s_texture;
uniform float  ambientPercent;
uniform mat4 model;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float fogDistance;
void main()              
{   
//this Phong shading is based off of a youtube video: 
// https://www.youtube.com/watch?v=LKXAIuCaKAQ&ab_channel=SuboptimalEngineer
//The code was changed by me but the basic idea is based off of the video.
// convert positions to model space

//diffuse lighting
	vec3 normal = normalize(v_normal);
	vec3 lightColor = vec3(1.0,1.0,1.0);
	vec3 lightDirection = normalize( lightPosition - model_v_pos);
	float diffuseStrength = max(0.0, dot(normal,lightDirection));
	vec3 diffuse = diffuseStrength * lightColor;

//specular lighting
	vec3 viewDirection = normalize(cameraPosition - model_v_pos );
	vec3 reflectDirection = normalize(reflect(-lightDirection,normal));
	float specularStrength = max(0.0, dot(viewDirection,reflectDirection));
	specularStrength = pow(specularStrength,64.0);
	vec3 specular = specularStrength * lightColor;

//final lighting
	vec4 lighting = vec4(0.0,0.0,0.0,1.0);
	vec4 Ambient =  vec4(1.0, 1.0, 1.0, 1.0) * ambientPercent;
	lighting = Ambient *  0.4 + vec4(diffuse,1.0) * 1.4 + vec4(specular,1.0) * 1.6;
	vec4 lightFragColor =  texture( s_texture, v_texCoord ) * lighting;

//fog code
	float fogFactor = clamp(dist / fogDistance, 0.0, 1.0);
    vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);  //white fog
    FragColor = mix(lightFragColor, fogColor, fogFactor);

}                                                