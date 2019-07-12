#pragma once
#include "scene.h"
#include "bezier1D.h"
#include "Player.h"

class Game : public Scene
{

	Player* snake;

	Bezier1D *head;
	Bezier1D *body1;
	Bezier1D *tail;

	int num_of_front_cube;
	int num_of_back_cube;
	int num_of_up_cube;
	int num_of_down_cube;
	int num_of_right_cube;
	int num_of_left_cube;
	
	int MIN_CTRL;
	int MAX_CTRL;

public:

	Game(void);
	Game(glm::vec3 position, float angle, float near, float far, Viewport &vp);
	~Game(void);
	void Init();
	void addShape(int type, int parent, unsigned int mode, Bezier1D* curve);
	/* Reut's addings:*/
	void addBasicShapes(); 
	void addBoundryBoxes(); 
	void addObstacles(); 
	void addRewards();

	//	void Update( glm::mat4 MVP ,glm::mat4 *jointTransforms,const int length,const int  shaderIndx);
	void Update(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, const int shaderIndx);
	void UpdateLBS(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::mat4 jointTransforms[5], int linksNum, const int shaderIndx);
	void WhenRotate();
	void WhenTranslate();
	void Motion();

	/* Reut's addings:*/
	void SkinningUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::vec4 dqRot[5], glm::vec4 dqTrans[5], const int shaderIndx, int index);
	void LBSUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::mat4 jointTransforms[5], glm::ivec3 jointIndices, const int shaderIndx, int index);
	void playTune(char* str);

	int CreateCurveControlPoints(int counter, Bezier1D *curve);
	void MoveControlCubes();
	
	int GetMINCTRL();
	int GetMAXCTRL();

	int GetNumOfFrontBox();
	int GetNumOfBackBox();
	int GetNumOfUpBox();
	int GetNumOfDownBox();
	int GetNumOfRightBox();
	int GetNumOfLeftBox();

	Player* GetPlayer();
};

