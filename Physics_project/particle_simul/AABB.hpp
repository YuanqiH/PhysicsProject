#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>
#include <vector>
#include<glm\gtx\rotate_vector.hpp>
#include <iostream>

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
#define NBODIES 2

struct EndPoint{
	int mOwner; //owner box
	float mValue; //actual value
	bool mIsMin; //value is min or max
};

struct EndPointList{
	EndPoint endPoints[2 * NBODIES];// list of endpoint for one axis
};

class AABB{
public:
	AABB(){};
	glm::vec3 m_center[NBODIES];
	double half_size[NBODIES];

	void initBox();
	void renderBox(GLint shaderID);
	
	void checkingCollision();
	int pairTable[NBODIES][NBODIES];

private:
	GLuint rigidbody_position_buffer[NBODIES], Rigidbody_color_buffer[NBODIES];
	GLuint ibo_cube_elements[NBODIES], m_vao[NBODIES];
	glm::mat4 BoxModelMatrix[NBODIES];
	void CalculateBoxModel();
	void createList();
	void Insert_sort(EndPointList* sortedList, int length);
	EndPointList mPointList[3];
	
};

const bool isBox = true;

void AABB::initBox(){
	for (int i = 0; i < NBODIES; i++)
	{

	glGenVertexArrays(1, &m_vao[i]);
	glBindVertexArray(m_vao[i]);

	//set for the index
	glGenBuffers(1, &ibo_cube_elements[i]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  ibo_cube_elements[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(cube_elements),cube_elements,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// only initalise the position buffers
	// the VBO containing the position and size of the rigidbodies
	
	glGenBuffers(1, &rigidbody_position_buffer[i]);
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer[i]);
	//initialize with empty( null) buffer, it will be updated later
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set the color buffers
	glGenBuffers(1, &Rigidbody_color_buffer[i]);
	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	}
}

void AABB::renderBox(GLint shaderID){
	//calculate the new matrix for the box
	this->CalculateBoxModel();

	for (int i = 0; i < NBODIES; i++)
	{
	//send the isCollided bool to shader
	glUniformMatrix4fv(glGetUniformLocation(shaderID,"model"),1,GL_FALSE, &BoxModelMatrix[i][0][0]);
	glUniform1f(glGetUniformLocation(shaderID, "isBox"), float(isBox));

	glBindVertexArray(m_vao[i]); // important to bind VA0 before rendering

	//prepare the buffer
	glEnableVertexAttribArray(0);// 1st attribute buffers
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer[i]);
	glVertexAttribPointer(
		0, //attribute 0 
		3, // x+y+z = 3
		GL_FLOAT, //type
		GL_FALSE, //
		0, //stride
		(void*)0); //array buffer offset


	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer[i]);
	glVertexAttribPointer(
		1,
		3,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		0,
		(void*)0);
	

	//set Index for drawing 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements[i]);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	}
}

void AABB::CalculateBoxModel(){
	for (int i = 0; i < NBODIES; i++)
	{
		BoxModelMatrix[i] = glm::mat4(1.0f);
		// translate first, then scale
		// cant figure out why this order
		BoxModelMatrix[i] = glm::translate(BoxModelMatrix[i], m_center[i]);
		BoxModelMatrix[i] = glm::scale(BoxModelMatrix[i],glm::vec3(half_size[i]));
		//std::cout<< "im from calculation "<< i << " current halfsize is" <<half_size[i]<<std::endl;
		
	}
}

void AABB::createList(){
	// once update create a new list
	for (int j = 0; j < 3; j++) // for each axis
	{
		for (int i = 0; i <  NBODIES; i ++)
		{
			mPointList[j].endPoints[2*i].mOwner = i;
			mPointList[j].endPoints[2*i].mIsMin = true;
			mPointList[j].endPoints[2*i].mValue =  m_center[i][j] - half_size[i];

			mPointList[j].endPoints[2*i+1].mOwner = i;
			mPointList[j].endPoints[2*i+1].mIsMin = false;
			mPointList[j].endPoints[2*i+1].mValue =  m_center[i][j] + half_size[i];
		}

	Insert_sort(&mPointList[j],2 * NBODIES);
	}
}

void AABB::Insert_sort(EndPointList* sortedList, int length){
	int j;
	EndPoint temp;
	for (int i = 0; i < length; i++)
	{
		j = i;
		while (j > 0 && sortedList->endPoints[j].mValue <=  sortedList->endPoints[j-1].mValue)
		{
			if (sortedList->endPoints[j-1].mValue == sortedList->endPoints[j].mValue )
			{
				if (sortedList->endPoints[j-1].mOwner < sortedList->endPoints[j].mOwner)
				{
					break; //if the order is increase, dont move it
				}
			}

			temp =  sortedList->endPoints[j];
			 sortedList->endPoints[j] =  sortedList->endPoints[j-1];
			 sortedList->endPoints[j-1] = temp;
			j--;
		}
	}
}

void AABB::checkingCollision(){
	
	createList(); // creatlist here

	memset(pairTable, 0, sizeof(pairTable));//reset pairtable

	for (int j = 0; j < 3; j++) // for every axis
	{
		
		int p=0;
		int startp = 0,endp = 1;
		while (startp < 2*NBODIES)
		{
			int ID ;
			if (mPointList[j].endPoints[startp].mIsMin == true){
				ID = mPointList[j].endPoints[startp].mOwner;
			}
			else{
				startp ++;
				continue;
			}
			endp = startp + 1;
			while (endp < 2*NBODIES)
			{
			
				if ( mPointList[j].endPoints[endp].mIsMin == false && mPointList[j].endPoints[endp].mOwner == ID) // end point is pair of the start 
					{
						startp ++;
						break; // get out of this while loop
					}
				else{
					if ( mPointList[j].endPoints[endp].mIsMin == true){ // only check the start point 
						pairTable[ID][mPointList[j].endPoints[endp].mOwner]++; // use counter for each pair
						//cout<< "pair is: "<< ID << " - " << mPointList[j].endPoints[endp].mOwner<< "at axis : "<< j <<endl;
					}
					endp++;
				}
			}
		}
	}

}