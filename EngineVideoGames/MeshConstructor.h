#pragma once
#include <vector>
#include "glm\glm.hpp"
#include "VertexArray.hpp"
#include "Mesh.h"
#include "bezier1D.h"
#include "kdtree.h"
#include "BVH.h"

class MeshConstructor
{
	VertexArray vao;
	IndexBuffer *ib;

	/* Reut's Addings: */
	Kdtree kdtree;
	BVH bvh;
	int kind;
	std::vector<glm::vec3> positions;

	std::vector<VertexBuffer*> vbs;
	bool is2D;
	int unsigned indicesNum;
	
	void InitMesh(IndexedModel &model);
	void CopyMesh(const MeshConstructor &mesh);
	void CopyLine(const MeshConstructor &mesh);
	static const unsigned int VEC3_ATTRIB_NUM = 4;
	static const unsigned int VEC2_ATTRIB_NUM = 1;
	
public:
	enum Kind { Default, Snake, Reward, Obstacle, WallWin, Bubble };
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
	MeshConstructor(const int type, const int kind);
	MeshConstructor(Bezier1D *curve,bool isSurface,unsigned int resT,unsigned int resS, const int kind);
	MeshConstructor(const MeshConstructor &mesh, const int kind);
	MeshConstructor(const std::string& fileName, const int kind, float scale);

	void Bind() {vao.Bind();}
	void Unbind() {vao.Unbind();}
	inline unsigned int GetIndicesNum(){return indicesNum;}
	inline bool Is2D() {return is2D;}
	void InitLine(IndexedModel &model);
	~MeshConstructor(void);

	void CreateTree(std::vector<glm::vec3> positions);
	BVH* CreateBVH(std::vector<glm::vec3> points, Node* curr_node, int level);
	BoundingBox* MeshConstructor::CollisionDetection(MeshConstructor* other, glm::mat4 this_trans, glm::mat4 this_rot,
		glm::mat4 other_trans, glm::mat4 other_rot);

	BVH* GetBVH();
	int GetKind();
	void SetKind(int kind);
};

