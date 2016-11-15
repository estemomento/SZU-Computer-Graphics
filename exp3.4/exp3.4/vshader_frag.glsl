#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;

out vec3 N;
out vec3 V;

// Phong 光照模型的实现 (per-fragment shading)

void main()
{
	gl_Position = modelViewProjMatrix * vec4(vPosition, 1.0);

	// 将顶点变换到相机坐标系下
	vec4 vertPos_cameraspace = modelViewMatrix * vec4(vPosition, 1.0);
	
	// 将法向量变换到相机坐标系下并传入片元着色器
	N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;
	
	// 对顶点坐标做透视投影
	V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
}