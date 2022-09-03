#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include"simulation.h"
#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

using namespace std;

const GLint CELL_NO = 400; // total number of cells
const GLfloat CELL_DIM = 0.075f; // length and width of each cell
const GLfloat CELL_GAP = 0.025f; // Gap between cells

struct point {
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	GLfloat z = 0.0f;
};

typedef struct block {
	point vertices[4];
}cell;

GLfloat vertices[400][32] = { 0.0f };
string name = "";
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLuint indices[] =
{
	0, 1, 2, // Upper triangle
	2, 3, 0 // Lower triangle
};

void inititialize(cell* C)
{
	// corner left top
	C[0].vertices[0] = { -0.9875f, 0.9875f };
	C[0].vertices[1] = { -0.9875f, 0.9125f };
	C[0].vertices[2] = { -0.9125f, 0.9125f };
	C[0].vertices[3] = { -0.9125f, 0.9875f };

	// corner bottom left
	C[380].vertices[0] = { -0.9875f, -0.9125f };
	C[380].vertices[1] = { -0.9875f, -0.9875f };
	C[380].vertices[2] = { -0.975f, -0.9875f };
	C[380].vertices[3] = { -0.975f, -0.9125f };


	// left column
	for (int i = 1; i < 20; i++)
	{
		for (short k = 0; k < 4; k++)
		{
			C[i * 20].vertices[k] = { C[(i - 1) * 20].vertices[k].x, C[(i - 1) * 20].vertices[k].y - CELL_DIM - CELL_GAP };
		}
	}

	// whole grid
	for (int i = 0; i < 20; i++)
	{
		for (int j = 1; j < 20; j++)
		{
			for (short k = 0; k < 4; k++)
			{
				C[i * 20 + j].vertices[k] = { C[i * 20 + j - 1].vertices[k].x + CELL_DIM + CELL_GAP, C[i * 20 + j - 1].vertices[k].y };
			}
		}
	}
}

void load_vertices(cell* c)
{
	for (unsigned int i = 0; i < 20; i++)
	{
		for (unsigned int j = 0; j < 20; j++)
		{
			vertices[i * 20 + j][8] = c[i * 20 + j].vertices[0].x;
			vertices[i * 20 + j][9] = c[i * 20 + j].vertices[0].y;
			vertices[i * 20 + j][10] = c[i * 20 + j].vertices[0].z;
			for (short k = 0; k < 3; k++)
			{
				vertices[i * 20 + j][11 + k] = 0.0f;
			}
			vertices[i * 20 + j][14] = 0.0f;
			vertices[i * 20 + j][15] = 1.0f;


			vertices[i * 20 + j][0] = c[i * 20 + j].vertices[1].x;
			vertices[i * 20 + j][1] = c[i * 20 + j].vertices[1].y;
			vertices[i * 20 + j][2] = c[i * 20 + j].vertices[1].z;
			for (short k = 0; k < 3; k++)
			{
				vertices[i * 20 + j][11 + k] = 0.0f;
			}
			vertices[i * 20 + j][6] = 0.0f;
			vertices[i * 20 + j][7] = 0.0f;


			vertices[i * 20 + j][24] = c[i * 20 + j].vertices[2].x;
			vertices[i * 20 + j][25] = c[i * 20 + j].vertices[2].y;
			vertices[i * 20 + j][26] = c[i * 20 + j].vertices[2].z;
			for (short k = 0; k < 3; k++)
			{
				vertices[i * 20 + j][27 + k] = 0.0f;
			}
			vertices[i * 20 + j][30] = 1.0f;
			vertices[i * 20 + j][31] = 0.0f;


			vertices[i * 20 + j][16] = c[i * 20 + j].vertices[3].x;
			vertices[i * 20 + j][17] = c[i * 20 + j].vertices[3].y;
			vertices[i * 20 + j][18] = c[i * 20 + j].vertices[3].z;
			for (short k = 0; k < 3; k++)
			{
				vertices[i * 20 + j][19 + k] = 0.0f;
			}
			vertices[i * 20 + j][22] = 1.0f;
			vertices[i * 20 + j][23] = 1.0f;
		}
	}
}

int main()
{
	cell* c = new cell[400];
	inititialize(c);
	load_vertices(c);
	delete[]c;
	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Boltzman Distribution", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glViewport(0, 0, 800, 800);
	Shader shaderProgram("vertex.shader", "fragment.shader");
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	VAO vao[400];
	VBO vbo[400];
	EBO ebo[400];

	GLfloat vertice[32] = {0};
	for (unsigned int i = 0; i < 400; i++)
	{
		for (unsigned int j = 0; j < 32; j++)
		{
			vertice[j] = vertices[i][j];
		}
		vao[i].Bind();
		vbo[i] = { vertice, sizeof(vertice)};
		ebo[i] = { indices, sizeof(indices) };

		vao[i].LinkAttrib(vbo[i], 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		vao[i].LinkAttrib(vbo[i], 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		vao[i].LinkAttrib(vbo[i], 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		vao[i].Unbind();
		vbo[i].Unbind();
		ebo[i].Unbind();
	}
	Texture number[400];

	for (int i = 0; i < 400; i++)
	{
		//string name = to_string(i) + ".png";
		number[i] = { "1.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE};
		number[i].texUnit(shaderProgram, "tex0", 0);
	}

	us i = 0;
	initialize(cell_value, 1);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();
		glUniform1f(uniID, 0.0f);
		for (int i = 0; i < 400; i++)
		{
			number[i].Bind();
			vao[i].Bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		simulation();
		
		name = to_string(cell_value[r]) + ".png";
		number[r] = { name.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE };
		number[r].texUnit(shaderProgram, "tex0", 0);

		name = to_string(cell_value[a]) + ".png";
		number[a] = { name.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE };
		number[a].texUnit(shaderProgram, "tex0", 0);
		if (i > 30000)
		{
			break;
		}
		i++;
	}

	display(cell_value);

	for (unsigned int i = 0; i < 400; i++)
	{
		vao[i].Delete();
		vbo[i].Delete();
		ebo[i].Delete();
		number[i].Delete();
	}

	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
