#include "Angel.h"

#pragma comment(lib, "glew32.lib")

const vec3 WHITE(1.0, 1.0, 1.0);
const vec3 BLACK(0.0, 0.0, 0.0);
const vec3 RED(1.0, 0.0, 0.0);
const vec3 GREEN(0.0, 1.0, 0.0);
const vec3 BLUE(0.0, 0.0, 1.0);

const int TRIANGLE_NUM_POINTS = 3;
const int SQUARE_NUM = 1;
const int SQUARE_NUM_POINTS = 4 * SQUARE_NUM;
const int TOTAL_NUM_POINTS = TRIANGLE_NUM_POINTS + SQUARE_NUM_POINTS;

// 获得三角形的每个角度
double getTriangleAngle(int point)
{
	return 2 * M_PI / 3 * point;
}

// 获得正方形的每个角度
double getSquareAngle(int point)
{
	return M_PI / 4 + (M_PI / 2 * point);
}

// 生成三角形上的每个点
void generateTrianglePoints(vec2 vertices[], vec3 colors[], int startVertexIndex)
{
	/*在此函数中修改三角形的顶点位置*/

	double scale = 0.25;
	vec2 center(0.0, 0.70);

	for (int i = 0; i < 3; ++i)
	{
		// 当前顶点对应的角度
		double currentAngle = getTriangleAngle(i);
		vertices[startVertexIndex + i] = vec2(cos(currentAngle), sin(currentAngle)) * scale + center;
	}

	colors[startVertexIndex] = RED;
	colors[startVertexIndex + 1] = GREEN;
	colors[startVertexIndex + 2] = BLUE;
}

// 生成正方形上的每个点
void generateSquarePoints(vec2 vertices[], vec3 colors[], int squareNumber, int startVertexIndex)
{
	/*在此函数中修改，生成多个嵌套正方形*/

	double scale = 0.90;
	vec2 center(0.0, -0.25);
	int vertexIndex = startVertexIndex;

	vec3 currentColor = WHITE;
	for (int j = 0; j < 4; ++j)
	{
		// 当前顶点对应的角度
		double currentAngle = getSquareAngle(j);
		vertices[vertexIndex] = vec2(cos(currentAngle), sin(currentAngle)) * scale + center;
		colors[vertexIndex] = currentColor;
		vertexIndex++;
	}
}

// 负责设置程序中用到的数据
void init()
{
	vec2 vertices[TOTAL_NUM_POINTS];
	vec3 colors[TOTAL_NUM_POINTS];

	// 调用生成形状顶点位置的函数
	generateTrianglePoints(vertices, colors, 0);
	generateSquarePoints(vertices, colors, SQUARE_NUM, TRIANGLE_NUM_POINTS);

	// 创建顶点数组对象
	GLuint vao[1];
	// 分配1个顶点数组对象
	glGenVertexArrays(1, vao);
	// 绑定顶点数组对象
	glBindVertexArray(vao[0]);

	// 创建顶点缓存对象
	GLuint buffer;
	// 分配1个顶点数组对象
	glGenBuffers(1, &buffer);
	// 绑定顶点缓存对象
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// 分配数据所需的存储空间，将数据拷贝到OpenGL服务端内存
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);

	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	// 读取着色器并使用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	// 启用顶点属性数组
	glEnableVertexAttribArray(pLocation);
	// 关联到顶点属性数组 (index, size, type, normalized, stride, *pointer)
	glVertexAttribPointer(pLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 从片元着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));

	// 设置了当前使用的清除颜色值，这里设置为黑色背景
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

// 负责渲染需要的内容
void display(void)
{
	// 清理指定缓存数据并重设为当前的清除值
	glClear(GL_COLOR_BUFFER_BIT);

	// 使用当前绑定的顶点数据建立几何图元 (mode, first, count)
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM_POINTS);

	// 绘制多个正方形
	for (int i = 0; i < SQUARE_NUM; ++i)
	{
		glDrawArrays(GL_TRIANGLE_FAN, TRIANGLE_NUM_POINTS + (i * 4), 4);
	}

	// 强制所有进行中的OpenGL命令完成
	//glFlush()
	glutSwapBuffers();
}

// 创建窗口、初始化、进入事件循环
int main(int argc, char **argv)
{
	// 初始化GLUT库，必须是应用程序调用的第一个GLUT函数
	glutInit(&argc, argv);

	// 配置窗口的显示特性
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Simple 2D Shapes");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	// 指定当前窗口进行重绘时要调用的函数
	glutDisplayFunc(display);

	// 负责一直处理窗口和操作系统的用户输入等操作
	glutMainLoop();
	return 0;
}
