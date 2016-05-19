#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <map>

#include "Shader.hpp"

#define SHADER_MANAGER_DEFAULT_SHADER_FOLDER_PATH "../shaders/"
#define SHADER_MANAGER_DEFAULT_VERT_SHADER_EXT ".vert"
#define SHADER_MANAGER_DEFAULT_FRAG_SHADER_EXT ".frag"

class ShaderManager
{
public:
	static bool setShadersFolder(std::string);
	static Shader* loadShader(std::string);
	static bool useShader(std::string shaderName);
	static Shader* getShader(std::string shaderName);

private:
	static std::map<std::string, Shader*> _shaders;
	static std::string _shaderFolderPath;
	static std::string _vertShaderExt;
	static std::string _fragShaderExt;
};

#endif