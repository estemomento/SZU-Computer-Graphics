/*
*        Computer Graphics Course - Shenzhen University
*       Week 7 - Shadow Projection Matrix Skeleton Code
* ============================================================
*
* - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
* - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
*/

#include "Angel.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <iostream>

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint rotationMatrixID;
GLuint modelViewMatrixID;
GLuint projMatrixID;
GLuint fColorID;

// 相机视角参数
float l = -2.0, r = 2.0;    // 左右裁剪平面
float b = -2.0, t = 2.0;    // 上下裁剪平面
float n = -2.0, f = 2.0;    // 远近裁剪平面
float rotationAngle = -5.0; // 旋转角度

vec4 red(1.0, 0.0, 0.0, 1.0);
vec4 black(0.0, 0.0, 0.0, 1.0);

float lightPos[3] = {-0.5, 2.0, 0.5};

//////////////////////////////////////////////////////////////////////////
// 相机参数控制

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	mat4 ortho( const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar )
	{
		// TODO 请按照实验课讲解补全正交投影矩阵的计算
		return mat4(1.0);
	}

	mat4 perspective( const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		// TODO 请按照实验课讲解补全透视投影矩阵的计算
		return mat4(1.0);
	}

	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		return mat4(1.0);
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenGL 初始化

void init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	vec3 points[3];
	points[0] = vec3( -0.5, 0.5,  0.5);
	points[1] = vec3(  0.5, 0.5,  0.5);
	points[2] = vec3(  0.0, 0.75, 0.0);

	programID = InitShader("vshader.glsl", "fshader.glsl");

	// 从顶点着色器中获取相应变量的位置
	vPositionID = glGetAttribLocation(programID, "vPosition");

	rotationMatrixID = glGetUniformLocation(programID, "rotationMatrix");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	fColorID = glGetUniformLocation(programID, "fColor");

	// 生成VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 生成VBO，并绑定顶点坐标
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// OpenGL相应状态设置
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

//////////////////////////////////////////////////////////////////////////
// 渲染

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO 设置模-视变换矩阵，投影矩阵
	// （因为本节重点不在于投影变换，所以将投影矩阵设置为正交投影即可）
	//
	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = mat4(1.0);
	Camera::projMatrix = Ortho(-2, 2, -2, 2, -2, 2);

	// 为方便观察投影，设置场景旋转矩阵
	mat4 rotationMatrix = RotateX(rotationAngle);
	glUniformMatrix4fv(rotationMatrixID, 1, GL_TRUE, &rotationMatrix[0][0]);

	glEnableVertexAttribArray(vPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		vPositionID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	// TODO 在正常的投影矩阵下绘制原始的三角形（用红色表示）
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mat4 projMatrix = Camera::projMatrix;

	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, projMatrix);

	glUniform4fv(fColorID, 1, red);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// TODO 计算阴影投影矩阵，绘制投影之后的三角形（用黑色表示）
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];

	mat4 shadowProjMatrix(-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0.0, 0.0, -ly);

	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &shadowProjMatrix[0][0]);

	glUniform4fv(fColorID, 1, black);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// 重新设置窗口

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// 鼠标响应函数

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		lightPos[0] = (x - 250) / 10;
		lightPos[1] = (250 - y) / 10;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// 键盘响应函数

void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	case 'x':		// 'x' 键使得场景旋转-1.0度
		rotationAngle -= 1.0;
		break;
	case 'X':		// 'X' 键使得场景旋转+1.0度
		rotationAngle += 1.0;
		break;
	}
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void idle(void)
{
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL-Tutorial");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}