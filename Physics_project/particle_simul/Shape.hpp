#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>
#include <vector>
#include<glm\gtx\rotate_vector.hpp>
#include <iostream>
#include <list>

float collisionSkin = 0.00f;

class Shape
{
public:
	Shape();
	~Shape();

	void add(glm::vec3 vertex);
	std::list<glm::vec3>::iterator begin();
	std::list<glm::vec3>::iterator end();
	int getSize();
	glm::vec3 getSupporting(glm::vec3 Dir); //return the farthest point
	void claer();
private:
	std::list<glm::vec3> vertexArray;
};

Shape::Shape()
{
	vertexArray.clear();
}

Shape::~Shape()
{
}

void Shape::add(glm::vec3 vertex){
	vertexArray.push_back(vertex);
}

std::list<glm::vec3>::iterator Shape::begin(){
	std::list<glm::vec3>::iterator ret = vertexArray.begin();
	return ret;
}

std::list<glm::vec3>::iterator Shape::end(){
	std::list<glm::vec3>::iterator ret = vertexArray.end();
	return ret;
}

int Shape::getSize(){
	return vertexArray.size();
}

glm::vec3 Shape::getSupporting(glm::vec3 dir){
	float maxDot = glm::dot( *vertexArray.begin() , dir);
	glm::vec3 returnVertex;
	for (std::list<glm::vec3>::iterator it = vertexArray.begin(); it != vertexArray.end(); ++it)
	{
		float tempDot = glm::dot(*it, dir);
		if (tempDot >= maxDot)
		{
			maxDot = tempDot;
			returnVertex = collisionSkin* glm::normalize(dir) + *it;
		}
	}

	return returnVertex;
}

void Shape::claer(){
	vertexArray.clear();
}