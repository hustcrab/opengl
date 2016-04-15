

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
#include<Windows.h>

#include <GL/glew.h>
#include <GlFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
unsigned char data[4];
std::string message;

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

	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	//深度缓存
	/*glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);*/


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
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f


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
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f };
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
		glFrontFace(GL_CW);
*/



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
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){


			glFlush();
			glFinish();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			glReadPixels(xpos,768-ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			int a = data[0];
			int b = data[1];

			if (a == 255){
				message = "red cube";


			}
			else
			{
				if (b == 76){
					message = "green cube";
				}
				else
				{
					message = "no cube";
				}
				
			}
			cout << message << endl;
			Sleep(500);

		}
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

