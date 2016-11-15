#version 330 core

in vec3 vPosition;
in vec3 vColor;
out vec3 color;

void main()
{
    gl_Position = vec4(vPosition, 1);
    color = vColor;
}
