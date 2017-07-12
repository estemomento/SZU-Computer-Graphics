#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform mat4 rotationMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projMatrix;

out vec3 N;
out vec3 V;

// Phong 光照模型的实现 (per-fragment shading)

void main()
{
	vec4 v = projMatrix * modelViewMatrix * vec4(vPosition, 1.0);
	gl_Position = rotationMatrix * vec4(v.xyz / v.w, 1.0);

	// 将顶点变换到相机坐标系下
	vec4 vertPos_cameraspace = modelViewMatrix * vec4(vPosition, 1.0);

	// 将法向量变换到相机坐标系下并传入片元着色器
	N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;
	
	// 对顶点坐标做透视投影
	V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
}