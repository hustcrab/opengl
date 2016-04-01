

#include "stdafx.h"
#pragma warning(disable: 4996)
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GlFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){

	// 创造着色器
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// 从指定文件里面读取顶点着色器的内容
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;



	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// 链接程序
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
int main(void)
{

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	window = glfwCreateWindow(1024, 768, "two cubes", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}


	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// 蓝色背景
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	//打开混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//深度缓存
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices36个顶点
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, -0.0f,
		-1.0f, -1.0f, 2.0f,
		-1.0f, 1.0f, 2.0f,
		1.0f, 1.0f, -0.0f,
		-1.0f, -1.0f, -0.0f,
		-1.0f, 1.0f, -0.0f,
		1.0f, -1.0f, 2.0f,
		-1.0f, -1.0f, -0.0f,
		1.0f, -1.0f, -0.0f,
		1.0f, 1.0f, -0.0f,
		1.0f, -1.0f, -0.0f,
		-1.0f, -1.0f, -0.0f,
		-1.0f, -1.0f, -0.0f,
		-1.0f, 1.0f, 2.0f,
		-1.0f, 1.0f, -0.0f,
		1.0f, -1.0f, 2.0f,
		-1.0f, -1.0f, 2.0f,
		-1.0f, -1.0f, -0.0f,
		-1.0f, 1.0f, 2.0f,
		-1.0f, -1.0f, 2.0f,
		1.0f, -1.0f, 2.0f,
		1.0f, 1.0f, 2.0f,
		1.0f, -1.0f, -0.0f,
		1.0f, 1.0f, -0.0f,
		1.0f, -1.0f, -0.0f,
		1.0f, 1.0f, 2.0f,
		1.0f, -1.0f, 2.0f,
		1.0f, 1.0f, 2.0f,
		1.0f, 1.0f, -0.0f,
		-1.0f, 1.0f, -0.0f,
		1.0f, 1.0f, 2.0f,
		-1.0f, 1.0f, -0.0f,
		-1.0f, 1.0f, 2.0f,
		1.0f, 1.0f, 2.0f,
		-1.0f, 1.0f, 2.0f,
		1.0f, -1.0f, 2.0f
		
	};

	// One color for each vertex. They were generated randomly.要设置alpha值
	static const GLfloat g_color_buffer_data[] = {
		0.583f, 0.771f, 0.014f,
		0.609f, 0.115f, 0.436f, 
		0.327f, 0.483f, 0.844f, 
		0.822f, 0.569f, 0.201f, 
		0.435f, 0.602f, 0.223f, 
		0.310f, 0.747f, 0.185f, 
		0.597f, 0.770f, 0.761f, 
		0.559f, 0.436f, 0.730f, 
		0.359f, 0.583f, 0.152f, 
		0.483f, 0.596f, 0.789f, 
		0.559f, 0.861f, 0.639f, 
		0.195f, 0.548f, 0.859f, 
		0.014f, 0.184f, 0.576f, 
		0.771f, 0.328f, 0.970f, 
		0.406f, 0.615f, 0.116f,
		0.676f, 0.977f, 0.133f, 
		0.971f, 0.572f, 0.833f, 
		0.140f, 0.616f, 0.489f, 
		0.997f, 0.513f, 0.064f, 
		0.945f, 0.719f, 0.592f, 
		0.543f, 0.021f, 0.978f, 
		0.279f, 0.317f, 0.505f, 
		0.167f, 0.620f, 0.077f, 
		0.347f, 0.857f, 0.137f, 
		0.055f, 0.953f, 0.042f, 
		0.714f, 0.505f, 0.345f, 
		0.783f, 0.290f, 0.734f, 
		0.722f, 0.645f, 0.174f, 
		0.302f, 0.455f, 0.848f, 
		0.225f, 0.587f, 0.040f, 
		0.517f, 0.713f, 0.338f, 
		0.053f, 0.959f, 0.120f, 
		0.393f, 0.621f, 0.362f, 
		0.673f, 0.211f, 0.457f, 
		0.820f, 0.883f, 0.371f, 
		0.982f, 0.099f, 0.879f,
		
		
	};
	/////////////////////////////////////////////////////////////////
	//第二个
	static const GLfloat g_vertex_buffer_data1[] = {
		-1.0f, -1.0f, -2.5f,
		-1.0f, -1.0f, -0.5f,
		-1.0f, 1.0f, -0.5f,
		1.0f, 1.0f, -2.5f,
		-1.0f, -1.0f, -2.5f,
		-1.0f, 1.0f, -2.5f,
		1.0f, -1.0f, -0.5f,
		-1.0f, -1.0f, -2.5f,
		1.0f, -1.0f, -2.5f,
		1.0f, 1.0f, -2.5f,
		1.0f, -1.0f, -2.5f,
		-1.0f, -1.0f, -2.5f,
		-1.0f, -1.0f, -2.5f,
		-1.0f, 1.0f, -0.5f,
		-1.0f, 1.0f, -2.5f,
		1.0f, -1.0f, -0.5f,
		-1.0f, -1.0f, -0.5f,
		-1.0f, -1.0f, -2.5f,
		-1.0f, 1.0f, -0.5f,
		-1.0f, -1.0f, -0.5f,
		1.0f, -1.0f, -0.5f,
		1.0f, 1.0f, -0.5f,
		1.0f, -1.0f, -2.5f,
		1.0f, 1.0f, -2.5f,
		1.0f, -1.0f, -2.5f,
		1.0f, 1.0f, -0.5f,
		1.0f, -1.0f, -0.5f,
		1.0f, 1.0f, -0.5f,
		1.0f, 1.0f, -2.5f,
		-1.0f, 1.0f, -2.5f,
		1.0f, 1.0f, -0.5f,
		-1.0f, 1.0f, -2.5f,
		-1.0f, 1.0f, -0.5f,
		1.0f, 1.0f, -0.5f,
		-1.0f, 1.0f, -0.5f,
		1.0f, -1.0f, -0.5f
	};
	//第二个cube顶点颜色
	static const GLfloat g_color_buffer_data1[] = {
		0.583f, 0.771f, 0.0f,0.3f,
		0.609f, 0.115f, 0.4f, 0.3f,
		0.327f, 0.483f, 0.0f, 0.3f,
		0.822f, 0.569f, 0.0f, 0.3f,
		0.435f, 0.602f, 0.0f, 0.3f,
		0.310f, 0.747f, 0.0f, 0.3f,
		0.597f, 0.770f, 0.0f, 0.3f,
		0.559f, 0.436f, 0.0f, 0.3f,
		0.359f, 0.583f, 0.0f, 0.3f,
		0.483f, 0.596f, 0.0f, 0.3f,
		0.559f, 0.861f, 0.0f, 0.3f,
		0.195f, 0.548f, 0.0f, 0.3f,
		0.014f, 0.184f, 0.0f, 0.3f,
		0.771f, 0.328f, 0.0f, 0.3f,
		0.406f, 0.615f, 0.0f, 0.3f,
		0.676f, 0.977f, 0.0f, 0.3f,
		0.971f, 0.572f, 0.0f, 0.3f,
		0.140f, 0.616f, 0.0f, 0.3f,
		0.997f, 0.513f, 0.0f, 0.3f,
		0.945f, 0.719f, 0.0f, 0.3f,
		0.543f, 0.021f, 0.0f, 0.3f,
		0.279f, 0.317f, 0.0f, 0.3f,
		0.167f, 0.620f, 0.0f, 0.3f,
		0.347f, 0.857f, 0.0f, 0.3f,
		0.055f, 0.953f, 0.0f, 0.3f,
		0.714f, 0.505f, 0.0f, 0.3f,
		0.783f, 0.290f, 0.0f, 0.3f,
		0.722f, 0.645f, 0.0f, 0.3f,
		0.302f, 0.455f, 0.0f, 0.3f,
		0.225f, 0.587f, 0.0f, 0.3f,
		0.517f, 0.713f, 0.0f, 0.3f,
		0.053f, 0.959f, 0.0f, 0.3f,
		0.393f, 0.621f, 0.0f, 0.3f,
		0.673f, 0.211f, 0.0f, 0.3f,
		0.820f, 0.883f, 0.0f, 0.3f,
		0.982f, 0.099f, 0.0f, 0.3f };
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	///////////////////////////////////////////////////////////////
	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data1), g_vertex_buffer_data1, GL_STATIC_DRAW);

	GLuint colorbuffer1;
	glGenBuffers(1, &colorbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data1), g_color_buffer_data1, GL_STATIC_DRAW);

	do{

		// 清楚屏幕
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 用着色器
		glUseProgram(programID);



		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
		///////face culling
		/*glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);*/




		// 画12个三角形
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		 //12*3 indices starting at 0 -> 12 triangles
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size3表示RGB，4表示RGBA4个变量
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
		

		// 再画12个三角形
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		// 清楚缓冲区
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	 // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);
	


	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteBuffers(1, &colorbuffer1);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

