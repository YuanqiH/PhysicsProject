#include "ShaderManager.hpp"

/* Initialise static member variables */
std::map<std::string, Shader*> ShaderManager::_shaders;
std::string ShaderManager::_shaderFolderPath = SHADER_MANAGER_DEFAULT_SHADER_FOLDER_PATH;
std::string ShaderManager::_vertShaderExt = SHADER_MANAGER_DEFAULT_VERT_SHADER_EXT;
std::string ShaderManager::_fragShaderExt = SHADER_MANAGER_DEFAULT_FRAG_SHADER_EXT;

Shader* ShaderManager::loadShader(std::string shader)
{
	std::stringstream vertShaderSS;
	vertShaderSS << _shaderFolderPath;
	vertShaderSS << shader;
	vertShaderSS << _vertShaderExt;

	std::stringstream fragShaderSS;
	fragShaderSS << _shaderFolderPath;
	fragShaderSS << shader;
	fragShaderSS << _fragShaderExt;

	Shader* s = new Shader();
	bool loaded = s->loadShadersFromFiles(vertShaderSS.str(), fragShaderSS.str());

	if (!loaded)
		return NULL;

	_shaders[shader] = s;

	return s;
}

bool ShaderManager::useShader(std::string shader)
{
	/* Make sure we have such a shader first */
	Shader* s = ShaderManager::getShader(shader);
	if (s != NULL)
	{
		s->enableShader();
		return true;
	}
	return false;
}

Shader* ShaderManager::getShader(std::string shader)
{
	/* Make sure we have such a shader first */
	if (_shaders.find(shader) != _shaders.end())
	{
		return _shaders[shader];
	}
	return NULL;
}