#include"VAO.h"

// Constructor that generates a VAO ID
VAO::VAO(VBO& VBO, EBO& EBO)
{
	//glGenVertexArrays(1, &ID);
	glCreateVertexArrays(1, &ID);

	LinkAttrib(VBO, EBO);

}

// Links a VBO Attribute such as a position or color to the VAO
void VAO::LinkAttrib(VBO& VBO, EBO& EBO)// GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset)
{
	glEnableVertexArrayAttrib(ID, 0);
	glVertexArrayAttribBinding(ID, 0, 0);
	glVertexArrayAttribFormat(ID, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(ID, 1);
	glVertexArrayAttribBinding(ID, 1, 0);
	glVertexArrayAttribFormat(ID, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(ID, 0, VBO.ID, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(ID, EBO.ID);

	//VBO.Bind();
	//glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	//glEnableVertexAttribArray(layout);
	//VBO.Unbind();
}

// Binds the VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}

// Unbinds the VAO
void VAO::Unbind()
{
	//glBindVertexArray(0);
}

// Deletes the VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}