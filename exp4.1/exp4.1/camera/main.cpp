#include "include/Angel.h"
#include "include/TriMesh.h"

#include <cstdlib>
#include <iostream>

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint modelViewMatrixID;
GLuint projMatrixID;

TriMesh* mesh = new TriMesh();

// camera view
float rad = 4.0;
float tAngle = 0.0;
float pAngle = 0.0;

// perspective projection
float fov = 45.0;
float aspect = 1.0;
float zN = 0.1;
float zF = 100.0;

// orthogonal projection
float scale = 1.5;

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		mat4 c;
		c[0][0] = 2.0 / (right - left);
		c[1][1] = 2.0 / (top - bottom);
		c[2][2] = 2.0 / (zNear - zFar);
		c[3][3] = 1.0;
		c[0][3] = -(right + left) / (right - left);
		c[1][3] = -(top + bottom) / (top - bottom);
		c[2][3] = -(zFar + zNear) / (zFar - zNear);
		return c;
	}

	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		GLfloat top = tan(fovy*DegreesToRadians / 2) * zNear;
		GLfloat right = top * aspect;

		mat4 c;
		c[0][0] = zNear / right;
		c[1][1] = zNear / top;
		c[2][2] = -(zFar + zNear) / (zFar - zNear);
		c[2][3] = (-2.0*zFar*zNear) / (zFar - zNear);
		c[3][2] = -1.0;
		c[3][3] = 0.0;
		return c;
	}

	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		vec4 n = normalize(at - eye);
		vec4 u = normalize(vec4(cross(n, up), 0.0));
		vec4 v = normalize(vec4(cross(u, n), 0.0));

		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 c = mat4(u, v, -n, t);
		return c * Translate(-eye);
	}
}

// OpenGL initialization
void init()
{
	// Clear Color Buffer
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	programID = InitShader("vshader.glsl", "fshader.glsl");
	vPositionID = glGetAttribLocation(programID, "vPosition");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");

	// Read a cube model
	mesh->read_off("cube.off");

	vector<vec3f> vs = mesh->v();
	vector<vec3i> fs = mesh->f();

	// Vertex Array Object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Vertex Buffer Object
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// VBO indexing
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

// Rendering
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	
	float z = rad * cos(tAngle * DegreesToRadians) * cos(pAngle * DegreesToRadians);
	float x = rad * cos(tAngle * DegreesToRadians) * sin(pAngle * DegreesToRadians);
	float y = rad * sin(tAngle * DegreesToRadians);

	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = Camera::lookAt(vec4(x, y, z, 1), vec4(0, 0, 0, 1), vec4(0, 1, 0, 0.0));
	Camera::projMatrix = Camera::perspective(fov, aspect, zN, zF);
	//Camera::projMatrix = Camera::ortho(-scale, scale, -scale, scale, 0.1, 100.0);

	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;

	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

// Reshape window
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

// Control mouse actions
void mouse(int button, int state, int x, int y)
{
	return;
}

// Standard key presses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		exit(EXIT_SUCCESS);
		break;
		case 'q':
		exit (EXIT_SUCCESS);
		break;

		case 'x': pAngle += 5.0; break;
		case 'X': pAngle -= 5.0; break;

		case 'y': tAngle += 5.0; break;
		case 'Y': tAngle -= 5.0; break;

		case 'z': rad += 0.1; break;
		case 'Z': rad -= 0.1; break;

		case 'f': fov += 5.0; break;
		case 'F': fov -= 5.0; break;

		case 'a': aspect += 0.1; break;
		case 'A': aspect -= 0.1; break;
		
		case 'n': zN += 0.1; break;
		case 'N': zN -= 0.1; break;

		case 'm': zF += 0.1; break;
		case 'M': zF -= 0.1; break;

		case 'o': scale += 0.1; break;
		case 'O': scale -= 0.1; break;

		case ' ': 
			rad = 4.0; tAngle = 0.0; pAngle = 0.0; 
			fov = 45.0; aspect = 1.0; zN = 0.1; zF = 100.0;
			break;

		break;
	}
	glutPostRedisplay();
}

void idle(void)
{
	glutPostRedisplay();
}

void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);

	if (mesh) {
		delete mesh;
		mesh = NULL;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL-Tutorial");

	glewInit();
	init();

	// Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}