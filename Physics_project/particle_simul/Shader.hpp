#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <GL\glew.h>
#include <glm\mat4x4.hpp>

class Shader
{
public:
	GLuint shaderProgramID;

	Shader();

	void enableShader();
	void disableShader();

	void setUniformVector4fv(std::string uniformName, glm::vec4 matrix);
	void setUniformMatrix4fv(std::string uniformName, glm::mat4x4 matrix);

	bool loadShadersFromFiles(std::string vertShaderPath, std::string fragShaderPath);

	GLuint getUniformLocation(std::string uniform);

protected:
	std::string _readShaderFile(std::string path);
	bool _compileShader(std::string source, GLuint shaderType, GLuint shaderID);
	bool _linkShaders(GLuint shaderProgram, GLuint vertShader, GLuint fragShader);

	//GLuint _getUniformLocation(std::string uniform);

	GLuint _vertShaderID;
	GLuint _fragShaderID;
	//set program into public
	//GLuint _shaderProgramID;

	std::map<std::string, GLuint> _shaderVariableLocations;
};

#endif