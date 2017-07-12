#include "models.h"
using namespace std;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Keyboard(Robot& ro, Bricks& br);

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "", nullptr, nullptr); 
	glfwSetWindowTitle(window, u8"期末大作业"); // Prevent text encoding bugs
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Bricks br;
	Robot ro;
	Skybox sk;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Keyboard(ro, br);

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Draws
		glDepthFunc(GL_LEQUAL); 
		sk.draw(camera);
		glDepthFunc(GL_LESS);
		br.draw(camera);
		glActiveTexture(GL_TEXTURE0);
		ro.draw(camera);
		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose


	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void Keyboard(Robot& ro, Bricks& br)
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_L])
	{
		ro.rotate(RightUpperArm);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_I])
	{
		ro.rotate(Head);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_K])
	{
		ro.rotate(Torso);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_J])
	{
		ro.rotate(LeftUpperArm);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_M])
	{
		ro.rotate(LeftLowerLeg);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_COMMA])
	{
		ro.rotate(RightLowerLeg);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_F])
	{
		ro.moving('x');
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_G])
	{
		ro.moving('y');
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_H])
	{
		ro.moving('z');
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_V])
	{
		ro.moving('X');
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_B])
	{
		ro.moving('Y');
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_N])
	{
		ro.moving('Z');
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_T])
	{
		br.moving(0);
		ro.draw(camera);
	}
	if (keys[GLFW_KEY_Y])
	{
		br.moving(1);
		ro.draw(camera);
	}
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}