#include"computeShaderClass.h"

// Constructor that build the Shader Program from 2 different shaders
ComputeShader::ComputeShader(const char* computeFile)
{
	HINSTANCE moduleHandle = GetModuleHandle(nullptr);

	// Read vertexFile and fragmentFile and store the strings
	//computeCodeBuffer = get_file_contents(computeFile, &errorNo);
	computeCodeBuffer = LoadTextFileFromResource(IDR_TEXT1, moduleHandle, &errorNo);

	if (errorNo != 0)
	{
		return;
	}

	ReadShader();

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader" <<
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";
}

void ComputeShader::Update(double scale, double xPos, double yPos, int iterations)
{
	this->scale = scale;
	this->xPos = xPos;
	this->yPos = yPos;
	this->iterations = iterations;

	ReadShader();
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void ComputeShader::ReadShader()
{
	// Update shader code
	std::string computeCode = computeCodeBuffer;
	replace(computeCode, "NUMERIC scale = 1.0;", "NUMERIC scale = " + std::to_string(scale) + ";");
	replace(computeCode, "NUMERIC xpos = 0.0;", "NUMERIC xpos = " + std::to_string(xPos) + ";");
	replace(computeCode, "NUMERIC ypos = 0.0;", "NUMERIC ypos = " + std::to_string(yPos) + ";");
	replace(computeCode, "int maxIter = 128;", "int maxIter = " + std::to_string(iterations) + ";");

	const char* computeSource = computeCode.c_str();

	// Create Vertex Shader Object and get its reference
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(computeShader, 1, &computeSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(computeShader);
	// Checks if Shader compiled succesfully
	CompileErrors(computeShader, "COMPUTE");

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, computeShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);
	// Checks if Shaders linked succesfully
	CompileErrors(ID, "COMPUTE_PROGRAM");

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(computeShader);

}

// Activates the Shader Program
void ComputeShader::Activate(GLuint SCREEN_WIDTH,GLuint SCREEN_HEIGHT)
{
	glUseProgram(ID);
	glDispatchCompute((int)ceil(SCREEN_WIDTH / 8), (int)ceil(SCREEN_HEIGHT / 4), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

// Deletes the Shader Program
void ComputeShader::Delete()
{
	glDeleteProgram(ID);
}

// Checks if the different Shaders have compiled properly
void ComputeShader::CompileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}