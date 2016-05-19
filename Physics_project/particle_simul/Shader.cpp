#include "Shader.hpp"

Shader::Shader()
{
	this->_fragShaderID = NULL;
	this->_vertShaderID = NULL;
	this->shaderProgramID = NULL;

	this->_shaderVariableLocations = std::map<std::string, GLuint>();
}

void Shader::enableShader()
{
	glUseProgram(this->shaderProgramID);
}

void Shader::disableShader()
{
	glUseProgram(NULL);
}

bool Shader::loadShadersFromFiles(std::string vertShaderPath, std::string fragShaderFile)
{
	/* Read the shaders from file */
	std::string vertShaderSource = this->_readShaderFile(vertShaderPath);
	std::string fragShaderSource = this->_readShaderFile(fragShaderFile);

	/* Make sure we have shader sources */
	if (vertShaderSource == "" || fragShaderSource == "")
	{
		return false;
	}

	/* Get GL to allocate shader space */
	this->_vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	this->_fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	/* Compile the shaders */
	bool vertShaderCompiled = this->_compileShader(vertShaderSource, GL_VERTEX_SHADER, this->_vertShaderID);
	bool fragShaderCompiled = this->_compileShader(fragShaderSource, GL_FRAGMENT_SHADER, this->_fragShaderID);

	/* Make sure shaders compiled */
	if (!vertShaderCompiled || !fragShaderCompiled)
	{
		return false;
	}

	/* Get GL to allocate shader program space */
	this->shaderProgramID = glCreateProgram();

	/* Attach vert and frag shaders to shader program */
	bool linked = this->_linkShaders(this->shaderProgramID, this->_vertShaderID, this->_fragShaderID);

	if (!linked)
	{
		return false;
	}

	/* Always detach after successful link. See: https://www.opengl.org/wiki/Shader_Compilation */
	glDetachShader(this->shaderProgramID, this->_vertShaderID);
	glDetachShader(this->shaderProgramID, this->_fragShaderID);

	return true;
}

std::string Shader::_readShaderFile(std::string shaderPath)
{
	std::ifstream fStream;
	fStream.open(shaderPath);
	if (!fStream.is_open())
	{
		std::cerr << "Could not open shader file: " << shaderPath << std::endl;
		return "";
	}
	std::stringstream buffer;
	buffer << fStream.rdbuf();
	std::string shaderSource = buffer.str();

	return shaderSource;
}

bool Shader::_compileShader(std::string shaderSource, GLuint shaderType, GLuint shaderID)
{
	const char* vShaderSourceChar = shaderSource.c_str();
	const GLint vShaderSourceLength = shaderSource.length();
	glShaderSource(shaderID, 1, &vShaderSourceChar, &vShaderSourceLength);

	glCompileShader(shaderID);

	GLint compiled;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint logLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* errorString = new GLchar[logLength];
		glGetShaderInfoLog(shaderID, logLength, &logLength, errorString);

		const char* sType = shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment";
		std::cerr << "ERROR: Couldn't compile " << sType << " shader:" << std::endl << errorString << std::endl;
		glDeleteShader(shaderID);
		delete[] errorString;
		return false;
	}

	return true;
}

bool Shader::_linkShaders(GLuint shaderProgram, GLuint vertShader, GLuint fragShader)
{
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);

	GLint compiled;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint logLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* errorString = new GLchar[logLength];
		glGetProgramInfoLog(shaderProgram, logLength, &logLength, errorString);

		std::cerr << "ERROR: Couldn't link shader program:" << std::endl << errorString << std::endl;
		glDeleteProgram(shaderProgram);
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		delete[] errorString;
		return false;
	}

	return true;
}

GLuint Shader::getUniformLocation(std::string uniformName)
{
	if (this->_shaderVariableLocations.find(uniformName) != this->_shaderVariableLocations.end())
	{
		return this->_shaderVariableLocations[uniformName];
	}

	GLuint uniformLocation = glGetUniformLocation(this->shaderProgramID, uniformName.c_str());
	if (uniformLocation != -1)
	{
		this->_shaderVariableLocations[uniformName] = uniformLocation;
		return uniformLocation;
	}

	return -1;
}

void Shader::setUniformVector4fv(std::string uniformName, glm::vec4 v)
{
	GLuint uniformLocation = this->getUniformLocation(uniformName);
	if (uniformLocation == -1)
		return;
	glUniform4fv(uniformLocation, 1, &v[0]);
}

void Shader::setUniformMatrix4fv(std::string uniformName, glm::mat4x4 mat)
{
	GLuint uniformLocation = this->getUniformLocation(uniformName);
	if (uniformLocation == -1)
		return;
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &mat[0][0]);
}

