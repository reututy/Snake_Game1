#define GLEW_STATIC
#include <GL\glew.h>
#include <fstream> 
#include "MeshConstructor.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "bezier2D.h"
#include "obj_loader.h"

#define MINIMUM_VERTCIES_FOR_BVH 800

MeshConstructor::MeshConstructor(const int type, const int kind)
{
	this->kind = kind;
	switch (type)
	{
	case Axis:	
		InitLine(AxisGenerator());
		break;
	case Plane:
		InitMesh(PlaneTriangles());
		break;
	case Cube:
		 InitMesh(CubeTriangles());
		 break;
	case Octahedron:
		 InitMesh(OctahedronGenerator());
		 break;
	case Tethrahedron:
		 InitMesh(TethrahedronGenerator());
		 break;
	default:
		break;
	}
	
}

MeshConstructor::MeshConstructor(const std::string& fileName, const int kind, float scale)
{
	this->kind = kind;
	//if (kind == Reward)
		//InitMesh(OBJModel(fileName, scale).ToIndexedModel());
	//else
		InitMesh(OBJModel(fileName).ToIndexedModel());
}

MeshConstructor::MeshConstructor(Bezier1D *curve,bool isSurface,unsigned int resT,unsigned int resS, const int kind)
{
	if (isSurface)
	{
		this->kind = kind;
		Bezier2D surface(*curve, curve->GetAxis(), 4);
		InitMesh(surface.GetSurface(resT, resS));
	}
	else
	{
		this->kind = kind;
		InitLine(curve->GetLine(resT));
	}
}

MeshConstructor::MeshConstructor(const MeshConstructor &mesh, const int kind)
{
	this->kind = kind;
	indicesNum = mesh.indicesNum;
	if(mesh.is2D)
		CopyMesh(mesh);
	else
		CopyLine(mesh);
}

MeshConstructor::~MeshConstructor(void)
{
	if(ib)
		delete ib;
	for (unsigned int i = 0; i < vbs.size(); i++)
	{
		if(vbs[i])
			delete vbs[i];
	}
}

void MeshConstructor::InitLine(IndexedModel &model){
	
	int verticesNum = model.positions.size();
	indicesNum = model.indices.size();
	
	vao.Bind();

	for (int i = 0; i < 2; i++)
	{
		vbs.push_back(new VertexBuffer(model.GetData(i),verticesNum*sizeof(model.positions[0])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	
	ib = new IndexBuffer((unsigned int*)model.GetData(5),indicesNum);
	
	vao.Unbind();
	is2D = false;
	
}

void MeshConstructor::InitMesh(IndexedModel &model)
{
	positions.clear();
	//for (int i = 0; i < model.positions.size(); i++)
		//positions.emplace_back(model.positions.at(i));
	positions = model.positions;
	if (kind != Kind::Default && kind != Kind::Bubble)
		CreateTree(model.positions);

	int verticesNum = model.positions.size();
	indicesNum = model.indices.size();
	
	vao.Bind();
	int i = 0;
	for (; i < VEC3_ATTRIB_NUM; i++)
	{
		vbs.push_back(new VertexBuffer(model.GetData(i),verticesNum*sizeof(model.positions[0])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	for(;i < VEC2_ATTRIB_NUM + VEC3_ATTRIB_NUM;i++)
	{
		vbs.push_back(new VertexBuffer(model.GetData(i),verticesNum*sizeof(model.texCoords[0])));	
		vao.AddBuffer(*vbs.back(),i,2,GL_FLOAT);
	}

	ib = new IndexBuffer((unsigned int*)model.GetData(5),indicesNum);
	
	vao.Unbind();
	is2D = true;
	
}

void MeshConstructor::CopyLine(const MeshConstructor &mesh)
{
	vao.Bind();

	for (int i = 0; i < 2; i++)
	{
		vbs.push_back(new VertexBuffer(*(mesh.vbs[i])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	
	ib = new IndexBuffer(*mesh.ib);
	
	vao.Unbind();

	is2D = false;
	
}

void MeshConstructor::CopyMesh(const MeshConstructor &mesh)
{
	if (kind != Kind::Default && kind != Kind::Bubble)
		CreateTree(mesh.positions);

	vao.Bind();
	int i = 0;
	for (; i < VEC3_ATTRIB_NUM; i++)
	{
		vbs.push_back(new VertexBuffer(*(mesh.vbs[i])));	
		vao.AddBuffer(*vbs.back(),i,3,GL_FLOAT);
	}
	for(;i < VEC2_ATTRIB_NUM + VEC3_ATTRIB_NUM;i++)
	{
		vbs.push_back(new VertexBuffer(*(mesh.vbs[i])));	
		vao.AddBuffer(*vbs.back(),i,2,GL_FLOAT);
	}
	
	ib = new IndexBuffer(*mesh.ib);
	//ib = mesh.ib;
	vao.Unbind();

	is2D = true;
	
}

void MeshConstructor::CreateTree(std::vector<glm::vec3> positions)
{
	std::list<glm::vec4> points;
	for (int i = 0; i < positions.size(); i++)
		points.push_back(glm::vec4(positions.at(i), 1));
	kdtree.makeTree(points);
	this->bvh = *CreateBVH(positions, kdtree.getRoot(), 0);
}

BVH* MeshConstructor::CreateBVH(std::vector<glm::vec3> points, Node* curr_node, int level)
{
	BVH* bvh = new BVH();
	int curr_cut = level % 3;
	glm::vec3 center = glm::vec3(0);
	glm::vec3 size = glm::vec3(0);
	glm::vec3 sum = glm::vec3(0);
	glm::vec3 max = glm::vec3(0);

	//Calculates the center of the box:
	for (int i = 0; i < points.size(); i++)
		sum += points[i];
	center = (1.0f / points.size()) * sum;
	//Calculates the size of the box:
	for (int i = 0; i < points.size(); i++)
		size = glm::max(size, glm::abs(points[i] - center));

	bvh->SetBoundingBox(center, size);
	bvh->GetBox()->SetNumOfPoints(points.size());

	std::vector<glm::vec3> new_points;
	if (curr_node->left != nullptr && points.size() >= MINIMUM_VERTCIES_FOR_BVH)
	{
		for (int i = 0; i < points.size(); i++)
		{
			if (points[i][curr_cut] <= curr_node->data[curr_cut])
			{
				new_points.push_back(points[i]);
			}
		}
		bvh->SetLeft(CreateBVH(new_points, curr_node->left, level + 1));
	}
	new_points.clear();
	if (curr_node->right != nullptr && points.size() >= MINIMUM_VERTCIES_FOR_BVH)
	{
		for (int i = 0; i < points.size(); i++)
		{
			if (points[i][curr_cut] > curr_node->data[curr_cut])
			{
				new_points.push_back(points[i]);
			}
		}
		bvh->SetRight(CreateBVH(new_points, curr_node->right, level + 1));
	}
	return bvh;
}

BVH* MeshConstructor::GetBVH()
{
	return &bvh;
}

int MeshConstructor::GetKind()
{
	return kind;
}

void MeshConstructor::SetKind(int kind)
{
	this->kind = kind;
}

// Checks collision between two bvh using BB CheckCollision
BoundingBox* MeshConstructor::CollisionDetection(MeshConstructor* other, glm::mat4 this_trans, glm::mat4 this_rot,
	glm::mat4 other_trans, glm::mat4 other_rot)
{
	//First Checks if the big boxes collides:
	std::vector<std::pair<BVH*, BVH*>> queue;
	BVH* this_curr = &this->bvh;
	BVH* other_curr = other->GetBVH();

	queue.emplace_back(this_curr, other_curr);
	while (!queue.empty())
	{
		this_curr = queue.back().first;
		other_curr = queue.back().second;
		queue.pop_back();

		if (this_curr != nullptr && other_curr != nullptr)
		{
			this_curr->GetBox()->UpdateDynamicVectors(this_trans, this_rot);
			other_curr->GetBox()->UpdateDynamicVectors(other_trans, other_rot);
			if (this_curr->GetBox()->CheckCollision(other_curr->GetBox()))
			{
				if (this_curr->IsSmallestBox() && other_curr->IsSmallestBox())
				{
					//std::cout << "this_level: " << this_curr->GetLevel() << std::endl;
					//std::cout << "other_curr: " << other_curr->GetLevel() << std::endl;
					//std::cout << "They collide! " << std::endl;
					return this_curr->GetBox();
				}
				//Pushes children boxes into queue
				if (this_curr->IsSmallestBox() && other_curr != nullptr)
				{
					queue.emplace_back(this_curr, other_curr->GetLeft());
					queue.emplace_back(this_curr, other_curr->GetRight());
				}
				else if (other_curr->IsSmallestBox() && this_curr != nullptr)
				{
					queue.emplace_back(this_curr->GetLeft(), other_curr);
					queue.emplace_back(this_curr->GetRight(), other_curr);
				}
				else
				{
					queue.emplace_back(this_curr->GetLeft(), other_curr->GetLeft());
					queue.emplace_back(this_curr->GetLeft(), other_curr->GetRight());
					queue.emplace_back(this_curr->GetRight(), other_curr->GetLeft());
					queue.emplace_back(this_curr->GetRight(), other_curr->GetRight());
				}
			}
		}
	}
	return nullptr;
}