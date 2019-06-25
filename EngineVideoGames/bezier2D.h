#pragma once
#include "Bezier1D.h"

class Bezier2D
{
	int circularSubdivision; //usualy 4 how many subdivision in circular direction
	Bezier1D main_curve;
	glm::vec3 axis;

public:
	Bezier2D(void);
	Bezier2D(Bezier1D &b, glm::vec3 axis, int circularSubdivision);
	~Bezier2D(void);
	IndexedModel GetSurface(int resT, int resS);						//generates model for rendering using MeshConstructor::initMeshs
	Vertex GetVertex(int segmentT, int segmentS, float t, float s);		//returns point on surface in the requested segments for value of t and s
	glm::vec3 GetNormal(int segmentT, int segmentS, float t, float s);		//returns point on surface in the requested segments for value of t and s
	glm::vec3 CalcWeight(int segmentT, int segmentS, float t, float s);
};