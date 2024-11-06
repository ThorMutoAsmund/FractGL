#ifndef COMPUTE_SHADER_CLASS_H
#define COMPUTE_SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include "Resource.h"
#include <windows.h>

//std::string get_file_contents(const char* filename, int *errorNo);
std::string LoadTextFileFromResource(int resourceID, HINSTANCE hInstance, int* errorNo);
std::string toString(double d);

class ComputeShader
{
public:
	int errorNo;

	// Reference ID of the Shader Program
	GLuint ID = 0;
	// Constructor that build the Shader Program from 2 different shaders
	ComputeShader(const char* computeFile);

	// Activates the Shader Program
	void Activate(GLuint SCREEN_WIDTH, GLuint SCREEN_HEIGHT);
	// Deletes the Shader Program
	void Delete();
	void Update(double scale, double xPos, double yPos, int iterations);
private:
	double scale = 1.5f;
	double xPos = 0.7f;
	double yPos = 0.0f;
	int iterations = 128;

	std::string computeCodeBuffer;

	// Checks if the different Shaders have compiled properly
	void ReadShader();
	void CompileErrors(unsigned int shader, const char* type);

};


#endif