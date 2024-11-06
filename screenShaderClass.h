#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include "Resource.h"
#include <windows.h>

//std::string get_file_contents(const char* filename, int* errorNo);
std::string LoadTextFileFromResource(int resourceID, HINSTANCE hInstance, int* errorNo);

class ScreenShader
{
public:
	int errorNo;

	// Reference ID of the Shader Program
	GLuint ID = 0;
	// Constructor that build the Shader Program from 2 different shaders
	ScreenShader(const char* vertexFile, const char* fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};


#endif