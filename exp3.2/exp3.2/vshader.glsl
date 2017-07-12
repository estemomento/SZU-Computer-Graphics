#version 330 core 

in vec3 vPosition;

uniform mat4 rotationMatrix;

uniform mat4 modelViewMatrix;
uniform mat4 projMatrix;

void main() 
{
	vec4 v1 = projMatrix * modelViewMatrix * vec4(vPosition, 1.0);
	vec4 v2 = vec4(v1.xyz / v1.w, 1.0);
	
	gl_Position = rotationMatrix * v2;
}