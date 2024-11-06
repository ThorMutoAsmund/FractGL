//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

// References
// https://www.glfw.org/download.html
// https://glad.dav1d.de/
// https://alaingalvan.gitbook.io/a-trip-through-the-graphics-pipeline

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>

#include"Texture.h"
#include"screenShaderClass.h"
#include"computeShaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"WindowHandler.h"
#include <windows.h>


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

bool vSync = true;

// Vertices coordinates
GLfloat vertices[] =
{
//   COORDINATES            TexCoord
	-1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
	 1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
	 1.0f, -1.0f, 0.0f,     1.0f, 0.0f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};


std::string LoadTextFileFromResource(int resourceID, HINSTANCE hInstance, int* errorNo)
{
	*errorNo = 0;
	// Locate the resource
	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), RT_RCDATA);
	if (!hResource)
	{
		*errorNo = 1;
		return "";
	}

	// Load the resource
	HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
	if (!hLoadedResource)
	{
		*errorNo = 2;
		return "";
	}

	// Lock the resource to get a pointer to it
	LPVOID pLockedResource = LockResource(hLoadedResource);
	if (!pLockedResource)
	{
		*errorNo = 3;
		return "";
	}

	// Get the resource size
	DWORD resourceSize = SizeofResource(hInstance, hResource);
	if (resourceSize == 0)
	{
		*errorNo = 4;
		return "";
	}


	// Copy the resource data into a string
	return std::string(static_cast<const char*>(pLockedResource), resourceSize);
}

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename, int *errorNo)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		*errorNo = 0;
		return(contents);
	}

	*errorNo = errno;
	return "";
}

int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FractGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	std::cout << "Started\n";

	// Generates Shader objects
	ScreenShader screenShaderProgram("default.vert", "default.frag");
	if (screenShaderProgram.errorNo != 0)
	{
		screenShaderProgram.Delete();

		// Delete window before ending the program
		glfwDestroyWindow(window);

		// Terminate GLFW before ending the program
		glfwTerminate();
		std::cerr << "Error reading fragment/vertex shader: " << screenShaderProgram.errorNo << "\n";
		return screenShaderProgram.errorNo;
	}

	ComputeShader computeShaderProgram("default.comp");
	if (computeShaderProgram.errorNo != 0)
	{
		screenShaderProgram.Delete();
		computeShaderProgram.Delete();

		// Delete window before ending the program
		glfwDestroyWindow(window);

		// Terminate GLFW before ending the program
		glfwTerminate();
		std::cerr << "Error reading compute shader: " << computeShaderProgram.errorNo << "\n";
		return computeShaderProgram.errorNo;
	}


	// Specify the viewport of OpenGL in the Window
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	WindowHandler windowHandler(window);
	
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));
	VAO VAO1(VBO1, EBO1);

	GLuint screenTex;
	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	bool draw = true;
	bool clear = true;
	bool reset = true;
	bool zooming = false;

	double scale = 1.5f;
	double xPos = 0.7f;
	double yPos = 0.0f;
	double scaleSpeed = 1.5f;
	int iterations = 128;
	bool freeRun = true;

	computeShaderProgram.Update(scale, xPos, yPos, iterations);

	double previousTime = glfwGetTime();
	int frameCount = 0;

	// Main while loop
	while (!windowHandler.quitPressed() && !glfwWindowShouldClose(window))
	{
		if (clear || freeRun)
		{
			// Specify the color of the background
			glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
			// Clean the back buffer and assign the new color to it
			glClear(GL_COLOR_BUFFER_BIT);
		}

		if (draw || freeRun)
		{
			// Activate compute shader
			computeShaderProgram.Activate(SCREEN_WIDTH, SCREEN_HEIGHT);

			// Tell OpenGL which Shader Program we want to use
			screenShaderProgram.Activate();
			// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program

			glBindTextureUnit(0, screenTex);
			glUniform1i(glGetUniformLocation(screenShaderProgram.ID, "screen"), 0);

			// Bind the VAO so OpenGL knows to use it
			VAO1.Bind();

			// Draw primitives, number of indices, datatype of indices, index of indices
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		
		if (freeRun || clear || draw)
		{
			// Swap the back buffer with the front buffer
			glfwSwapBuffers(window);
		}

		if (reset)
		{
			clear = false;
			draw = false;
			reset = false;
			zooming = false;
		}

		// Take care of all GLFW events
		glfwPollEvents();

		switch (windowHandler.getKeyAction())
		{
		case WH_CLEAR:
			clear = true;
			reset = true;
			break;
		case WH_ITER_UP:
			iterations <<= 1;
			if (!zooming)
			{
				scaleSpeed = 1.0f;
				zooming = true;
				clear = true;
				draw = true;
				reset = true;
			}
			break;
		case WH_ITER_DOWN:
			if (iterations > 8)
			{
				iterations >>= 1;
				if (!zooming)
				{
					scaleSpeed = 1.0f;
					zooming = true;
					clear = true;
					draw = true;
					reset = true;
				}
			}
			break;
		case WH_ZOOM_IN:
			scaleSpeed = 0.99f;
			zooming = true;
			clear = true;
			draw = true;
			reset = false;
			break;
		case WH_ZOOM_OUT:
			scaleSpeed = 1 / 0.99f;
			zooming = true;
			clear = true;
			draw = true;
			reset = false;
			break;
		case WH_ZOOM_END:
			zooming = false;
			reset = true;
			break;
		case WH_FREE_RUN:
			freeRun = !freeRun;
			break;
		case WH_COPY_COORDINATES:
			//HWND handle = glfwGetWin32Window(window);
			if (OpenClipboard(0))
			{
				EmptyClipboard();
				
				std::wstring strData = std::to_wstring(xPos) + L" " + std::to_wstring(yPos) + L" " + std::to_wstring(scale) + L" " + std::to_wstring(iterations);
				int sz = sizeof(WCHAR) * strData.length() + 1;
				HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, sz);

				if (hClipboardData != nullptr)
				{
					WCHAR* pchData;
					pchData = (WCHAR*)GlobalLock(hClipboardData);
					wcscpy_s(pchData, sz, strData.c_str());
					GlobalUnlock(hClipboardData);
					SetClipboardData(CF_UNICODETEXT, hClipboardData);

				}
				CloseClipboard();
			}
			break;
		case WH_PASTE_COORDINATES:
			//HWND handle = glfwGetWin32Window(window);
			if (OpenClipboard(nullptr))
			{
				HANDLE hClipbpardData = GetClipboardData(CF_UNICODETEXT);
				if (hClipbpardData != nullptr)
				{
					WCHAR* pszText = static_cast<WCHAR*>(GlobalLock(hClipbpardData));
					if (pszText != nullptr)
					{
						std::wstring temp;
						std::wstring coordString(pszText);

						std::vector<double> parts;
						std::wstringstream wss(coordString);
						while (std::getline(wss, temp, L' '))
						{
							parts.push_back(std::stod(temp));
						}

						if (parts.size() > 3)
						{
							scaleSpeed = 1.0f;
							xPos = parts[0];
							yPos = parts[1];
							scale = parts[2];
							iterations = (int)parts[3];
							zooming = true;
							clear = true;
							draw = true;
							reset = true;
						}
					}
				}

				CloseClipboard();
			}
			break;
		}

		if (zooming)
		{
			double selXPos = ((windowHandler.xPos / (double)SCREEN_WIDTH) * 2.0f - 1.0f) * scale;
			double selYPos = ((windowHandler.yPos / (double)SCREEN_HEIGHT) * 2.0f - 1.0f) * -scale;

			scale *= scaleSpeed;

			double selXPos2 = ((windowHandler.xPos / (double)SCREEN_WIDTH) * 2.0f - 1.0f) * scale;
			double selYPos2 = ((windowHandler.yPos / (double)SCREEN_HEIGHT) * 2.0f - 1.0f) * -scale;

			xPos += (selXPos - selXPos2);
			yPos += (selYPos - selYPos2);

			computeShaderProgram.Delete();
			computeShaderProgram.Update(scale, xPos, yPos, iterations);
		}

		double currentTime = glfwGetTime();
		frameCount++;
		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			// Display the frame count here any way you want.
			std::string debugText = std::string("FractGL (FPS = ") + std::to_string(frameCount) + ")";
			glfwSetWindowTitle(window, debugText.c_str());

			frameCount = 0;
			previousTime = currentTime;
		}
		
	}

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	screenShaderProgram.Delete();
	computeShaderProgram.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);

	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

