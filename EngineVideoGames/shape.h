#pragma once
#include "MovableGLM.h"
#include "VertexArray.hpp"
#include "shader.h"
#include "MeshConstructor.h"

#include "texture.h"

class Shape : public MovableGLM
{
private:
	MeshConstructor *mesh = nullptr;
	int texID;
	int shaderID;
	bool isCopy;
	unsigned int mode;
	bool toRender;
	int num_of_shape;
	bool found; //for rewards

public:
	Shape(const Shape& shape,unsigned int mode, const int kind);
	Shape(const std::string& fileName,unsigned int mode, const int kind);
	Shape(const int SimpleShapeType,unsigned int mode, const int kind);
	Shape(Bezier1D *curve, unsigned int xResolution,unsigned int yResolution,bool is2D,unsigned int mode, const int kind);
	virtual ~Shape(void);
	
	void Draw(const std::vector<Shader*> shaders, const std::vector<Texture*> textures,bool isPicking);
	inline void Hide() {toRender = false;}
	inline void Unhide() {toRender = true;}
	inline bool Is2Render() {return toRender;}
	inline bool Is2D(){return mesh->Is2D();}
	inline void SetTexture(int id){ texID = id;}
	inline void SetShader(int id){ shaderID = id;}
	inline int GetShader(){return shaderID;}
	inline int GetTexture(){return texID;}
	
	/* Reut's addings:*/
	MeshConstructor* GetMesh();
	unsigned int GetMode();
	bool Getfound();
	void Setfound(bool value);
	void ChangeMode(unsigned int new_mode);
	void SetNumOfShape(int value);
	int GetNumOfShape();
	int CollisionDetection(Shape* other);
};

