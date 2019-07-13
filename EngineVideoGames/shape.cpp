#define GLEW_STATIC
#include <GL\glew.h>
#include "shape.h"
#include "Log.hpp"


Shape::Shape(const Shape& shape,unsigned int mode, const int kind)
{
	mesh = new MeshConstructor(*shape.mesh);
	//tex = shape.tex;
	isCopy = true;
	this->mode = mode;
	toRender = true;
	texID = -1;
	shaderID = 1;
}

Shape::Shape(const std::string& fileName, unsigned int mode, const int kind)
{
	mesh = new MeshConstructor(fileName, kind);
	isCopy = false;
	this->mode = mode;
	toRender = true;
	texID = -1;
	shaderID = 1;
}

Shape::Shape(const int SimpleShapeType,unsigned int mode, const int kind)
{
	mesh = new MeshConstructor(SimpleShapeType, kind);
	//mesh->Bind();
	this->mode = mode;
	isCopy = false;
	toRender = true;
	texID = -1;
	shaderID = 1;
}

Shape::Shape(Bezier1D *curve, unsigned int xResolution,unsigned int yResolution,bool is2D,unsigned int mode, const int kind)
{
	mesh = new MeshConstructor(curve,is2D,xResolution,yResolution, kind);
	this->mode = mode;
	isCopy = false;
	toRender = true;
	texID = -1;
	shaderID = 1;
}

Shape::~Shape(void)
{
	if (!isCopy)
	{
		if (mesh)
			delete mesh;
	}
}

void Shape::Draw( const std::vector<Shader*> shaders, const std::vector<Texture*> textures,bool isPicking)
{
	if(texID>=0)
		textures[texID]->Bind(0);
	if(isPicking)
		shaders[0]->Bind();
	else
		shaders[shaderID]->Bind();
	mesh->Bind();
	/*if(isCopy)
		glDrawArrays(GL_TRIANGLES, 0, indicesNum);
	else*/
	GLCall(glDrawElements(mode,mesh->GetIndicesNum(), GL_UNSIGNED_INT, 0));
	mesh->Unbind();
}

MeshConstructor* Shape::GetMesh()
{
	return mesh;
}

unsigned int Shape::GetMode()
{
	return mode;
}

int Shape::GetType()
{
	return type;
}

bool Shape::Getfound()
{
	return found;
}

void Shape::Setfound(bool value)
{
	found = value;
}

void Shape::SetType(int value)
{
	type = value;
}

void Shape::ChangeMode(unsigned int new_mode)
{
	this->mode = new_mode;
}

void Shape::SetNumOfShape(int value)
{
	num_of_shape = value;
}

int Shape::GetNumOfShape()
{
	return num_of_shape;
}

// Returns -1 if there is no collision 
int Shape::CollisionDetection(Shape* other)
{
	BoundingBox* box_that_collides = mesh->CollisionDetection(other->GetMesh(),
		glm::translate(glm::mat4(1), glm::vec3(this->getTraslate())), GetRot(),
		glm::translate(glm::mat4(1), glm::vec3(other->getTraslate())), other->GetRot());
	if (box_that_collides != nullptr)
		return box_that_collides->GetNumOfShape();
	return -1;
}

