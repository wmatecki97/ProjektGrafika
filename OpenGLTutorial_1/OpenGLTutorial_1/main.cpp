#define _CRT_SECURE_NO_DEPRECATE

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "stdio.h"
#include <iostream>
#include <stdlib.h>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objectsCoords.h"

// classes
#include "Shader.h"
#include "Camera.h"


// functions inits
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


/* 
SETTINGS
	screen param
	camera class and camera movement param
	timing
*/
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
// lighting
glm::vec3 lightPos(-1.0f, 7.0f, -1.0f);


/*
GLFW
	initialize
	configure
*/
void initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


/*
WINDOW
	join functions/callbacks to window:
		1. framebuffer_size_callback -- on every window resizing
		2. mouse_callback -- on every mouse moving
		3. scroll_callback -- one every scrolling
		-- it doesn't work without joining
	tell GLFW to capture mouse
*/
void windowObjectConfigure(GLFWwindow* window) {
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

/*
GLAD
	manages function pointers for OpenGL,
	so initialize GLAD before calling any OpenGL function (but not GLFW!)
*/
int initGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
}


/*
TEXTURE
	load and create a texture	
*/
unsigned int loadTexture(const char * textureName) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(textureName, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

/*
generate cubes
	choose position on Z-axis, X-axis on area
	take random number as height of the building (Y-axis)
	create cube and pass it on to the vector of vec3
*/
void generateCity( std::vector<glm::vec4>* cubePositions, int sizeOfCity){
	for (int k = 0; k < sizeOfCity; k++) { // z
		if (k % 2 == 0) {
			for (int j = 0; j < sizeOfCity; j++) {  // x
				if (j % 2 == 0) {
					int buildingHeight = std::rand() % 4 + 2; // <2,5>
					float texture = 0.0f;
						if (buildingHeight % 3 == 1) {
							texture = 1.0f;
						}
						else if (buildingHeight % 3 == 2) {
							texture = 2.0f;
						}
						else {
							texture = 0.0f;
						}
					for (int i = 0; i < buildingHeight; i++) {  // y
						cubePositions->push_back(glm::vec4((float)j, (float)i, (float)k, texture));
					}
				}
			}
		}
	}
}


/*
generate flat street crossings
	choose position on Z-axis, X-axis on area
	Y-axis always 0 (ground level)
	create square and pass it on to the vector of vec3
*/
void generateCrossings(std::vector<glm::vec3>* streetPositions, int sizeOfCity) {
	for (int k = 0; k < sizeOfCity; k++) { // z
		if (k % 2 != 0) {
			for (int j = 0; j < sizeOfCity; j++) {  // x
				if (j % 2 != 0) {
						streetPositions->push_back(glm::vec3((float)j, 0.0f, (float)k));
				}
			}
		}
	}
}


/*
generate flat street 
choose position on Z-axis, X-axis on area
Y-axis always 0 (ground level)
create square and pass it on to the vector of vec3
*/
void generateStreet(std::vector<glm::vec3>* streetPositions, int sizeOfCity) {
	for (int k = 0; k < sizeOfCity; k++) { // z
		if (k % 2 == 0) {
			for (int j = 0; j < sizeOfCity; j++) {  // x
				if (j % 2 != 0) {
					streetPositions->push_back(glm::vec3((float)j, 0.0f, (float)k));
				}
			}
		}
	}
}


/*
generate flat street
choose position on Z-axis, X-axis on area
Y-axis always 0 (ground level)
create square and pass it on to the vector of vec3
*/
void generateStreet2(std::vector<glm::vec3>* streetPositions, int sizeOfCity) {
	for (int k = 0; k < sizeOfCity; k++) { // z
		if (k % 2 != 0) {
			for (int j = 0; j < sizeOfCity; j++) {  // x
				if (j % 2 == 0) {
					streetPositions->push_back(glm::vec3((float)j, 0.0f, (float)k));
				}
			}
		}
	}
}


/*
VBO:	create / bind and select type / configure
VAO:	create / bind (connects to VBO)
EBO:	create / bind (connects to VAO and VBO) / configure (for using indices)
*/
void configureVAO_VBO_EBO(unsigned int * VAO, unsigned int * VBO, unsigned int * EBO) {
	// always add '*' before each parameter
	glGenVertexArrays(1, *&VAO);  // xd
	glGenBuffers(1, *&VBO);
	//glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize3, verticesTab3, GL_STATIC_DRAW);
	glBindVertexArray(*VAO);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

/*
	VAO - vertex array object creation
*/
void configureAnotherVAO(unsigned int * VAO, const void * verticesTab, size_t verticesSize) {
	// always add '*' before each parameter
	glGenVertexArrays(1, *&VAO);  // xd
	glBindVertexArray(*VAO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, verticesTab, GL_STATIC_DRAW);
}


/*
	clear and delete VAO VBO EBO
*/
void cleanVAO_VBO_EBO(unsigned int * VAO, unsigned int * VBO, unsigned int * EBO) {
	glDeleteVertexArrays(1, *&VAO);
	glDeleteBuffers(1, *&VBO);
	//glDeleteBuffers(1, &EBO);
}


/*
set how OpenGL should interpret objectCoords.cpp data
*/
void howInterpretVertexData(GLuint sizeOfRow, GLuint vertexCoordsNumber,
							GLuint normalCoordsNumber, GLuint textureCoordsNumber) {
	// position attribute:
	// 0 - attribute location, 3 - number of vertex coords, 5* - size of row
	//glVertexAttribPointer(0, vertexCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)0);
	glVertexAttribPointer(0, vertexCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	// 1 - attribute location, 3 - number of vertex coords, 8* - size of row
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	// texture coord attribute
	// 1 - attribute location, 2 - number of vertex coords, 5* - size of row, 3* - offset
	//glVertexAttribPointer(1, textureCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(1, textureCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}



/*
set how OpenGL should interpret objectCoords.cpp data
*/
void howInterpretVertexData(GLuint sizeOfRow, GLuint vertexCoordsNumber,
	GLuint normalCoordsNumber, GLuint textureCoordsNumber,
	GLuint vertexOffset, GLuint normalOffset, GLuint textureOffset) {
	// position attribute:
	// 0 - attribute location, 3 - number of vertex coords, 5* - size of row
	//glVertexAttribPointer(0, vertexCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)0);
	glVertexAttribPointer(0, vertexCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)vertexOffset);
	glEnableVertexAttribArray(0);

	// normal attribute
	// 1 - attribute location, 3 - number of vertex coords, 8* - size of row
	glVertexAttribPointer(1, normalCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)(normalOffset * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	// 1 - attribute location, 2 - number of vertex coords, 5* - size of row, 3* - offset
	//glVertexAttribPointer(1, textureCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, textureCoordsNumber, GL_FLOAT, GL_FALSE, sizeOfRow * sizeof(float), (void*)(textureOffset * sizeof(float)));
	glEnableVertexAttribArray(2);
}

/*
_________________________________________________________
_________________________________________________________
_________________________________________________________
*/
int main() {

	// init GLFW lib
	initGLFW();

	// make the context of our window the main context on the current thread 
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "project_1", NULL, NULL);
	if (window == NULL) { 		
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	// init window functions
	windowObjectConfigure(window);

	// init GLAD lib
	initGLAD();

	// enable z-buffer
	glEnable(GL_DEPTH_TEST); 

	// create shader object
	Shader ourShader("vertexshader.vs", "fragmentshader.fs"); 
	Shader lampShader("lamp.vs", "lamp.fs");
	Shader lightingShader("lighting_maps.vs", "lighting_maps.fs");

	// generate city
	srand(time(NULL));
	std::vector <glm::vec4> cubePositions;  // !!!
	std::vector <glm::vec3> crossingPositions;
	std::vector <glm::vec3> streetPositions;
	std::vector <glm::vec3> street2Positions;
	int sizeOfCity = 47;

	generateCity(&cubePositions, sizeOfCity);
	generateCrossings(&crossingPositions, sizeOfCity);
	generateStreet(&streetPositions, sizeOfCity);
	generateStreet2(&street2Positions, sizeOfCity);

	// VAO VBO EBO
	unsigned int VBO, VAO, VAO2, VAO3, VAO4, EBO;
	configureVAO_VBO_EBO(&VAO, &VBO, &EBO);
	howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
	//howInterpretVertexData(5, 3, 0, 2);

	// next set of objects - crossing, street
	configureAnotherVAO(&VAO2, verticesTab2, verticesSize2);
	howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
	configureAnotherVAO(&VAO3, verticesTab2, verticesSize2);
	howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
	configureAnotherVAO(&VAO4, verticesTab2, verticesSize2);
	howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);


	// light cube
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// load texture
	unsigned int texture, textureCrossing, textureStreet, textureStreet2, textureBrick, textureStones;
	texture = loadTexture("textures/skyscaper2.jpg");
	textureCrossing = loadTexture("textures/crossing1.jpg");
	textureStreet = loadTexture("textures/street1.jpg");
	textureStreet2 = loadTexture("textures/street2.jpg");
	textureBrick = loadTexture("textures/bricks.jpg");
	textureStones = loadTexture("textures/stones.jpg");

	// join textures
	ourShader.setInt("texture", 0);
	ourShader.setInt("textureCrossing", 1);
	ourShader.setInt("textureStreet", 2);
	ourShader.setInt("textureStreet2", 3);
	ourShader.setInt("textureBrick", 4);
	ourShader.setInt("textureStones", 4);


	unsigned int diffuseMap = loadTexture("textures/bricks.jpg");
	unsigned int specularMap = loadTexture("textures/woodspec.png");
	// shader configuration
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	// activate Shader Program
	//ourShader.use();
	

/* 
RENDER LOOP
=================== TO DO
(if window shouldn't be closed do):
	process input - keyboard
	render background
	use shader program
	bind VAO
	draw elements

	swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
*/
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear COLOR BUFFER | Z-BUFFER
		//ourShader.use();

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("material.shininess", 32.0f);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//ourShader.setMat4("projection", projection);
		lightingShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		//ourShader.setMat4("view", view);
		lightingShader.setMat4("view", view);

		// BUILDINGS - first group of object
		configureVAO_VBO_EBO(&VAO, &VBO, &EBO);
		howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
		//howInterpretVertexData(5, 3, 0, 2);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glBindVertexArray(VAO);

		for (unsigned int i = 0; i < cubePositions.size(); i++)		{
			// calculate the model matrix for each object and pass it to shader before drawing
			if (cubePositions[i].w == 1.0f) {
				glm::mat4 model;
				model = glm::translate(model, glm::vec3(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
				lightingShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			else if (cubePositions[i].w == 2.0f){
				glm::mat4 model;
				model = glm::translate(model, glm::vec3(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
				lightingShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureStones);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			else {
				glm::mat4 model;
				model = glm::translate(model, glm::vec3(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
				lightingShader.setMat4("model", model);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			//glm::mat4 model;
			//model = glm::translate(model, glm::vec3(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z));
			//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
			//ourShader.setMat4("model", model);
			//lightingShader.setMat4("model", model);
			//glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		

		// CROSSINGS - second group of object
		configureAnotherVAO(&VAO2, verticesTab2, verticesSize2);
		howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCrossing);
		glBindVertexArray(VAO2);
		for (unsigned int i = 0; i < crossingPositions.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, crossingPositions[i]);
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// STREETS 1 - 3rd group of object
		configureAnotherVAO(&VAO3, verticesTab2, verticesSize2);
		howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreet);
		glBindVertexArray(VAO3);
		for (unsigned int i = 0; i < streetPositions.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, streetPositions[i]);
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.5f, 0.0f, 0.0f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// STREETS 2 - 4th group of object
		configureAnotherVAO(&VAO4, verticesTab2, verticesSize2);
		howInterpretVertexData(8, 3, 3, 2, 0, 3, 6);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureStreet2);
		glBindVertexArray(VAO4);
		for (unsigned int i = 0; i < street2Positions.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, street2Positions[i]);
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.5f, 0.0f, 0.0f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		

		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents(); 

		cleanVAO_VBO_EBO(&VAO, &VBO, &EBO);
		glDeleteVertexArrays(1, &VAO2);
		glDeleteVertexArrays(1, &VAO3);
		glDeleteVertexArrays(1, &VAO4);
		glDeleteVertexArrays(1, &lightVAO);
	}

	// clear and delete
	cleanVAO_VBO_EBO(&VAO, &VBO, &EBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteVertexArrays(1, &VAO4);
	glDeleteVertexArrays(1, &lightVAO);
	glfwTerminate();
		
	return 0;
}


//  process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  // 'Esc' key
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


// whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}


