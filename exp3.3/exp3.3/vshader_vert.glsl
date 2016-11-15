#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 lightPos;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;

out vec4 color;

// Phong 光照模型的实现 (per-vertex shading)

void main()
{
	gl_Position = modelViewProjMatrix * vec4(vPosition, 1.0);

	// TODO 将顶点坐标变换到相机坐标系
	vec4 vertPos_cameraspace = modelViewMatrix * vec4(vPosition, 1.0);
	vec3 V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
	vec3 N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;
	
	// TODO 设置三维物体的材质属性
	vec3 ambiColor = vec3(0.2, 0.2, 0.2);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);
	
	// TODO 计算N，L，V，R四个向量并归一化
	vec3 N_norm = normalize(N);
	vec3 L_norm = normalize(lightPos - V);
	vec3 V_norm = normalize(-V);
	vec3 R_norm = reflect(L_norm, N_norm);
	
	// TODO 计算漫反射系数和镜面反射系数
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	float specular = clamp(dot(R_norm, V_norm), 0.0, 1.0);
	
	// float shininess = 10.0;
	
	// TODO 计算最终每个顶点的输出颜色
	color = vec4(ambiColor + 
							 diffColor * lambertian + 
							 specColor * pow(specular, 10.0),
							1.0);
}