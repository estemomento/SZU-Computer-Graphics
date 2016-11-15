#include "Angel.h"
#include "mat.h"
#include "vec.h"

#include <vector>
#include <string>
#include <fstream>

#pragma comment(lib, "glew32.lib")

// 三角面片中的顶点序列
typedef struct vIndex {
    unsigned int a, b, c;
    vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

int axis;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;
const int ANIMATION_START = 3;
const int ANIMATION_STOP = 4;
const int ANIMATION_AXIS = 5;

const double DELTA_DELTA = 0.1;    // Delta的变化率
const double DEFAULT_DELTA = 0.3;    // 默认的Delta值

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

vec3 scaleTheta(1.0, 1.0, 1.0);    // 缩放控制变量
vec3 rotateTheta(0.0, 0.0, 0.0);    // 旋转控制变量
vec3 translateTheta(0.0, 0.0, 0.0);    // 平移控制变量

GLint matrixLocation;
int currentTransform = TRANSFORM_TRANSLATE;    // 设置当前变换
int mainWindow;
const int NUM_VERTICES = 2904;

std::string filename;
std::vector<vec3i> faces;

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

// 单位立方体的各个点
vec3 vertices[NUM_VERTICES];

void read_off(const std::string filename)
{
	if (filename.empty())
		return;

	std::ifstream fin;
	fin.open(filename);
	std::string offchar;

	fin >> offchar; // 读取第一行的OFF
	fin >> nVertices >> nFaces >> nEdges; // 读取顶点面片的数据

	// 存储数据
	for (int i = 0; i < nVertices; i++)
	{
		double x, y, z;
		fin >> x >> y >> z;
		vertices[i] = vec3(x, y, z);
	}
	for (int i = 0; i < nFaces; i++)
	{
		int n, x, y, z;
		fin >> n >> x >> y >> z;
		faces.push_back(vec3i(x, y, z));
	}

	fin.close();
}

void init()
{

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(vec3), vertices, GL_STATIC_DRAW);

    // 创建并初始化顶点索引缓存对象
    GLuint vertexIndexBuffer;
    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(vec3i), faces.data(), GL_STATIC_DRAW);

	// 读取着色器并使用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 获得矩阵存储位置
	matrixLocation = glGetUniformLocation(program, "matrix");

	// 黑色背景
	glClearColor(0, 0, 0, 0);
}

void display()
{
	// 清理窗口
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 生成变换矩阵
	mat4 m( 1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

    // 计算变换矩阵
	switch (currentTransform)
	{
	case TRANSFORM_SCALE:
		m += Scale(scaleTheta);
		break;
	case TRANSFORM_ROTATE:
		m *= RotateX(rotateTheta.x) * RotateY(rotateTheta.y) * RotateZ(rotateTheta.z);
		break;
	case TRANSFORM_TRANSLATE:
		m += Translate(translateTheta);
		break;
	}

	// 从指定位置matrixLocation中传入变换矩阵m
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
    // 绘制立方体中的各个三角形
    glDrawElements(GL_TRIANGLES, int(faces.size() * 3), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glutSwapBuffers();
}

// 通过Delta值更新Theta
void updateTheta(int axis, int sign) {
	switch (currentTransform) {
	case TRANSFORM_SCALE:
		scaleTheta[axis] += sign * scaleDelta; 
		break;
	case TRANSFORM_ROTATE: 
		rotateTheta[axis] += sign * rotateDelta; 
		break;
	case TRANSFORM_TRANSLATE: 
		translateTheta[axis] += sign * translateDelta;
		break;
	}
}

// 复原Theta和Delta
void resetTheta()
{
	scaleTheta = vec3(1.0, 1.0, 1.0);
	rotateTheta = vec3(0.0, 0.0, 0.0);
	translateTheta = vec3(0.0, 0.0, 0.0);
	scaleDelta = DEFAULT_DELTA;
	rotateDelta = DEFAULT_DELTA;
	translateDelta = DEFAULT_DELTA;
}

// 更新变化Delta值
void updateDelta(int sign)
{
	switch (currentTransform) {
	case TRANSFORM_SCALE:
		scaleDelta += sign * DELTA_DELTA;
		break;
	case TRANSFORM_ROTATE:  
		rotateDelta += sign * DELTA_DELTA; 
		break;
	case TRANSFORM_TRANSLATE: 
		translateDelta += sign * DELTA_DELTA;
		break;
	}
}

// 手动操作键盘函数
void manualKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		updateTheta(X_AXIS, 1);
		break;
	case 'a':
		updateTheta(X_AXIS, -1);
		break;
	case 'w':
		updateTheta(Y_AXIS, 1);
		break;
	case 's':
		updateTheta(Y_AXIS, -1);
		break;
	case 'e':
		updateTheta(Z_AXIS, 1);
		break;
	case 'd':
		updateTheta(Z_AXIS, -1);
		break;
	case 'r':
		updateDelta(1);
		break;
	case 'f':
		updateDelta(-1);
		break;
	case 't':
		resetTheta();
		break;
	case 033:
		// Esc按键
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostWindowRedisplay(mainWindow);
}

// 自动操作键盘函数
void autoKeyboard(unsigned char key, int x, int y)
{
	// 设置旋转轴以及变化速率
	switch (key)
	{
	case 'x':
		axis = X_AXIS;
		break;
	case 'y':
		axis = Y_AXIS;
		break;
	case 'z':
		axis = Z_AXIS;
		break;
	case 'r':
		updateDelta(1);
		break;
	case 'f':
		updateDelta(-1);
		break;
	}
	glutPostWindowRedisplay(mainWindow);
}

// 自动调用旋转函数
void idleFunction()
{
	// 按键改变旋转轴
	glutKeyboardFunc(autoKeyboard);
	// 调用updateTheta旋转
	updateTheta(axis, 1);
	// 标记主窗口重绘
	glutPostWindowRedisplay(mainWindow);
}

// 鼠标操作
void mainWindowMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// 按下鼠标左键，指定当没有其他事件处理时，调用idleFunction
		currentTransform = TRANSFORM_ROTATE;
		glutIdleFunc(idleFunction);
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		// 按下鼠标右键，解除调用，恢复手动键盘指令
		glutIdleFunc(NULL);
		glutKeyboardFunc(manualKeyboard);
	}
}

// 菜单操作
void menuEvents(int menuChoice)
{
	switch (menuChoice)
	{
		// 设置操作类型
	case TRANSFORM_SCALE: case TRANSFORM_ROTATE: case TRANSFORM_TRANSLATE:
		currentTransform = menuChoice;
		break;
		// 调用idleFunction控制动画开始
	case ANIMATION_START:
		currentTransform = TRANSFORM_ROTATE;
		glutIdleFunc(idleFunction);
		break;
		// 控制动画停止，恢复手动键盘指令
	case ANIMATION_STOP:
		glutIdleFunc(NULL);
		glutKeyboardFunc(manualKeyboard);
		break;
		// 设置旋转轴
	case ANIMATION_AXIS + X_AXIS: case ANIMATION_AXIS + Y_AXIS: case ANIMATION_AXIS + Z_AXIS:
		axis = menuChoice - ANIMATION_AXIS;
	default:
		break;
	}
}

// 菜单选项设定
void setupMenu()
{
	int sub = glutCreateMenu(menuEvents);
	glutAddMenuEntry("X", ANIMATION_AXIS + X_AXIS);
	glutAddMenuEntry("Y", ANIMATION_AXIS + Y_AXIS);
	glutAddMenuEntry("Z", ANIMATION_AXIS + Z_AXIS);
	glutCreateMenu(menuEvents);
	glutAddMenuEntry("Scale", TRANSFORM_SCALE);
	glutAddMenuEntry("Rotate", TRANSFORM_ROTATE);
	glutAddMenuEntry("Translate", TRANSFORM_TRANSLATE);
	glutAddMenuEntry("Start Animation", ANIMATION_START);
	glutAddMenuEntry("Stop Animation", ANIMATION_STOP);
	glutAddSubMenu("Rotation Axis", sub);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void printHelp() {
	printf("%s\n\n", "3D Transfomations");
	printf("Keyboard options:\n");
	printf("	r: Increase delta of currently selected transform\n");
	printf("	f: Decrease delta of currently selected transform\n");
	printf(" - While not in animation\n");
	printf("	q: Increase x\n");
	printf("	a: Decrease x\n");
	printf("	w: Increase y\n");
	printf("	s: Decrease y\n");
	printf("	e: Increase z\n");
	printf("	d: Decrease z\n");
	printf("	t: Reset all transformations and deltas\n");
	printf("	Esc: Exit Program\n");
	printf(" - While in animation\n");
	printf("	x: Set X Axis as rotation axis\n");
	printf("	y: Set Y Axis as rotation axis\n");
	printf("	z: Set Z Axis as rotation axis\n");
	printf("Mouse options:\n");
	printf("	Left Button: Start Animation\n");
	printf("	Right Button: Stop Animation\n");
	printf("	Middle Button: Open Menu\n");
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 窗口支持双重缓冲和深度测试
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	mainWindow = glutCreateWindow("2015160196_刘畅宇_实验2");

	glewExperimental = GL_TRUE;
	glewInit();
	
	// 读入OFF
	read_off("cow.off");

	init();
	setupMenu();
	glutDisplayFunc(display);
	// 启用键鼠函数
	glutMouseFunc(mainWindowMouse);
	glutKeyboardFunc(manualKeyboard);
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}
