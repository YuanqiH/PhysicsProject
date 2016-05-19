#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MyTexture
{
private:
	GLuint textureID;
public:
	
	
	MyTexture()
	{
		
	}


	void TextureFromFile(const char* name, string directory)
	{
		 //Generate texture ID and load texture data 
		string filename = string(name);
		filename = directory + '/' + filename;
		//GLuint textureID;
		glGenTextures(1, &this->textureID);
		int width,height;
		unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		std::cout<<"the texture image's name is: "<<filename.c_str()<<std::endl;
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);	

		// Parameters
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);
		//return textureID;
	}

	void use(GLuint shaderProgramID,std::string name, int textUnit = 0)
	{
		//assume this texture only use texture unit 0
		//this function only for single texture, if more needed, modification needed
		glActiveTexture(GL_TEXTURE0 + textUnit);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), textUnit);
		
            // And finally bind the texture
        
	}



};
