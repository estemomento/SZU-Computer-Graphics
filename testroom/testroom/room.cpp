#include "Angel.h"
#include "TriMesh.h"
#include <vector>
//#include "bmp_loader.h"

#pragma comment(lib, "glew32.lib")
using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexNormalID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint vNormalID;
GLuint modelViewMatrixID;
GLuint projMatrixID;
GLuint rotationMatrixID;

GLuint lightPosID;
GLuint shadowID;

TriMesh* mesh = new TriMesh();

vec3 lightPos = vec3(0.5, 2.0, 1.0);
float rotationAngle = -25.0;

// 观察点位置
vec3 eye(0.7, 0, 0.3);
vec3 at(-100, 0, 0);
vec3 up(0, 0, 1);

GLuint texture[3]; // 保存纹理

namespace Camera
{
	mat4 modelMatrix(1.0);
	mat4 viewMatrix(1.0);
	mat4 projMatrix = Ortho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
}

void init(void)
{
	programID = InitShader("vshader.glsl", "fshader.glsl");

	// 从顶点着色器和片元着色器中获取变量的位置
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vNormalID = glGetAttribLocation(programID, "vNormal");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	rotationMatrixID = glGetUniformLocation(programID, "rotationMatrix");
	lightPosID = glGetUniformLocation(programID, "lightPos");
	shadowID = glGetUniformLocation(programID, "isShadow");

	// 读取外部三维模型
	mesh->read_off("sphere.off");

	vector<vec3f> vs = mesh->v();
	vector<vec3i> fs = mesh->f();
	vector<vec3f> ns;

	// 设置偏移向量，让阴影移动到模型y最小的点
	vec3f shift = vs[0];
	for (int i = 0; i < vs.size(); ++i)
		if (vs[i].y < shift.y) shift = vs[i];

	// 计算球模型在每个顶点的法向量，并存储到ns数组中
	for (int i = 0; i < vs.size(); ++i)
	{
		vs[i].y -= shift.y;
		ns.push_back(vs[i] - vec3(0.0, -shift.y, 0.0));
	}

	// 生成VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 生成VBO，并绑定顶点数据
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// 生成VBO，并绑定法向量数据
	glGenBuffers(1, &vertexNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(vec3f), ns.data(), GL_STATIC_DRAW);

	// 生成VBO，并绑定顶点索引
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	// 加载纹理
	//LoadGLTextures("Data/giger1.bmp", &texture[0]);
	//LoadGLTextures("Data/floor.bmp", &texture[1]);
	//LoadGLTextures("Data/sky3.bmp", &texture[2]);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f); // 蓝色背景
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

// 画房间
void drawRoom()
{
	glBindTexture(GL_TEXTURE_2D, texture[0]); // 绑定墙壁的纹理
	glBegin(GL_QUADS);
	// 前边墙壁
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);

	// 后边墙壁
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);

	// 右边墙壁
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);

	// 左边墙壁
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]); // 绑定地面的纹理
	glBegin(GL_QUADS);
	// 地面      
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
}

// 画天空
void drawSky()
{
	glBindTexture(GL_TEXTURE_2D, texture[2]); // 绑定天空的纹理

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.0f, 50.0f); // 绘制原点

	glTexCoord2f(1.0f, 1.0f); glVertex3f(-150.0f, -150.0f, -50.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(150.0f, -150.0f, -50.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(150.0f, 150.0f, -50.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-150.0f, 150.0f, -50.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-150.0f, -150.0f, -50.0f);
	glEnd();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	glEnableVertexAttribArray(vPositionID);
	// 计算相机观察矩阵，并传入顶点着色器
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;

	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniform1i(shadowID, 0); // shader内设置是否为阴影选项，分别着色

	glDrawElements(GL_TRIANGLES, int(mesh->f().size() * 3), GL_UNSIGNED_INT, (void*)0);

	// 计算阴影投影矩阵，并传入顶点着色器
	float lx = lightPos.x;
	float ly = lightPos.y;
	float lz = lightPos.z;

	mat4 shadowProjMatrix(
		-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0.0, 0.0, -ly);

	shadowProjMatrix = shadowProjMatrix * modelViewMatrix;
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &shadowProjMatrix[0][0]);
	glUniform1i(shadowID, 1);

	// 设置旋转矩阵
	mat4 rotationMatrix = RotateX(rotationAngle);
	glUniformMatrix4fv(rotationMatrixID, 1, GL_TRUE, &rotationMatrix[0][0]);

	// 将相机位置传入顶点着色器
	glUniform3fv(lightPosID, 1, lightPos);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		vPositionID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(vNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glVertexAttribPointer(
		vNormalID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);
	//glLoadIdentity(); // Each time clean the matrix
	//gluLookAt(eye.x, eye.y, eye.z,
	//	at.x, at.y, at.z,
	//	up.x, up.y, up.z); // Translate the world to the current position
	//drawRoom();
	//drawSky();
	glutSwapBuffers(); // Double Buffer  
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 0.001, 400.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q': case 'Q': case 033: exit(0); break;
	}
}

void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: eye.y -= 0.1; break;
	case GLUT_KEY_RIGHT: eye.y += 0.1; break;
	case GLUT_KEY_UP: eye.x -= 0.1; break;
	case GLUT_KEY_DOWN: eye.x += 0.1; break;
	case GLUT_KEY_PAGE_UP: eye.z += 0.1; break;
	case GLUT_KEY_PAGE_DOWN: eye.z -= 0.1; break;
	default: return; // 按下其他键时不触发重绘
	}
	glutPostRedisplay();
}

void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);

	if (mesh)
	{
		delete mesh;
		mesh = NULL;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("room");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutMainLoop();
	clean();
	return 0;
}
