#version 310 es

precision mediump float;  
out vec4 FragColor;
in vec3 v_normal;
in vec3 model_v_pos;
in vec3 v_color;
in float dist;
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
//diffuse lighting
	vec3 normal = normalize(v_normal);
	vec3 lightColor = vec3(1.0,1.0,1.0);
	vec3 lightDirection = normalize( lightPosition - model_v_pos);
	float diffuseStrength = max(dot(normal,lightDirection), 0.0);
	vec3 diffuse = diffuseStrength * lightColor;

//specular lighting
//	vec3 viewDirection = normalize(cameraPosition - model_v_pos );
//	vec3 reflectDirection = normalize(reflect(-lightDirection,normal));
//	float specularStrength = max(0.0, dot(viewDirection,reflectDirection));
//	specularStrength = pow(specularStrength,64.0);
//	vec3 specular = specularStrength * lightColor;

//final lighting
	vec4 lighting = vec4(0.0,0.0,0.0,1.0);
	vec4 Ambient =  vec4(1.0, 1.0, 1.0, 1.0) * ambientPercent;
	lighting =  Ambient *  0.4 + vec4(diffuse,1.0);// + vec4(specular,1.0) * 1.6; 
	vec4 lightFragColor = vec4(v_color,1.0) * lighting;
	FragColor = lightFragColor;

////fog code
	float fogFactor = clamp(dist / fogDistance, 0.0, 1.0);
    vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);  //white fog
    FragColor = mix(lightFragColor, fogColor, fogFactor);

	// normal display debug
  //  vec3 normalColor = normal * 0.5 + 0.5;
  //  FragColor = vec4(normalColor, 1.0);
}                                                