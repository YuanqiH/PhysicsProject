#pragma once

#include <iostream>
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class Cube
{
public:
	Cube();
	void render(GLint shaderID);

private:
	GLuint rigidbody_position_buffer, Rigidbody_color_buffer;
	GLuint ibo_cube_elements, m_vao;
};

Cube::Cube()
{

	const GLfloat cube_vertices[] = {
    // front
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // back
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,
  };

  GLfloat cube_colors[] = {
    // front colors
    1.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    // back colors
    1.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
  };

	/* init_resources */
GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	//prepare for all the buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//set for the index
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(cube_elements),cube_elements,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// only initalise the position buffers
	// the VBO containing the position and size of the rigidbodies
	
	glGenBuffers(1, &rigidbody_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer);
	//initialize with empty( null) buffer, it will be updated later
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set the color buffers
	glGenBuffers(1, &Rigidbody_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void Cube::render(GLint shaderID)
{
	
	glBindVertexArray(m_vao); // important to bind VA0 before rendering

	//prepare the buffer
	glEnableVertexAttribArray(0);// 1st attribute buffers
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer);
	glVertexAttribPointer(
		0, //attribute 0 
		3, // x+y+z = 3
		GL_FLOAT, //type
		GL_FALSE, //
		0, //stride
		(void*)0); //array buffer offset


	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer);
	glVertexAttribPointer(
		1,
		3,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		0,
		(void*)0);
	

	//set Index for drawing 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

}