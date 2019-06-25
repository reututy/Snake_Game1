#pragma once
#include <vector>
#include "glm\glm.hpp"
#include "VertexArray.hpp"
#include "Mesh.h"
#include "bezier1D.h"
#include "kdtree.h"


class MeshConstructor
{
	VertexArray vao;
	IndexBuffer *ib;
	std::vector<VertexBuffer*> vbs;
	bool is2D;
	int unsigned indicesNum;
	
	void InitMesh(IndexedModel &model);
	void CopyMesh(const MeshConstructor &mesh);
	void CopyLine(const MeshConstructor &mesh);
	static const unsigned int VEC3_ATTRIB_NUM = 4;
	static const unsigned int VEC2_ATTRIB_NUM = 1;
	
public:
	enum SimpleShapes
	{
		Axis,
		Plane,
		Cube,
		Octahedron,
		Tethrahedron,
		BezierLine,
		BezierSurface,
	};
	MeshConstructor(const int type);
	MeshConstructor(Bezier1D *curve,bool isSurface,unsigned int resT,unsigned int resS);
	MeshConstructor(const MeshConstructor &mesh);
	MeshConstructor(const std::string& fileName);

	void Bind() {vao.Bind();}
	void Unbind() {vao.Unbind();}
	inline unsigned int GetIndicesNum(){return indicesNum;}
	inline bool Is2D() {return is2D;}
	void InitLine(IndexedModel &model);
	~MeshConstructor(void);
};

