#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <time.h>

#include "Shader.h"
#include "RenderObject.hpp"
#include "RayTest.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

enum TerrainEnum {// 枚举
	Terrain1,
	Terrain2,
	Terrain3,
	Terrain4,
	Terrain5,
	Terrain6,
	TerrainEnum_MAX
};//地形

enum BuildingEnum {
	Building_NONE,
	Building1,
	Building2,
	Building3,
	Building4,
	Building5,
	Building6,
	BuildingEnum_MAX
};

struct Grid {
	TerrainEnum terrainType;
	BuildingEnum buildingType;
	int data;
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int mouse, int action, int mods);
void genTerrainRandom( int typeCount);
void drawTerrain();
bool FirstProbability(int seed);
void lakeMy(Grid* origin, int I, int J);
void lakeShape1(Grid* origin,int I,int J);
void lakeShape2(Grid* origin, int I, int J);
void lakeShape3(Grid* origin, int I, int J);
void lakeInit(Grid* origin, int seedI, int seedJ,int k);
const GLuint WIDTH = 1080, HEIGHT = 720;

GLuint mouseX, mouseY;
GLuint lastMouseX, lastMouseY;
GLFWwindow* window;
glm::vec3 cameraPosition(0.0f, 6.0f, 10.0f);
glm::vec3 cameraPositionSaved;
glm::vec3 ray_origin;
glm::vec3 ray_direction;
glm::mat4 projection;
glm::mat4 view;
glm::vec3 aabb_min(0.0f, 0.0f, -1.0f);
glm::vec3 aabb_max(1.0f, 0.0f, 0.0f);

bool rotateView = false;
bool leftButtonClick = false;

const GLuint gridI = 40;
const GLuint gridJ = 40;
GLuint lakeNum = 0;
float intersection_distance; // Output of TestRayOBBIntersection()
Grid grid[gridI][gridJ] = {};
std::map<int, RenderObject*> terrainDictionary;
std::map<int, RenderObject*> buildingDictionary;

void terrainInitial(){
	// ================================== terrain START ====================================== //
	GLfloat vertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, -1.0f, 0.0f, 1.0f
	};

	GLuint indeces[] = {
		0, 1, 2, 2, 3, 0
	};

	RenderObject* ter1 = new RenderObject(1, LoadShader("meshVertex.vert", "meshFragment.frag"));
	ter1->BindMesh_p3_t2(vertices, sizeof(vertices), indeces, sizeof(indeces));
	ter1->SetTexture(".\\texture\\T1.png");
	terrainDictionary.insert(std::map<TerrainEnum, RenderObject*>::value_type(Terrain1, ter1));

	RenderObject* ter2 = new RenderObject(2, LoadShader("meshVertex.vert", "meshFragment.frag"));
	ter2->BindMesh_p3_t2(vertices, sizeof(vertices), indeces, sizeof(indeces));
	ter2->SetTexture(".\\texture\\T2.png");
	terrainDictionary.insert(std::map<TerrainEnum, RenderObject*>::value_type(Terrain2, ter2));

	RenderObject* ter3 = new RenderObject(3, LoadShader("meshVertex.vert", "meshFragment.frag"));
	ter3->BindMesh_p3_t2(vertices, sizeof(vertices), indeces, sizeof(indeces));
	ter3->SetTexture(".\\texture\\T3.png");
	terrainDictionary.insert(std::map<TerrainEnum, RenderObject*>::value_type(Terrain3, ter3));

	RenderObject* ter4 = new RenderObject(4, LoadShader("meshVertex.vert", "meshFragment.frag"));
	ter4->BindMesh_p3_t2(vertices, sizeof(vertices), indeces, sizeof(indeces));
	ter4->SetTexture(".\\texture\\T4.png");
	terrainDictionary.insert(std::map<TerrainEnum, RenderObject*>::value_type(Terrain4, ter4));

	RenderObject* ter5 = new RenderObject(5, LoadShader("meshVertex.vert", "meshFragment.frag"));
	ter5->BindMesh_p3_t2(vertices, sizeof(vertices), indeces, sizeof(indeces));
	ter5->SetTexture(".\\texture\\T5.png");
	terrainDictionary.insert(std::map<TerrainEnum, RenderObject*>::value_type(Terrain5, ter5));

	RenderObject* ter6 = new RenderObject(6, LoadShader("meshVertex.vert", "meshFragment.frag"));
	ter6->BindMesh_p3_t2(vertices, sizeof(vertices), indeces, sizeof(indeces));
	ter6->SetTexture(".\\texture\\T6.png");
	terrainDictionary.insert(std::map<TerrainEnum, RenderObject*>::value_type(Terrain6, ter6));
	// ================================== terrain DONE ====================================== //
}

void buildingInitial(){

	// ================================== building START ====================================== //
	GLfloat vertices1[] = {
		0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.2f, 1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.9f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.9f, 1.0f, -0.9f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	GLfloat vertices2[] = {
		0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.2f, 1.0f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.9f, 1.0f, 0.1f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.9f, 1.0f, -0.9f, 1.0f, 1.0f, 0.0f, 1.0f	
	};
	GLfloat vertices3[] = {
		0.5f, 0.0f, -0.5f, 0.8f, 0.8f, 0.8f, 1.0f,
		0.2f, 1.0f, -0.5f, 0.8f, 0.8f, 0.8f, 1.0f,
		0.9f, 1.0f, 0.1f, 0.8f, 0.8f, 0.8f, 1.0f,
		0.9f, 1.0f, -0.9f, 0.8f, 0.8f, 0.8f, 1.0f
	};
	GLfloat vertices4[] = {
		0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.2f, 1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.9f, 1.0f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.9f, 1.0f, -0.9f, 0.0f, 1.0f, 0.0f, 1.0f
	};
	GLfloat vertices5[] = {
		0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.2f, 1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.9f, 1.0f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.9f, 1.0f, -0.9f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	GLfloat vertices6[] = {
		0.5f, 0.0f, -0.5f, 0.0f, 0.5f, 0.1f, 1.0f,
		0.2f, 1.0f, -0.5f, 0.0f, 0.5f, 0.1f, 1.0f,
		0.9f, 1.0f, 0.1f, 0.0f, 0.5f, 0.1f, 1.0f,
		0.9f, 1.0f, -0.9f, 0.0f, 0.5f, 0.1f, 1.0f
	};
	

	GLuint indeces1[] = {
		0, 1, 2, 0, 2, 3, 0, 3, 1, 1, 2, 3
	};

	RenderObject* build1 = new RenderObject(3, LoadShader("singlecolor.vert", "singlecolor.frag"));
	build1->BindMesh_p3_c4(vertices1, sizeof(vertices1), indeces1, sizeof(indeces1));
	buildingDictionary.insert(std::map<BuildingEnum, RenderObject*>::value_type(Building1, build1));

	RenderObject* build2 = new RenderObject(4, LoadShader("singlecolor.vert", "singlecolor.frag"));
	build2->BindMesh_p3_c4(vertices2, sizeof(vertices2), indeces1, sizeof(indeces1));
	buildingDictionary.insert(std::map<BuildingEnum, RenderObject*>::value_type(Building2, build2));

	RenderObject* build3 = new RenderObject(5, LoadShader("singlecolor.vert", "singlecolor.frag"));
	build3->BindMesh_p3_c4(vertices3, sizeof(vertices3), indeces1, sizeof(indeces1));
	buildingDictionary.insert(std::map<BuildingEnum, RenderObject*>::value_type(Building3, build3));

	RenderObject* build4 = new RenderObject(6, LoadShader("singlecolor.vert", "singlecolor.frag"));
	build4->BindMesh_p3_c4(vertices4, sizeof(vertices4), indeces1, sizeof(indeces1));
	buildingDictionary.insert(std::map<BuildingEnum, RenderObject*>::value_type(Building4, build4));

	RenderObject* build5 = new RenderObject(7, LoadShader("singlecolor.vert", "singlecolor.frag"));
	build5->BindMesh_p3_c4(vertices5, sizeof(vertices5), indeces1, sizeof(indeces1));
	buildingDictionary.insert(std::map<BuildingEnum, RenderObject*>::value_type(Building5, build5));

	RenderObject* build6 = new RenderObject(8, LoadShader("singlecolor.vert", "singlecolor.frag"));
	build6->BindMesh_p3_c4(vertices6, sizeof(vertices6), indeces1, sizeof(indeces1));
	buildingDictionary.insert(std::map<BuildingEnum, RenderObject*>::value_type(Building6, build6));
	// ================================== building DONE ====================================== //


}
// The MAIN function, from here we start the application and run the game loop
int main()
{
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	 window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	terrainInitial();
	
	buildingInitial();



	// Generate terrain randomly
	genTerrainRandom( TerrainEnum_MAX);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// ========================================= Draw Meshes From Array ========================================== //

		if (rotateView)
		{
			cameraPosition = glm::vec3(glm::rotate(glm::mat4(), -((GLint)mouseX - (GLint)lastMouseX) * 20.0f / WIDTH, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPositionSaved, 1.0f));
		}
		 view = glm::lookAt(
			cameraPosition,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			);
		
	// ====================Screen Position Translated To World Position ==================================== // 	
		 ScreenPosToWorldRay(
			 mouseX, mouseY,
			 WIDTH, HEIGHT,
			 view,
			 projection,
			 ray_origin,
			 ray_direction
			 );

		 //==================================Draw 
		drawTerrain();
		
	// ========================== Draw Done ================================= //


		glfwSwapBuffers(window);
	}
	// ========================== Destroy ======================= //
	for (std::map<int, RenderObject*>::iterator it = terrainDictionary.begin(); it != terrainDictionary.end(); it++){
		delete it->second;
	}

	for (std::map<int, RenderObject*>::iterator it = buildingDictionary.begin(); it != buildingDictionary.end(); it++){
		delete it->second;
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
	mouseX = x;
	mouseY = HEIGHT - y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS) {
			rotateView = true;
			lastMouseX = mouseX;
			lastMouseY = mouseY;
			cameraPositionSaved = cameraPosition;//鼠标中键可以转动左右视角
		}
		else if (action == GLFW_RELEASE)
			rotateView = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS) {
			leftButtonClick = true;
		}
		else if (action == GLFW_RELEASE)
			leftButtonClick = false;
	}
}


void genTerrainRandom(int typeCount)//随机赋予地面
{
	Grid* orderGrid;
	Grid* lakeOrigin;
	Grid* lakeOrigin1;
	int lakeSeedI;
	int lakeSeedJ;
	int lakeSeedI1;
	int lakeSeedJ1;
	int lakeNumber=5;//lake的数量，可修改来控制
	int lakeSeedi[10];
	int lakeSeedj[10];

	srand((int)time(0));
	for (int i = 0; i < gridI; i++)
	{
		for (int j = 0; j < gridJ; j++){
			orderGrid = &grid[i][j];	
			orderGrid->terrainType = (TerrainEnum)((rand() % (typeCount-1)) + 1);
		}
	}
	for (int i = 0; i < lakeNumber; i++){//在5个随机获得的位置画湖
		lakeSeedi[i] = rand() % gridI;
		lakeSeedj[i] = rand() % gridJ;
		lakeOrigin = &grid[lakeSeedi[i]][lakeSeedj[i]];
		int lakeDraw = rand() % 3;//获得随机数
		if (lakeDraw == 0)
		{
			lakeShape1(lakeOrigin, lakeSeedi[i], lakeSeedj[i]);
		}
		if (lakeDraw == 1){
			lakeShape2(lakeOrigin, lakeSeedi[i], lakeSeedj[i]);
		}
		if (lakeDraw == 2){
			lakeShape3(lakeOrigin, lakeSeedi[i], lakeSeedj[i]);
		}
	
	}
	/*lakeSeedI = rand() % gridI;
	lakeSeedJ = rand() % gridJ;
	lakeOrigin = &grid[lakeSeedI][lakeSeedJ];*///起点
	/*lakeMy(lakeOrigin,lakeSeedI+1,lakeSeedJ+1);*/
	/*lakeMy(lakeOrigin, lakeSeedI + 2, lakeSeedJ + 1);*/
	/*lakeShape1(lakeOrigin, lakeSeedI, lakeSeedJ);*/
	/*lakeOrigin->terrainType = Terrain1;*/
	/*lakeInit(lakeOrigin, lakeSeedI, lakeSeedJ, 1);*/
	/*lakeInit(lakeOrigin, 10, 10, 3);
	lakeInit(lakeOrigin, 30, 10, 3);
	lakeInit(lakeOrigin, 1, 1, 1);*/

	
}
void lakeMy(Grid* origin,int I,int J){
	origin = &grid[I][J];
	origin->terrainType = Terrain1;
}
void lakeShape1(Grid* origin,int I,int J){
	lakeMy(origin, I + 1, J + 1);
	lakeMy(origin, I + 1, J + 2);
	lakeMy(origin, I + 2, J + 3);
	lakeMy(origin, I + 3, J + 3);
	lakeMy(origin, I + 4, J + 2);
	lakeMy(origin, I + 4, J + 1);
	lakeMy(origin, I + 3, J );
	lakeMy(origin, I + 2, J );
}

void lakeShape2(Grid* origin, int I, int J){
	for (int i = 1; i < 8; i++)
		for (int j = 1; j < 8; j++){
			lakeMy(origin, I + i, J + j);
		}
}

void lakeShape3(Grid* origin, int I, int J){
	for (int i = 1; i < 3; i++)
		for (int j = 1; j < 8; j++){
			lakeMy(origin, I + i, J + j);
		}
}


//这个东西跟概率有什么关系啊
void lakeInit(Grid* origin,int seedI,int seedJ,int k){
	
	int proNum[4];
	srand((int)time(NULL) );// 选取种子文件
	for (int i = 0; i < 4; i++)
	{
		proNum[i] = rand() % (k+1) ;//获得随机数范围0——k+19
	}
	if (origin->terrainType ==Terrain1&& k > 0)
	{
		
		k--;
		if (FirstProbability(proNum[0])&&seedI < gridI -1)
		{
			origin = &grid[seedI + 1][seedJ];
			origin->terrainType = Terrain1;
			lakeInit(origin, seedI + 1, seedJ, k);

		}
		if (FirstProbability(proNum[1])&&seedJ < gridJ -1)
		{
			origin = &grid[seedI][seedJ + 1];
			origin->terrainType = Terrain1;
			lakeInit(origin, seedI, seedJ + 1, k);
		}
		if (FirstProbability(proNum[2])&& seedI >  0)
		{
			origin = &grid[seedI - 1][seedJ];
			origin->terrainType = Terrain1;
			lakeInit(origin, seedI - 1, seedJ , k);
		}
		if (FirstProbability(proNum[3])&& seedJ > 0)
		{
			origin = &grid[seedI][seedJ - 1];
			origin->terrainType = Terrain1;
			lakeInit(origin, seedI, seedJ - 1, k);
		}
	}
}


//====================draw the terrain=======================//
void drawTerrain(){
	// Grid Mesh Drawing LOOP
	RenderObject* curTerrain;
	RenderObject* curBuilding;
	Grid* curGrid;// 网格
	int terrainX;
	int terrainZ;
	//	drawTerrain(curTerrain, curBuilding, curGrid);
	for (int i = 0; i < gridI; i++)
	{
		for (int j = 0; j < gridJ; j++)
		{
			terrainX = i - (gridI/2);
			terrainZ = j - (gridJ/2);
			curGrid = &grid[i][j];
			curTerrain = terrainDictionary.find(curGrid->terrainType)->second;
			glUseProgram(curTerrain->shaderProgram);
			curTerrain->SetPosition(terrainX, 0, terrainZ);

			if (TestRayOBBIntersection(
				ray_origin,
				ray_direction,
				aabb_min,
				aabb_max,
				curTerrain->model,
				intersection_distance)
				)
			{
				glUniform1i(glGetUniformLocation(curTerrain->shaderProgram, "_Pick"), 1);

				if (leftButtonClick)
				{
					
					switch (curGrid->terrainType){

					case Terrain1:
						curGrid->buildingType = Building1;
						break;
					case Terrain2:
						curGrid->buildingType = Building2;
						break;
					case Terrain3:
						curGrid->buildingType = Building3;
						break;
					case Terrain4:
						curGrid->buildingType = Building4;
						break;
					case Terrain5:
						curGrid->buildingType = Building5;
						break;
					case Terrain6:
						curGrid->buildingType = Building6;
						break;
					default:
						break;
					}
				}
			}
			else{
				glUniform1i(glGetUniformLocation(curTerrain->shaderProgram, "_Pick"), 0);
			}

			curTerrain->Draw(&view, &projection);

			if (curGrid->buildingType != Building_NONE)
			{
				curBuilding = buildingDictionary.find(curGrid->buildingType)->second;

				glUseProgram(curBuilding->shaderProgram);
				curBuilding->SetPosition(terrainX, 0, terrainZ);
				curBuilding->Draw(&view, &projection);
			}

		}
	}
}
bool FirstProbability(int seed){//概率 seed不同生成的随机数序列不同
	int num;
	srand(seed);
	num = rand() % 10;
	if (num < 8)
	{
		return true;
	}
	else
	{
		return false;
	}

}
