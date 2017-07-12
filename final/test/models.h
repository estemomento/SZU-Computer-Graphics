#include <string>
#include <algorithm>
#include <stack>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tools/shader.h>
#include <tools/camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <Angel.h>
using namespace std;
GLuint screenWidth = 800, screenHeight = 600;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

#define  ROBOT_SCALE 0.1

#define TORSO_HEIGHT (3.0 * ROBOT_SCALE)
#define TORSO_WIDTH (3.0 *ROBOT_SCALE)
#define UPPER_ARM_HEIGHT (2.0 *ROBOT_SCALE)
#define LOWER_ARM_HEIGHT (1.0 *ROBOT_SCALE)
#define UPPER_LEG_WIDTH  (0.5 *ROBOT_SCALE)
#define LOWER_LEG_WIDTH  (0.5 *ROBOT_SCALE)
#define LOWER_LEG_HEIGHT (1.0 *ROBOT_SCALE)
#define UPPER_LEG_HEIGHT (2.0 *ROBOT_SCALE)
#define UPPER_LEG_WIDTH  (0.5 *ROBOT_SCALE)
#define UPPER_ARM_WIDTH  (0.5 *ROBOT_SCALE)
#define LOWER_ARM_WIDTH  (0.5 *ROBOT_SCALE)
#define HEAD_HEIGHT (1.5 *ROBOT_SCALE)
#define HEAD_WIDTH (1.0*ROBOT_SCALE)

#define DICK_HEIGHT (0.7* UPPER_LEG_HEIGHT)

GLuint loadTexture(GLchar const * path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

GLuint loadCubemap(std::vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

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
glm::vec4 color_torso = glm::vec4(0, 0, 1, 1);
glm::vec4 color_head = glm::vec4(0, 1, 0, 1);
glm::vec4 color_upper_arm = glm::vec4(1, 1, 0, 1);
glm::vec4 color_lower_arm = glm::vec4(1, 0, 0, 1);

glm::vec4 color_upper_leg = glm::vec4(1, 1, 1, 1);
glm::vec4 color_lower_leg = glm::vec4(0, 0, 0, 1);

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

class Robot
{
	GLuint VBO, VAO;
	Shader shader;
	GLuint texture1, texture2;
	glm::vec3 move{0};
	std::stack<glm::mat4> mstack;
	glm::mat4 model{ 1 };

public:
	Robot() :shader(Shader("mainshader.vs", "mainshader.frag"))
	{
		init();
	}
	void rotate(int part)
	{
		theta[part] += 0.5;
	}
	void moving(char direction){
		if (direction == 'x') move.x += 0.01;
		if (direction == 'y') move.y += 0.01;
		if (direction == 'z') move.z += 0.01;
		if (direction == 'X') move.x -= 0.01;
		if (direction == 'Y') move.y -= 0.01;
		if (direction == 'Z') move.z -= 0.01;
	}
	void draw(Camera & camera)
	{
		shader.Use();

		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)800 / (float)600, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, texture2);
		mstack.push(model);
		model = RotateY(theta[Torso]);
		model = model * Translate(move.x, move.y + LOWER_LEG_HEIGHT + UPPER_LEG_HEIGHT + 0.5*TORSO_HEIGHT - 0.5, move.z);
		// torso
		draw_part(color_torso, TORSO_WIDTH, TORSO_HEIGHT);
		{
			glBindTexture(GL_TEXTURE_2D, texture1);	
			mstack.push(model);
			// head 
			model = RotateY(theta[Head]) * Translate(0, 0.5*(HEAD_WIDTH + TORSO_HEIGHT), 0) *model;
			draw_part(color_head, HEAD_WIDTH, HEAD_WIDTH);
			model = mstack.top();
			mstack.pop();

		// back to torso
			glBindTexture(GL_TEXTURE_2D, texture2);
			mstack.push(model);
			// right upper arm
			model = RotateX(theta[RightUpperArm]) * Translate(0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0, 0) *model;
			draw_part(color_upper_arm, UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT);
			// right lower arm
			model = Translate(0, -0.5*(UPPER_ARM_HEIGHT + LOWER_ARM_HEIGHT), 0) *model;
			draw_part(color_lower_arm, LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT);
			model = mstack.top();
			mstack.pop();

		// back to torso	
			mstack.push(model);
			// left upper arm
			model = RotateX(theta[LeftUpperArm]) * Translate(-0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0, 0)* model;
			draw_part(color_upper_arm, UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT);
			// left lower arm
			model = Translate(0, -0.5*(UPPER_ARM_HEIGHT + LOWER_ARM_HEIGHT), 0) * model;
			draw_part(color_lower_arm, LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT);
			model = mstack.top();
			mstack.pop();

		// back to torso
			mstack.push(model);
			// right upper leg
			model = Translate(0.4*TORSO_WIDTH, -0.5*(TORSO_HEIGHT + UPPER_LEG_HEIGHT), 0)* model;
			draw_part(color_upper_leg, UPPER_LEG_WIDTH, UPPER_ARM_HEIGHT);
			// right lower leg
			model = RotateX(theta[RightLowerLeg]) * Translate(0, -0.5*(UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT), 0) *model;
			draw_part(color_lower_leg, LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT);
			model = mstack.top();
			mstack.pop();
		// back to torso
			mstack.push(model);
			// left upper leg
			model = Translate(-0.4*TORSO_WIDTH, -0.5*(TORSO_HEIGHT + UPPER_LEG_HEIGHT), 0)* model;
			draw_part(color_upper_leg, UPPER_LEG_WIDTH, UPPER_ARM_HEIGHT);
			// left lower leg
			model = RotateX(theta[LeftLowerLeg]) * Translate(0, -0.5*(UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT), 0)*model;
			draw_part(color_lower_leg, LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT);
			model = mstack.top();
			mstack.pop();
		}
		model = mstack.top();
		mstack.pop();
		glBindVertexArray(0);
	}

private:
	void draw_part(const glm::vec4 & color, GLfloat x, GLfloat y)
	{
		glm::mat4 instance = Scale(x, y, x);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_TRUE, glm::value_ptr(instance * model));
		glUniform4fv(glGetUniformLocation(shader.Program, "inColor"), 1, glm::value_ptr(color));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	void init()
	{
		GLfloat vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// Bind our Vertex Array Object first, then bind and set our buffers and pointers.
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0); // Unbind VAO

		texture1 = loadTexture("resources/textures/face.png");
		texture2 = loadTexture("resources/textures/metal.png");

	}
};

class Bricks
{
	GLuint VBO, VAO;
	Shader shader;
	GLuint texture1, texture2;
	glm::vec3 cubePositions[10] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};
public:
	Bricks() :shader(Shader("mainshader.vs", "mainshader.frag"))
	{
		init();
	}

	void moving(int x)
	{
		for (int i = 0; i < 10; ++i) x ? cubePositions[i].z -= 0.01f: cubePositions[i].z += 0.01f;
	}

	void draw(Camera &camera)
	{
		shader.Use();

		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture2"), 1);

		// Create camera transformation
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
		GLint viewLoc = glGetUniformLocation(shader.Program, "view");
		GLint projLoc = glGetUniformLocation(shader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		for (GLuint i = 0; i < 10; i++)
		{
			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
	}
	void init()
	{
		texture1 = loadTexture("resources/textures/marble.jpg");
		texture2 = loadTexture("resources/textures/bricks2.jpg");
		GLfloat vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// Bind our Vertex Array Object first, then bind and set our buffers and pointers.
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0); // Unbind VAO
	}
};

class Skybox
{
	GLuint VBO, VAO;
	Shader shader;
	GLuint texture;
public:
	Skybox():shader(Shader("skybox.vs", "skybox.frag"))
	{
		init();
	}
	void draw(Camera & camera)
	{
		glm::mat4 model;
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		shader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void init()
	{
		std::vector<std::string> faces;
		faces.push_back("resources/textures/skybox/alpha-island_rt.jpg");
		faces.push_back("resources/textures/skybox/alpha-island_lf.jpg");
		faces.push_back("resources/textures/skybox/alpha-island_up.jpg");
		faces.push_back("resources/textures/skybox/alpha-island_dn.jpg");
		faces.push_back("resources/textures/skybox/alpha-island_bk.jpg");
		faces.push_back("resources/textures/skybox/alpha-island_ft.jpg");
		texture = loadCubemap(faces);

		GLfloat skyboxVertices[] = {
			// Positions          
			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,

			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}
};