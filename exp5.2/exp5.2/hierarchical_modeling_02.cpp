#pragma comment(lib,"glew32.lib")
#include "Angel.h"
#include <assert.h>
#include "TriMesh.h"
#include <stack>

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;

	mat4 LookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		vec4 vpn = at - eye;
		vec4 n = normalize(vpn);
		vec4 u = normalize(up * n);
		vec4 v = normalize(n * u);
		mat4 res = mat4(
			(u[1], u[2], u[3], 0),
			(v[1], v[2], v[3], 0),
			(n[1], n[2], n[3], 0),
			(0, 0, 0, 1)
		);
		return res * Translate(-eye);
	}
}

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

point4 color_torso = point4(0, 0, 1, 1);
point4 color_head = point4(0, 1, 0, 1);
point4 color_upper_arm = point4(1, 1, 0, 1);
point4 color_lower_arm = point4(1, 0, 0, 1);

//----------------------------------------------------------------------------

std::stack<mat4>  mvstack;
mat4         model_view;
GLuint       ModelView, Projection;
GLuint       draw_color;

//----------------------------------------------------------------------------

#define TORSO_HEIGHT 5.0
#define TORSO_WIDTH 3.0
#define UPPER_ARM_HEIGHT 3.0
#define LOWER_ARM_HEIGHT 2.0
#define UPPER_LEG_WIDTH  0.5
#define LOWER_LEG_WIDTH  0.5
#define LOWER_LEG_HEIGHT 2.0
#define UPPER_LEG_HEIGHT 3.0
#define UPPER_LEG_WIDTH  0.5
#define UPPER_ARM_WIDTH  0.5
#define LOWER_ARM_WIDTH  0.5
#define HEAD_HEIGHT 1.5
#define HEAD_WIDTH 1.0

// Set up menu item indices, which we can alos use with the joint angles
enum
{
	Torso = 0,
	Head = 1,
	Head1 = 1,
	Head2 = 2,
	LeftUpperArm = 3,
	LeftLowerArm = 4,
	RightUpperArm = 5,
	RightLowerArm = 6,
	LeftUpperLeg = 7,
	LeftLowerLeg = 8,
	RightUpperLeg = 9,
	RightLowerLeg = 10,
	NumNodes,
	Quit
};

// Joint angles with initial values
GLfloat
theta[NumNodes] = {
	0.0,    // Torso
	0.0,    // Head1
	0.0,    // Head2
	0.0,    // LeftUpperArm
	0.0,    // LeftLowerArm
	0.0,    // RightUpperArm
	0.0,    // RightLowerArm
	180.0,  // LeftUpperLeg
	0.0,     // LeftLowerLeg
	180.0,  // RightUpperLeg
	0.0    // RightLowerLeg
};

GLint angle = Head2;
struct Node
{
	mat4  transform;
	void(*render)(void);
	Node* sibling;
	Node* child;

	Node() :
		render(NULL), sibling(NULL), child(NULL)
	{
	}

	Node(mat4& m, void(*render)(void), Node* sibling, Node* child) :
		transform(m), render(render), sibling(sibling), child(child)
	{
	}
};

Node  nodes[NumNodes];
int Index = 0;

void quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

void colorcube(void)
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void traverse(Node* node)
{
	if (node == NULL) { return; }

	mvstack.push(model_view);

	model_view *= node->transform;
	node->render();

	if (node->child) { traverse(node->child); }

	model_view = mvstack.top();
	mvstack.pop();

	if (node->sibling) { traverse(node->sibling); }
}

//----------------------------------------------------------------------------

void torso()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glUniform4fv(draw_color, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, color_torso);

	model_view = mvstack.top();
	mvstack.pop();
}

void head()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
		Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glUniform4fv(draw_color, 1, color_head);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, color_head);

	model_view = mvstack.top();
	mvstack.pop();
}

void left_upper_arm()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH,
			UPPER_ARM_HEIGHT,
			UPPER_ARM_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glUniform4fv(draw_color, 1, color_upper_arm);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, color_upper_arm);

	model_view = mvstack.top();
	mvstack.pop();
}

void left_lower_arm()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH,
			LOWER_ARM_HEIGHT,
			LOWER_ARM_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glUniform4fv(draw_color, 1, color_lower_arm);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, color_lower_arm);

	model_view = mvstack.top();
	mvstack.pop();
}

void right_upper_arm()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH,
			UPPER_ARM_HEIGHT,
			UPPER_ARM_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glUniform4fv(draw_color, 1, color_upper_arm);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, color_upper_arm);

	model_view = mvstack.top();
	mvstack.pop();
}

void right_lower_arm()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH,
			LOWER_ARM_HEIGHT,
			LOWER_ARM_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glUniform4fv(draw_color, 1, color_lower_arm);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(draw_color, 1, color_lower_arm);

	model_view = mvstack.top();
	mvstack.pop();
}

void left_upper_leg()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH,
			UPPER_LEG_HEIGHT,
			UPPER_LEG_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.top();
	mvstack.pop();
}

void left_lower_leg()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH,
			LOWER_LEG_HEIGHT,
			LOWER_LEG_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.top();
	mvstack.pop();
}

void right_upper_leg()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH,
			UPPER_LEG_HEIGHT,
			UPPER_LEG_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.top();
	mvstack.pop();
}

void right_lower_leg()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH,
			LOWER_LEG_HEIGHT,
			LOWER_LEG_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.top();
	mvstack.pop();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	traverse(&nodes[Torso]);
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		theta[angle] += 5.0;
		if (theta[angle] > 360.0) { theta[angle] -= 360.0; }
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		theta[angle] -= 5.0;
		if (theta[angle] < 0.0) { theta[angle] += 360.0; }
	}

	mvstack.push(model_view);

	switch (angle)
	{
	case Torso:
		nodes[Torso].transform =
			RotateY(theta[Torso]);
		break;

	case Head1: case Head2:
		nodes[Head].transform =
			Translate(0.0, TORSO_HEIGHT + 0.5*HEAD_HEIGHT, 0.0) *
			RotateX(theta[Head1]) *
			RotateY(theta[Head2]) *
			Translate(0.0, -0.5*HEAD_HEIGHT, 0.0);
		break;

	case LeftUpperArm:
		nodes[LeftUpperArm].transform =
			Translate(-(TORSO_WIDTH + UPPER_ARM_WIDTH),
				0.9*TORSO_HEIGHT, 0.0) *
			RotateX(theta[LeftUpperArm]);
		break;

	case RightUpperArm:
		nodes[RightUpperArm].transform =
			Translate(TORSO_WIDTH + UPPER_ARM_WIDTH, 0.9*TORSO_HEIGHT, 0.0) *
			RotateX(theta[RightUpperArm]);
		break;

	case RightUpperLeg:
		nodes[RightUpperLeg].transform =
			Translate(TORSO_WIDTH + UPPER_LEG_WIDTH,
				0.1*UPPER_LEG_HEIGHT, 0.0) *
			RotateX(theta[RightUpperLeg]);
		break;

	case LeftUpperLeg:
		nodes[LeftUpperLeg].transform =
			Translate(-(TORSO_WIDTH + UPPER_LEG_WIDTH),
				0.1*UPPER_LEG_HEIGHT, 0.0) *
			RotateX(theta[LeftUpperLeg]);
		break;

	case LeftLowerArm:
		nodes[LeftLowerArm].transform =
			Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *
			RotateX(theta[LeftLowerArm]);
		break;

	case LeftLowerLeg:
		nodes[LeftLowerLeg].transform =
			Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *
			RotateX(theta[LeftLowerLeg]);
		break;

	case RightLowerLeg:
		nodes[RightLowerLeg].transform =
			Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *
			RotateX(theta[RightLowerLeg]);
		break;

	case RightLowerArm:
		nodes[RightLowerArm].transform =
			Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *
			RotateX(theta[RightLowerArm]);
		break;
	}

	model_view = mvstack.top();
	mvstack.pop();
	glutPostRedisplay();
}

void menu(int option)
{
	if (option == Quit)
	{
		exit(EXIT_SUCCESS);
	}

	angle = option;
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	GLfloat left = -10.0, right = 10.0;
	GLfloat bottom = -10.0, top = 10.0;
	GLfloat zNear = -10.0, zFar = 10.0;

	GLfloat aspect = GLfloat(width) / height;

	if (aspect > 1.0)
	{
		left *= aspect;
		right *= aspect;
	}
	else
	{
		bottom /= aspect;
		top /= aspect;
	}

	mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	model_view = mat4(1.0);   // An Identity matrix
}

void initNodes(void)
{
	mat4 m;

	m = RotateY(theta[Torso]);
	nodes[Torso] = Node(m, torso, NULL, &nodes[Head1]);

	m = Translate(0.0, TORSO_HEIGHT, 0.0) *
		RotateX(theta[Head1]) *
		RotateY(theta[Head2]);
	nodes[Head1] = Node(m, head, &nodes[LeftUpperArm], NULL);

	m = Translate(-0.5 *(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9*TORSO_HEIGHT, 0.0) *
		RotateX(theta[LeftUpperArm]);
	nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

	m = Translate(0.5 *(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9*TORSO_HEIGHT, 0.0) *
		RotateX(theta[RightUpperArm]);
	nodes[RightUpperArm] = Node(m, right_upper_arm,	&nodes[LeftUpperLeg], &nodes[RightLowerArm]);

	m = Translate(-0.5 *(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.1*UPPER_LEG_HEIGHT, 0.0) *
		RotateX(theta[LeftUpperLeg]);
	nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

	m = Translate(0.5 *(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.1*UPPER_LEG_HEIGHT, 0.0) *
		RotateX(theta[RightUpperLeg]);
	nodes[RightUpperLeg] = Node(m, right_upper_leg, NULL, &nodes[RightLowerLeg]);

	m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateX(theta[LeftLowerArm]);
	nodes[LeftLowerArm] = Node(m, left_lower_arm, NULL, NULL);

	m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateX(theta[RightLowerArm]);
	nodes[RightLowerArm] = Node(m, right_lower_arm, NULL, NULL);

	m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateX(theta[LeftLowerLeg]);
	nodes[LeftLowerLeg] = Node(m, left_lower_leg, NULL, NULL);

	m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateX(theta[RightLowerLeg]);
	nodes[RightLowerLeg] = Node(m, right_lower_leg, NULL, NULL);
}

void init(void)
{
	colorcube();

	// Initialize tree
	initNodes();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
		colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader82.glsl", "fshader82.glsl");
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(points));

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	draw_color = glGetUniformLocation(program, "draw_color");

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("robot");
	glewExperimental = GL_TRUE;

	glewInit();

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutCreateMenu(menu);
	glutAddMenuEntry("torso", Torso);
	glutAddMenuEntry("head1", Head1);
	glutAddMenuEntry("head2", Head2);
	glutAddMenuEntry("right_upper_arm", RightUpperArm);
	glutAddMenuEntry("right_lower_arm", RightLowerArm);
	glutAddMenuEntry("left_upper_arm", LeftUpperArm);
	glutAddMenuEntry("left_lower_arm", LeftLowerArm);
	glutAddMenuEntry("right_upper_leg", RightUpperLeg);
	glutAddMenuEntry("right_lower_leg", RightLowerLeg);
	glutAddMenuEntry("left_upper_leg", LeftUpperLeg);
	glutAddMenuEntry("left_lower_leg", LeftLowerLeg);
	glutAddMenuEntry("quit", Quit);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	glutMainLoop();
	return 0;
}