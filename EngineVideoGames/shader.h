#ifndef SHADER_INCLUDED_H
#define SHADER_INCLUDED_H

#include <string>
#include <unordered_map>
#include "glm\glm.hpp"

class Shader
{
public:
	Shader(const std::string& fileName);

	void Bind() const;
	void Unbind() const;

	//Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform3i(const std::string& name, int vi0, int vi1, int vi2); //Added
	void SetUniform4i(const std::string& name, int vi0,int vi1,int vi2,int vi3);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix, int shaderIndx);
	void SetUniformMat4fv(const std::string& name,const glm::mat4 *matrix,const int length);
	void SetUniform4fv(const std::string& name, float value[20]);
	void SetUniform4vArr5(const std::string& name, glm::vec4 value[5], int shaderIndx);

	 ~Shader();
protected:
private:
	enum AttributesLayout
	{
		POSITION_VB,
		COLOR_VB,
		NORMAL_VB,
		WEIGHT_VB,
		TEXCOORD_VB,
		INDEX_VB,	
		JOINT_INDEX_VB
	};
	static const unsigned int NUM_SHADERS = 2;
	void operator=(const Shader& shader) {}
	Shader(const Shader& shader) {}
	
	std::string LoadShader(const std::string& fileName);
	//void CheckShaderError(unsigned int shader, unsigned int flag, bool isProgram, const std::string& errorMessage);
	unsigned int CreateShader(const std::string& text, unsigned int type);

	unsigned int m_program;
	unsigned int m_shaders[NUM_SHADERS];
	std::unordered_map<std::string, int> m_UniformLocationCache;
	int GetUniformLocation (const std::string& name);
	/* Reut's addings: */
	int GetUniformLocationWithShader(const std::string& name, int shaderIndx);
};

#endif
