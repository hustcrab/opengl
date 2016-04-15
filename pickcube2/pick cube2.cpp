

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
#include <sstream>
#include<Windows.h>

#include <GL/glew.h>
#include <GlFW/glfw3.h>
#include<AntTweakBar.h>
using namespace std;
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
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


void ScreenPosToWorldRay(
	int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	int screenWidth, int screenHeight,  // Window size, in pixels
	glm::mat4 ViewMatrix,               // Camera position and orientation
	glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
	){

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	//光线起点和终点确定，根据这两个点去确定光线的方向
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
		);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
		);


	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	glm::vec4 lRayStart_world = InverseViewMatrix       * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
	glm::vec4 lRayEnd_world = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;


	// Faster way (just one inverse)
	//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);


	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}
bool TestRayOBBIntersection(
	glm::vec3 ray_origin,        // Ray origin, in world space
	glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
	){

	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f){ // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1>t2){
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f){

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1>t2){ float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else{
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f){

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1>t2){ float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else{
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;

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
	window = glfwCreateWindow(1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
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
	//混合
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//深度缓存
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	
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
		//---------------------------------------------------------------------------------//我只是华丽的分割线

	};

	// One color for each vertex. They were generated randomly.
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
		/////////////////////////////////////

	};




	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	do{
		// 清楚屏幕
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);


		// 用着色器

		glm::vec3 cubePositions[2] = {
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -3.0f),

		};

		for (GLuint i = 0; i < 2; i++){
			glUseProgram(programID);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			//设置顶点属性指针
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



			// Model matrix : an identity matrix (model will be at the origin)
			glm::mat4 Model;
			Model = glm::translate(Model, cubePositions[i]);


			// Our ModelViewProjection : multiplication of our 3 matrices
			glm::mat4 MVP = Projection * View * Model;

			GLuint MatrixID = glGetUniformLocation(programID, "MVP");

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

		}
		glm::mat4 model1;
		model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 model2;
		model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, -3.0f));

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){




			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			glm::vec3 ray_origin;
			glm::vec3 ray_direction;
			ScreenPosToWorldRay(
				xpos, 768-ypos,
				1024, 768,
				View,
				Projection,
				ray_origin,
				ray_direction
				);




			//检测面
			float intersection_distance; // Output of TestRayOBBIntersection()
			glm::vec3 aabb_min(-2.0f, -2.0f, -2.0f);
			glm::vec3 aabb_max(2.0f, 2.0f, 2.0f);

			message = "no cube";

			if (TestRayOBBIntersection(
				ray_origin,
				ray_direction,
				aabb_min,
				aabb_max,
				model1,
				intersection_distance)
				){
				message = "1";
			}
			if (TestRayOBBIntersection(
				ray_origin,
				ray_direction,
				aabb_min,
				aabb_max,
				model2,
				intersection_distance)
				){
				message = "2";
			}


			cout << message << endl;
			Sleep(500);


		}


		// 1rst attribute buffer : vertices




		// 画12个三角形


		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);



	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);


	// Close OpenGL window and terminate GLFW

	glfwTerminate();



	return 0;
}

