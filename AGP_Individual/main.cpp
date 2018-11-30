// Std. Includes
#include <string>

// GLEW

#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "skyboxTexture.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2/SOIL2.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

//Light
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLuint initQuadVAO()
{
	// Configure VAO/VBO
	GLfloat vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	return quadVAO;
}


int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders
	Shader skyboxShader("skybox.vs", "skybox.frag");
	Shader modelShader("modelShader.vs", "modelShader.frag");
	Shader greyscaleFilter("greyscale-fbo.vert", "greyscale-fbo.frag");
	Shader shader("res/shaders/modelLoading.vs", "res/shaders/modelLoading.frag");

	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glm::vec3 pointLightPos[] = {
		glm::vec3(0.0f, 0.0f, 2.0f),
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glBindVertexArray(0);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// fbo
	unsigned int fbo;
	glGenFramebuffers(1, &fbo); //Creates the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);//binds the fbo as the active framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) //checks the status of the framebuffer to see if we have completed the requirments.
		std::cout << "framebuffer not complete" << std::endl;

	// creates the texture for the framebuffer
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//// attaches the texture to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// quad for second pass texture

	GLuint quadVAO = initQuadVAO();

	// Cubemap (Skybox)
	vector<const GLchar*> faces;
	faces.push_back("skybox/rt.tga");
	faces.push_back("skybox/lf.tga");
	faces.push_back("skybox/up2.tga");
	faces.push_back("skybox/dn.tga");
	faces.push_back("skybox/bk.tga");
	faces.push_back("skybox/ft.tga");
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	

	// Load models
	Model ourModel("res/models/nanosuit.obj", false);

	//Loads ground plain
	Model ourGroundPlain("res/models/cube.obj", true);

	// Draw in wireframe
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	modelShader.Use();
	glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

	int counter(0);
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		if (counter == 0)
		{
			//// first pass
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			// Set frame time
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Check and call events
			glfwPollEvents();
			DoMovement();

			// Clear the colorbuffer
			glClearColor(0.05f, 1.05f, 0.05f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader.Use();

			glm::mat4 view = camera.GetViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));



			// Draw skybox as last
			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader.Use();
			glm::mat4 view_sky = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix

			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view_sky));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			// skybox cube
			glBindVertexArray(skyboxVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // Set depth function back to default




			modelShader.Use();
			// Draw the loaded model
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// It's a bit too big for our scene, so scale it down
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ourModel.Draw(modelShader);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// Draw the loaded ground plain
			glm::mat4 groundPlain;
			groundPlain = glm::translate(groundPlain, glm::vec3(0.0f, -1.75f, -1.0f)); // Translate it down a bit so it's at the center of the scene
			groundPlain = glm::scale(groundPlain, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(groundPlain));
			ourGroundPlain.Draw(modelShader);








			GLint viewPosLoc = glGetUniformLocation(modelShader.Program, "viewPos");
			glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
			glUniform1f(glGetUniformLocation(modelShader.Program, "material.shininess"), 32.0f);

			//Setting uniforms for types of light
			//Room For more efficiency

			// Directional Light
			glUniform3f(glGetUniformLocation(modelShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

			// Point Light
			glUniform3f(glGetUniformLocation(modelShader.Program, "pointLights[0].position"), pointLightPos[0].x, pointLightPos[0].y, pointLightPos[0].z);
			glUniform3f(glGetUniformLocation(modelShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "pointLights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "pointLights[0].linear"), 0.09f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "pointLights[0].quadratic"), 0.032f);

			// Spot Light
			glUniform3f(glGetUniformLocation(modelShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
			glUniform3f(glGetUniformLocation(modelShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
			glUniform3f(glGetUniformLocation(modelShader.Program, "spotLight.ambient"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "spotLight.diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(modelShader.Program, "spotLight.specular"), 0.8f, 0.8f, 0.8f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "spotLight.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "spotLight.linear"), 0.09f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "spotLight.quadratic"), 0.032f);
			glUniform1f(glGetUniformLocation(modelShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
			glUniform1f(glGetUniformLocation(modelShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));


			GLint modelLoc = glGetUniformLocation(modelShader.Program, "model");
			GLint viewLoc = glGetUniformLocation(modelShader.Program, "view");
			GLint projLoc = glGetUniformLocation(modelShader.Program, "projection");

			//Pass the matrices to the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));



			

			counter++;

		}
		else
		{
			// second pass

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			greyscaleFilter.Use();

			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			GLuint glu_loc = glGetUniformLocation(greyscaleFilter.Program, "screenTexture");
			glUniform1f(glu_loc, 2);

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDeleteVertexArrays(1, &lightVAO);
			glDeleteBuffers(1, &VBO);
			counter = 0;
			// Swap the buffers
			glfwSwapBuffers(window);
		}

	}



	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
