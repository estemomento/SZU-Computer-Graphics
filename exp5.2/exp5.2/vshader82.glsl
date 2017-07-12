#version 150 

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform  vec4 draw_color;

void main() 
{
    //color = vColor;
    color = draw_color;
    gl_Position = Projection*ModelView*vPosition;
} 
