#include "Angel.h"

#pragma comment(lib, "glew32.lib")

// ������ɫ��ͼ�ε�������
const vec3 WHITE(1.0, 1.0, 1.0);
const vec3 BLACK(0.0, 0.0, 0.0);
const vec3 RED(1.0, 0.0, 0.0);
const vec3 GREEN(0.0, 1.0, 0.0);
const vec3 BLUE(0.0, 0.0, 1.0);

const int CIRCLE_NUM_POINTS = 100;
const int ELLIPSE_NUM_POINTS = 100;
const int TRIANGLE_NUM_POINTS = 3;
const int SQUARE_NUM = 6;
const int SQUARE_NUM_POINTS = 4 * SQUARE_NUM;
const int TOTAL_NUM_POINTS = TRIANGLE_NUM_POINTS + SQUARE_NUM_POINTS + CIRCLE_NUM_POINTS + ELLIPSE_NUM_POINTS;

// ������Բ�߽��ĵ�
vec2 getEllipseVertex(vec2 center, double scale, double verticalScale, double angle)
{
	vec2 vertex(sin(angle), cos(angle));
	vertex += center;
	vertex *= scale;
	vertex.y *= verticalScale;
	return vertex;
}

// ���ݽǶ�������ɫ
float generateAngleColor(double angle)
{
	return 1.0 / (2 * M_PI) * angle;
}

// ���������ε�ÿ���Ƕ�
double getTriangleAngle(int point)
{
	return 2 * M_PI / 3 * point;
}

// ���������ε�ÿ���Ƕ�
double getSquareAngle(int point)
{
	return M_PI / 4 + (M_PI / 2 * point);
}

// ������Բ�ϵ�ÿ����
void generateEllipsePoints(vec2 vertices[], vec3 colors[], int startVertexIndex, int numPoints, vec2 center, double scale, double verticalScale)
{
	double angleIncrement = (2 * M_PI) / numPoints;
	double currentAngle = M_PI / 2;

	for (int i = startVertexIndex; i < startVertexIndex + numPoints; ++i)
	{
		vertices[i] = getEllipseVertex(center, scale, verticalScale, currentAngle);
		// �� verticalScale ����Ϊ1.0��Ҳ����Բ�����ø��ݽǶ����ɺ������佥����ɫ
		if (verticalScale == 1.0)
		{
			colors[i] = vec3(generateAngleColor(currentAngle), 0.5, 0.75);
		}
		// ��Ϊ��Բ ������ɫ
		else
		{
			colors[i] = BLUE;
		}
		currentAngle += angleIncrement;
	}
}

// �����������ϵ�ÿ����
void generateTrianglePoints(vec2 vertices[], vec3 colors[], int startVertexIndex)
{
	double scale = 0.25;
	vec2 center(0.0, 0.80);

	for (int i = 0; i < 3; ++i)
	{
		// ��ǰ������Ӧ�ĽǶ�
		double currentAngle = getTriangleAngle(i);
		// vec2���������������η���
		vertices[startVertexIndex + i] = vec2(sin(currentAngle), -cos(currentAngle)) * scale + center;
	}

	// ����������������ɫ
	colors[startVertexIndex] = RED;
	colors[startVertexIndex + 1] = WHITE;
	colors[startVertexIndex + 2] = BLUE;
}

// �����������ϵ�ÿ����
void generateSquarePoints(vec2 vertices[], vec3 colors[], int squareNumber, int startVertexIndex)
{
	double scale = 0.90;
	vec2 center(0.0, -0.25);
	int vertexIndex = startVertexIndex;

	for (int is = 0; is < SQUARE_NUM; is++)
	{
		vec3 currentColor = RED;
		// ������ɫ�ı仯
		currentColor = (is % 2) ? GREEN : WHITE;
		for (int j = 0; j < 4; ++j)
		{
			// ��ǰ������Ӧ�ĽǶ�
			double currentAngle = getSquareAngle(j);
			vertices[vertexIndex] = vec2(cos(currentAngle), sin(currentAngle)) * scale + center;
			colors[vertexIndex] = currentColor;
			vertexIndex++;
		}
		scale -= 0.15;
	}
}

// �������ó������õ�������
void init()
{
	vec2 vertices[TOTAL_NUM_POINTS];
	vec3 colors[TOTAL_NUM_POINTS];

	// ������Բ��Բ�Ļ������ĵ�
	vec2 ellipseCenter(-3, 6);
	vec2 circleCenter(3, 3.75);

	// ����������״����λ�õĺ���
	generateTrianglePoints(vertices, colors, 0);
	generateSquarePoints(vertices, colors, SQUARE_NUM, TRIANGLE_NUM_POINTS);
	generateEllipsePoints(vertices, colors, TRIANGLE_NUM_POINTS + SQUARE_NUM_POINTS, ELLIPSE_NUM_POINTS, ellipseCenter, 0.2, 0.6);
	generateEllipsePoints(vertices, colors, TRIANGLE_NUM_POINTS + SQUARE_NUM_POINTS + ELLIPSE_NUM_POINTS, CIRCLE_NUM_POINTS, circleCenter, 0.2, 1);

	// ����������������
	GLuint vao[1];
	// ����1��������������
	glGenVertexArrays(1, vao);
	// �󶨶�����������
	glBindVertexArray(vao[0]);

	// �������㻺������
	GLuint buffer;
	// ����1��������������
	glGenBuffers(1, &buffer);
	// �󶨶��㻺������
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// �������������Ĵ洢�ռ䣬�����ݿ�����OpenGL�������ڴ�
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);

	// �ֱ���ȡ����
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	// ��ȡ��ɫ����ʹ��
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// �Ӷ�����ɫ���г�ʼ��������λ��
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	// ���ö�����������
	glEnableVertexAttribArray(pLocation);
	// ������������������ (index, size, type, normalized, stride, *pointer)
	glVertexAttribPointer(pLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// ��ƬԪ��ɫ���г�ʼ����������ɫ
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));

	// �����˵�ǰʹ�õ�������ɫֵ����������Ϊ��ɫ����
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

// ������Ⱦ��Ҫ������
void display(void)
{
	// ����ָ���������ݲ�����Ϊ��ǰ������ֵ
	glClear(GL_COLOR_BUFFER_BIT);

	// ʹ�õ�ǰ�󶨵Ķ������ݽ�������ͼԪ (mode, first, count)
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM_POINTS);

	// ���ƶ���������
	for (int i = 0; i < SQUARE_NUM; ++i)
	{
		glDrawArrays(GL_TRIANGLE_FAN, TRIANGLE_NUM_POINTS + (i * 4), 4);
	}

	// ������Բ
	glDrawArrays(GL_POLYGON, SQUARE_NUM_POINTS + TRIANGLE_NUM_POINTS, ELLIPSE_NUM_POINTS);

	// ����Բ��
	glDrawArrays(GL_POLYGON, SQUARE_NUM_POINTS + TRIANGLE_NUM_POINTS + ELLIPSE_NUM_POINTS, CIRCLE_NUM_POINTS);

	// ǿ�����н����е�OpenGL��������
	//glFlush()
	glutSwapBuffers();
}

// �������ڡ���ʼ���������¼�ѭ��
int main(int argc, char **argv)
{
	// ��ʼ��GLUT�⣬������Ӧ�ó������õĵ�һ��GLUT����
	glutInit(&argc, argv);

	// ���ô��ڵ���ʾ����
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("ʵ��1");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	// ָ����ǰ���ڽ����ػ�ʱҪ���õĺ���
	glutDisplayFunc(display);

	// ����һֱ�������ںͲ���ϵͳ���û������Ȳ���
	glutMainLoop();
	return 0;
}
