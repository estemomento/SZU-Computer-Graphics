#include "Angel.h"

#pragma comment(lib, "glew32.lib")

const int MENU_CHOICE_WHITE = 0;
const int MENU_CHOICE_BLACK = 1;
const int MENU_CHOICE_RED = 2;
const int MENU_CHOICE_GREEN = 3;
const int MENU_CHOICE_BLUE = 4;
const int MENU_CHOICE_YELLOW = 5;
const int MENU_CHOICE_ORANGE = 6;
const int MENU_CHOICE_PURPLE = 7;
const int MENU_CHOICE_START = 8;
const int MENU_CHOICE_STOP = 9;

const vec3 WHITE(1.0, 1.0, 1.0);
const vec3 BLACK(0.0, 0.0, 0.0);
const vec3 RED(1.0, 0.0, 0.0);
const vec3 GREEN(0.0, 1.0, 0.0);
const vec3 BLUE(0.0, 0.0, 1.0);
const vec3 YELLOW(1.0, 1.0, 0.0);
const vec3 ORANGE(1.0, 0.65, 0.0);
const vec3 PURPLE(0.8, 0.0, 0.8);

// 主窗口变量
const int SQUARE_NUM = 6;
const int SQUARE_NUM_POINTS = 4 * SQUARE_NUM;
int mainWindow;
int mainWindowMenu;
int mainWindowSubmenu;
int width = 600;    // 主窗口宽度
int height = 600;    // 主窗口高度
double offsetAngle = 0;    // 角度偏移量
double delta = 0.05;    // 每次改变角度偏移的变化量
vec3 mainWindowSquareColor = WHITE;

// 子窗口变量
const int ELLIPSE_NUM_POINTS = 100;
int subWindow;
int subWindowMenu;
vec3 subWindowObjectColor = RED;

// 获得圆上的点
vec2 getEllipseVertex(vec2 center, double scale, double verticleScale, double angle)
{
	vec2 vertex(sin(angle), cos(angle));
	vertex += center;
	vertex *= scale;
	vertex.y *= verticleScale;    // 修改垂直分量
	return vertex;
}

// 生成圆上顶点的属性
void generateEllipsePoints(vec2 vertices[], vec3 colors[], vec3 color, int startVertexIndex, int numPoints,
	vec2 center, double scale, double verticalScale)
{
	double angleIncrement = (2 * M_PI) / numPoints;
	double currentAngle = M_PI / 2;

	for (int i = startVertexIndex; i < startVertexIndex + numPoints; i++) {
		vertices[i] = getEllipseVertex(center, scale, verticalScale, currentAngle);
		colors[i] = color;

		currentAngle += angleIncrement;
	}
}

// 获得正方形的每个角度
double getSquareAngle(int point)
{
    return (M_PI / 4 + (M_PI / 2 * point)) + offsetAngle;
}

// 生成正方形上顶点的属性
void generateSquarePoints(vec2 vertices[], vec3 colors[], int squareNum, int startVertexIndex) {
	double scale = 0.90;
	double scaleAdjust = scale / squareNum;
	vec2 center(0.0, -0.25);

	int vertexIndex = startVertexIndex;

	for (int i = 0; i < squareNum; i++) {
		vec3 currentColor = 0 == i % 2 ? mainWindowSquareColor : BLACK;

		for (int j = 0; j < 4; j++) {
			double currentAngle = getSquareAngle(j);
			vertices[vertexIndex] = vec2(sin(currentAngle), cos(currentAngle)) * scale + center;
			colors[vertexIndex] = currentColor;

			vertexIndex++;
		}

		scale -= scaleAdjust;
	}
}

// 空闲回调函数
void idleFunction()
{
	// 改变角度的偏移量
    offsetAngle += delta;
    // 标记主窗口重绘
	glutPostWindowRedisplay(mainWindow);
}


void mainWindowInit()
{
	vec2 vertices[SQUARE_NUM * 4];
	vec3 colors[SQUARE_NUM * 4];

	// 创建主窗口中多个正方形
	generateSquarePoints(vertices, colors, SQUARE_NUM, 0);

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);

	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	// 读取着色器并使用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// 从片元着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	
	// 黑色背景
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

// 菜单回调函数
void mainWindowMenuEvents(int menuChoice)
{
	switch (menuChoice) {
	case MENU_CHOICE_WHITE:
		mainWindowSquareColor = WHITE;
		break;
	case MENU_CHOICE_BLACK:
		mainWindowSquareColor = BLACK;
		break;
	case MENU_CHOICE_RED:
		mainWindowSquareColor = RED;
		break;
	case MENU_CHOICE_GREEN:
		mainWindowSquareColor = GREEN;
		break;
	case MENU_CHOICE_BLUE:
		mainWindowSquareColor = BLUE;
		break;
	case MENU_CHOICE_YELLOW:
		mainWindowSquareColor = YELLOW;
		break;
	case MENU_CHOICE_ORANGE:
		mainWindowSquareColor = ORANGE;
		break;
	case MENU_CHOICE_PURPLE:
		mainWindowSquareColor = PURPLE;
		break;
	case MENU_CHOICE_START:
		glutIdleFunc(idleFunction);
		break;
	case MENU_CHOICE_STOP:
		glutIdleFunc(NULL);
		break;

	}

    // 标记mainWindow主窗口进行重绘
	glutPostWindowRedisplay(mainWindow);
}

// 创建和设置主窗口菜单
void mainWindowSetupMenu()
{
	// 创建子菜单，并注册菜单回调函数mainWindowMenuEvents
    mainWindowSubmenu = glutCreateMenu(mainWindowMenuEvents);
	glutAddMenuEntry("Yellow", MENU_CHOICE_YELLOW);
	glutAddMenuEntry("Orange", MENU_CHOICE_ORANGE);
	glutAddMenuEntry("Purple", MENU_CHOICE_PURPLE);
	glutAddMenuEntry("Black", MENU_CHOICE_BLACK);

    // 创建主菜单
	mainWindowMenu = glutCreateMenu(mainWindowMenuEvents);
	glutAddMenuEntry("Start Anime", MENU_CHOICE_START);
	glutAddMenuEntry("Stop Anime", MENU_CHOICE_STOP);
	glutAddMenuEntry("Red", MENU_CHOICE_RED);
	glutAddMenuEntry("Green", MENU_CHOICE_GREEN);
	glutAddMenuEntry("Blue", MENU_CHOICE_BLUE);
	glutAddMenuEntry("White", MENU_CHOICE_WHITE);
    
    // 在主菜单中添加子菜单
	glutAddSubMenu("Other Square Colors", mainWindowSubmenu);
    
    // 关联鼠标右键激活菜单
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mainWindowDisplay()
{
	mainWindowInit();    // 重绘时写入新的颜色数据
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < SQUARE_NUM; i++) {
		glDrawArrays(GL_TRIANGLE_FAN, (i * 4), 4);
	}

	glutSwapBuffers();
}

// 主窗口键盘回调函数
void mainWindowKeyboard(unsigned char key, int x, int y)
{
	if (key == 033)
		exit(EXIT_SUCCESS);

}

// 主窗口鼠标回调函数
void mainWindowMouse(int button, int state, int x, int y)
{
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		// 按下鼠标中键，指定当没有其他事件处理时，去调用idleFunction()这个函数
		glutIdleFunc(idleFunction);
	} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		// 释放鼠标中键，解除调用
		glutIdleFunc(NULL);
	}
}


void subWindowInit()
{
	vec2 vertices[ELLIPSE_NUM_POINTS];
	vec3 colors[ELLIPSE_NUM_POINTS];

	// 创建子窗口中的椭圆
	generateEllipsePoints(vertices, colors, subWindowObjectColor, 0, ELLIPSE_NUM_POINTS,
		vec2(0.0, 0.0), 0.7, 0.5);

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);

	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	// 读取着色器并复用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// 从片元着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));

	// 设置子窗口背景颜色为白色
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void subWindowDisplay()
{
	subWindowInit();    // 重绘时写入新的颜色数据
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_FAN, 0, ELLIPSE_NUM_POINTS);
	glutSwapBuffers();
}

// 子窗口键盘回调函数
void subWindowKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
		subWindowObjectColor = RED;
		break;
	case 'g':
		subWindowObjectColor = GREEN;
		break;
	case 'b':
		subWindowObjectColor = BLUE;
		break;
	case 'y':
		subWindowObjectColor = YELLOW;
		break;
	case 'o':
		subWindowObjectColor = ORANGE;
		break;
	case 'p':
		subWindowObjectColor = PURPLE;
		break;
	case 'w':
		subWindowObjectColor = WHITE;
		break;
	}

    // 标记subWindow子窗口进行重绘
	glutPostWindowRedisplay(subWindow);
}


void printHelp() {
	printf("%s\n\n", "Interaction and Submenu");
	printf("Keys to update the background color in sub window:\n");
	printf("'r' - red\n'g' - green\n'b' - blue\n'y' - yellow\n'o' - orange\n'p' - purple\n'w' - white\n");
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);    // 启用双重缓冲
	glutInitWindowSize(width, height);
	mainWindow = glutCreateWindow("Interaction and Submenu");

	glewExperimental = GL_TRUE;
	glewInit();
	
	mainWindowInit();
	mainWindowSetupMenu();
	glutDisplayFunc(mainWindowDisplay);
    // 在主窗口中指定函数mainWindowMouse，在鼠标按下或释放时将会被调用
	glutMouseFunc(mainWindowMouse);
	// 主窗口按键调用函数
	glutKeyboardFunc(mainWindowKeyboard);

	// 创建子窗口
	subWindow = glutCreateSubWindow(mainWindow, 0, 0, width / 4, height / 4);
	subWindowInit();
	glutDisplayFunc(subWindowDisplay);
    // 在子窗口中指定函数subWindowKeyboard，当一个能够生成ASCII字符的键释放时会被调用
	glutKeyboardFunc(subWindowKeyboard);

	
	// 输出帮助信息
	printHelp();

	glutMainLoop();
	return 0;
}
