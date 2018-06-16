/*
learnopengl.com Hello Triangle
to draw using triangles:
	add vertex to vertex array
	add index to index array
	change number 9 in glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0) in render loop to number of vertices
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// functions inits
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// end settings

/*
VERTEX SHADER SOURCE CODE
simple vertex shader
*/
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

/*
FRAGMENT SHADER SOURCE CODE
simple fragment shader
*/
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


int main()
{

	/*
	GLFW
	initialize
	configure
	*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*
	create a window object
	*/
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "project_1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/*
	make the context of our window the main context on the current thread /
	call framebuffer_size_callback on every window resize
	*/
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*
	GLAD
	manages function pointers for OpenGL,
	so initialize GLAD before calling any OpenGL function (but not GLFW!)
	*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*
	VERTEX SHADER
	vertex_creating:	initialize / link to *vertexShaderSource code / build and compile
	compiling:			check if processed correctly / if not - print error
	*/
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*
	FRAGMENT SHADER
	fragment_creating:	initialize / link to *fragmentShaderSource code / build and compile
	compiling:			check if processed correctly / if not - print error
	*/
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*
	SHADER PROGRAM
	linking:	create shaderProgram / link verex / link fragment / link all together
	errors:		check for errors
	deleting:	delete vertex / delete fragment
	*/
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/*
	set triangle objects
	*/
	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f,   // top left 
		1.0f,  1.0f, 0.0f   // top left 
	};
	unsigned int indices[] = {
		0, 1, 3,	// first triangle
		1, 2, 3,    // second triangle
		0, 1, 4		// tird triangle
	};

	/*
	VBO:	create / bind and select type / configure
	VAO:	create / bind (connects to VBO)
	EBO:	create / bind (connects to VAO and VBO) / configure
	*/
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/*
	set how OpenGL should interpret vertex data
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 0 == location
								  //glBindBuffer(GL_ARRAY_BUFFER, 0);
								  //glBindVertexArray(0);

								  /*
								  RENDER LOOP
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
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*
	clear and delete
	*/
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();

	return 0;
}


/*
process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
*/
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  // 'Esc' key
		glfwSetWindowShouldClose(window, true);
}

/*
whenever the window size changed (by OS or user resize) this callback function executes
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


