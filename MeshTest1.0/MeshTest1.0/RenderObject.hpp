#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

/////////////////////////////////////////////////////////////////
//                                                             //
//    File: RenderObject.hpp								   //
//    Class: RenderObject                                      //
//    Describe: The base class of every object that will be    //
//  rendered by OpenGL.                                        //
//                                                             //
/////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <SOIL.h>
#pragma comment(lib, "soil.lib")


//-- The base class of every object that will be rendered by OpenGL.
class RenderObject 
{
	public:
		/* ============================================= Public Member Variables ====================================================== */

		//-- Different unique ID for each object.
		GLuint ID;		
		//-- Model Matrix of the object.
		glm::mat4 model;	
		//-- Shader used to render this object
		//-- Should be Used By glUseProgram() before you modify its shader.
		GLuint shaderProgram;	


		/* ============================================= Constructor ========================================================= */

		//-- Constructor of class RenderObject.
		RenderObject(GLuint ID, GLuint shaderProgram)
		{
			this->ID = ID;
			this->shaderProgram = shaderProgram;
			this->model = glm::mat4();		// Model Matrix should be initialized as an Identity Matrix
		}
		

		/* ============================================= Public functions ====================================================== */

		//-- Assign a mesh to this object.
		//-- For each vertex: 3 GLfloat for Position, 4 GLfloat for Color.
		void BindMesh_p3_c4(GLfloat vertices[], GLsizeiptr verticesLength, GLuint indices[], GLsizeiptr indicesLength) 
		{
			GLuint VBO, EBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, verticesLength, vertices, GL_STATIC_DRAW);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength, indices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

			glBindVertexArray(0);

			indexCount = indicesLength / sizeof(GLuint);
		}

		//-- Assign a mesh to this object.
		//-- For each vertex: 3 GLfloat for Position.
		void BindMesh_p3(GLfloat vertices[], GLsizeiptr verticesLength, GLuint indices[], GLsizeiptr indicesLength)
		{
			GLuint VBO, EBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, verticesLength, vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength, indices, GL_STATIC_DRAW);
			
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

			glBindVertexArray(0);

			indexCount = indicesLength / sizeof(GLuint);
		}

		//-- Assign a mesh to this object.
		//-- For each vertex: 3 GLfloat for Position, 2 GLfloat for TexCoord.
		void BindMesh_p3_t2(GLfloat vertices[], GLsizeiptr verticesLength, GLuint indices[], GLsizeiptr indicesLength)
		{
			GLuint VBO, EBO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, verticesLength, vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength, indices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

			glBindVertexArray(0);

			indexCount = indicesLength / sizeof(GLuint);
		}

		//-- Call this function to render this object.
		void Draw(glm::mat4* view, glm::mat4* projection)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);		
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(shaderProgram, "_Diffuse"), 0);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(*view));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		//-- Call this function to render this object if you want to render it in other mode.
		void Draw(GLenum type)
		{
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawElements(type, indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		//-- Set the Model Matrix by hand.
		void SetModel(glm::mat4& model)
		{
			this->model = model;
		}

		//-- Set the Position in world space.
		//-- This will automatically modify its Model Matrix.
		void SetPosition(GLfloat x, GLfloat y, GLfloat z)
		{
			model[3].x = x;
			model[3].y = y;
			model[3].z = z;
		}

		//-- Set the Texture of this object if it should have one.
		void SetTexture(const char* imagePath)
		{
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// Set texture filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Load, create texture
			int w, h;
			unsigned char* image = SOIL_load_image(imagePath, &w, &h, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		

	protected:
		/* ============================================= Protected Member Variables ====================================================== */

		GLuint VAO;
		GLuint indexCount;
		GLuint texture;
};


#endif // RENDER_OBJECT_H

