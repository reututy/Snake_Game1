#include "game.h"
#include <iostream>

#define CONTROL_POINT_SCALE 0.1
#define WATER_PLANE_SCALE 180
#define SPEED 0.05
#define BASIC_SHADER 1
#define LBS_SHADER 2
#define CYCLE 25
#define BUBBLE_CYCLE 100
#define ANGLE 10.0f
#define BALL_SCALE 0.05
#define BUBBLE_SCALE 0.02
#define REWARD_TEX 7
#define BUBBLE_TEX 8

bool once = false;
int cycle = 0;
int bubble_cycle = 0;
bool bubble_up = true;
bool bubble_sound = true;
int bubble_kind = 0;

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game::Game() : Scene()
{
	head = nullptr;
	body1 = nullptr;
}

Game::Game(glm::vec3 position,float angle,float near, float far,Viewport &vp) : Scene(position,angle,near,far,vp)
{ 
	std::vector<glm::mat4> ctrlPointsVec;
	float x = 1.0;

	glm::mat4 mat = glm::mat4(glm::vec4(0.0 - x, 0.0, 0.0, 1.0), glm::vec4(0.2 - x, 0.7, 0.0, 1.0),
		glm::vec4(1.5 - x, 1.0, 0.0, 1.0), glm::vec4(2.0 - x, 1.0, 0.0, 1.0));

	ctrlPointsVec.push_back(mat);
	head = new Bezier1D(ctrlPointsVec);
	MIN_CTRL = 3;
	MAX_CTRL = 3;
	ctrlPointsVec.clear();

	mat = glm::mat4(glm::vec4(0.0 - x, 1.0, 0.0, 1.0), glm::vec4(0.5 - x, 1.0, 0.0, 1.0),
		glm::vec4(1.0 - x, 1.0, 0.0, 1.0), glm::vec4(2.0 - x, 1.0, 0.0, 1.0));
	ctrlPointsVec.push_back(mat);
	body1 = new Bezier1D(ctrlPointsVec);
	MIN_CTRL = 3;
	MAX_CTRL = 3;
	ctrlPointsVec.clear();

	mat = glm::mat4(glm::vec4(0.0 - x, 1.0, 0.0, 1.0), glm::vec4(0.5 - x, 1.0, 0.0, 1.0),
		glm::vec4(1.7 - x, 0.7, 0.0, 1.0), glm::vec4(2.0 - x, 0.0, 0.0, 1.0));
	ctrlPointsVec.push_back(mat);
	tail = new Bezier1D(ctrlPointsVec);
	MIN_CTRL = 3;
	MAX_CTRL = 3;
	ctrlPointsVec.clear();
}

Game::~Game(void)
{
	delete head;
	delete body1;
}

void Game::addShape(int type, int parent, unsigned int mode, Bezier1D* curve, int kind)
{
	chainParents.push_back(parent);
	if (type != BezierLine && type != BezierSurface)
		shapes.push_back(new Shape(type, mode, kind));
	else
	{
		if (type == BezierLine)
			shapes.push_back(new Shape(curve, 30, 30, false, mode, kind));
		else
			shapes.push_back(new Shape(curve, 30, 30, true, mode, kind));
	}
}

void Game::addBasicShapes()
{
	addShape(Axis, -1, LINES, nullptr, MeshConstructor::Kind::Default); //0 Add Axis
	SetNumOfShape();
	//HideShape(0);
	addShape(BezierLine, -1, LINE_STRIP, head, MeshConstructor::Kind::Default); //1 Add curve
	SetNumOfShape();
	//Translate all scene away from camera
	myTranslate(glm::vec3(0, 0, -20), 0);

	//Axis scale:
	pickedShape = 0;
	shapeTransformation(yScale, 20);
	shapeTransformation(xScale, 20);
	shapeTransformation(zScale, 20);

	addShape(Cube, -1, TRIANGLES, nullptr, MeshConstructor::Kind::Default); //2 Add Cube for copying
	SetNumOfShape();
	pickedShape = 2;
	HideShape(pickedShape);

	num_of_shapes = MIN_CTRL;
	num_of_shapes = CreateCurveControlPoints(num_of_shapes, head);
	MAX_CTRL = num_of_shapes;

	//create 3d of head to copy:
	addShape(Scene::shapes::BezierSurface, -1, QUADS, head, MeshConstructor::Kind::Default); //7 Add head to copy
	SetNumOfShape();
	for (int i = MIN_CTRL - 2; i < MAX_CTRL; i++)
	{
		if (i != 2 && i != 10)
			HideShape(i);
	}

	//create 3d of a tail to copy:
	addShape(BezierLine, -1, LINE_STRIP, tail, MeshConstructor::Kind::Default); //8 Add curve
	SetNumOfShape();
	num_of_shapes = CreateCurveControlPoints(9, tail); //returns 13

	addShape(Scene::shapes::BezierSurface, -1, QUADS, tail, MeshConstructor::Kind::Default); // 13 Add tail to copy
	SetNumOfShape();
	for (int i = 8; i < num_of_shapes + 1; i++)
	{
		if (i != 13)
			HideShape(i);
	}

	//create 3d of a cylinder to copy:
	addShape(BezierLine, -1, LINE_STRIP, body1, MeshConstructor::Kind::Default); //14 Add curve 
	SetNumOfShape();
	num_of_shapes = CreateCurveControlPoints(15, body1); //returns 19

	addShape(Scene::shapes::BezierSurface, -1, QUADS, body1, MeshConstructor::Kind::Default); // 19 Add cylinder to copy 
	SetNumOfShape();
	for (int i = 14; i < num_of_shapes + 1; i++)
	{
		if (i != 19)
			HideShape(i);
	}

	HideShape(7); //hides head to copy
	HideShape(13); //hides tail to copy
	HideShape(19); //hides cylinder to copy
}

void Game::addBoundryBoxes()
{
	//Added boundry Boxes of the game: 
	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //20 Add copy cube = front
	SetNumOfShape();
	pickedShape = 20;
	shapeTransformation(xScale, 0.1);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(yGlobalRotate, 180);
	shapeTransformation(xGlobalTranslate, -WATER_PLANE_SCALE / 0.1);
	num_of_front_cube = 20;

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //21 Add copy cube = back
	SetNumOfShape();
	pickedShape = 21;
	shapeTransformation(xScale, 0.1);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(xGlobalTranslate, WATER_PLANE_SCALE / 0.1);
	shapeTransformation(xGlobalRotate, 180);
	num_of_back_cube = 21;

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //22 Add copy cube = up
	SetNumOfShape();
	pickedShape = 22;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, 0.1);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(yGlobalTranslate, WATER_PLANE_SCALE / 0.1);
	num_of_up_cube = 22;

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //23 Add copy cube = down
	SetNumOfShape();
	pickedShape = 23;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, 0.1);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(yGlobalTranslate, -WATER_PLANE_SCALE / 0.1);
	num_of_down_cube = 23;

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //24 Add copy cube = right
	SetNumOfShape();
	pickedShape = 24;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, 0.1);
	shapeTransformation(zGlobalTranslate, -WATER_PLANE_SCALE / 0.1);
	shapeTransformation(xGlobalRotate, 180);
	shapeTransformation(zGlobalRotate, 180);
	num_of_right_cube = 24;

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //25 Add copy cube = right
	SetNumOfShape();
	pickedShape = 25;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, 0.1);
	shapeTransformation(zGlobalTranslate, WATER_PLANE_SCALE / 0.1);
	shapeTransformation(xGlobalRotate, 180);
	shapeTransformation(zGlobalRotate, 180);
	num_of_left_cube = 25;

	//Set the parents to connect the Boxes:
	/*
	setParent(num_of_front_cube, -1);
	setParent(num_of_back_cube, num_of_front_cube);
	setParent(num_of_up_cube, num_of_front_cube);
	setParent(num_of_down_cube, num_of_front_cube);
	setParent(num_of_right_cube, num_of_front_cube);
	setParent(num_of_left_cube, num_of_front_cube);
	*/

	//Set boxes textures:
	SetShapeTex(num_of_front_cube, 2);
	SetShapeTex(num_of_back_cube, 3);
	SetShapeTex(num_of_right_cube, 3);
	SetShapeTex(num_of_left_cube, 3);
	SetShapeTex(num_of_up_cube, 4);
	SetShapeTex(num_of_down_cube, 5);

	//Set Boxes shader - works with basicShader:
	SetShapeShader(num_of_front_cube, BASIC_SHADER);
	SetShapeShader(num_of_back_cube, BASIC_SHADER);
	SetShapeShader(num_of_up_cube, BASIC_SHADER);
	SetShapeShader(num_of_down_cube, BASIC_SHADER);
	SetShapeShader(num_of_right_cube, BASIC_SHADER);
	SetShapeShader(num_of_left_cube, BASIC_SHADER);
}

void Game::addObstacles()
{
	//Obstacles:
	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //26 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 26;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 2);
	shapeTransformation(yScale, 2);
	shapeTransformation(zScale, 2);
	shapeTransformation(xGlobalTranslate, 10);
	//shapeTransformation(yGlobalTranslate, -10 / 2);
	shapeTransformation(zGlobalTranslate, 8);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //27 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 27;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 2);
	shapeTransformation(yScale, 2);
	shapeTransformation(zScale, 5);
	shapeTransformation(xGlobalTranslate, 10);
	shapeTransformation(yGlobalTranslate, -2);
	//shapeTransformation(zGlobalTranslate, 5);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //28 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 28;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 5);
	shapeTransformation(yScale, 2);
	shapeTransformation(zScale, 2);
	//shapeTransformation(xGlobalTranslate, -8);
	shapeTransformation(yGlobalTranslate, -5);
	shapeTransformation(zGlobalTranslate, -8);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //29 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 29;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 5);
	shapeTransformation(yScale, 2);
	shapeTransformation(zScale, 2);
	//shapeTransformation(xGlobalTranslate, -13);
	//shapeTransformation(yGlobalTranslate, -10 / 2);
	shapeTransformation(zGlobalTranslate, 10);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //30 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 30;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 2);
	shapeTransformation(yScale, 2);
	shapeTransformation(zScale, 5);
	shapeTransformation(xGlobalTranslate, -10);
	shapeTransformation(yGlobalTranslate, -2);
	shapeTransformation(zGlobalTranslate, -3);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Obstacle); //31 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 31;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 2);
	shapeTransformation(yScale, 2);
	shapeTransformation(zScale, 5);
	shapeTransformation(xGlobalTranslate, -10);
	shapeTransformation(yGlobalTranslate, -8);
	shapeTransformation(zGlobalTranslate, 3);
	SetShapeShader(pickedShape, BASIC_SHADER);
}

void Game::addRewards()
{
	//Rewards:
	/* balls:
	addShapeFromFile("../res/objs/ball.obj", -1, TRIANGLES, MeshConstructor::Kind::Reward, BALL_SCALE); //32
	SetNumOfShape();
	pickedShape = 32;
	SetShapeTex(pickedShape, 7);
	shapeTransformation(xGlobalTranslate, -30);
	//shapeTransformation(zGlobalTranslate, -20);
	//shapeTransformation(xGlobalTranslate, -10);
	//shapeTransformation(zGlobalTranslate, -1);
	SetShapeShader(pickedShape, BASIC_SHADER);
	*/
	
	addShape(Octahedron, -1, TRIANGLES, nullptr, MeshConstructor::Kind::Reward); //32 Add an Octahedron for copy = Reward 1
	SetNumOfShape();
	pickedShape = 32;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(xGlobalTranslate, -5);
	//shapeTransformation(xGlobalTranslate, 730);
	SetShapeShader(pickedShape, BASIC_SHADER);
	
	addShapeCopy(32, -1, TRIANGLES, MeshConstructor::Kind::Reward); //33 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 33;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(xGlobalTranslate, -15);
	shapeTransformation(yGlobalTranslate, -15);
	shapeTransformation(zGlobalTranslate, 13);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(32, -1, TRIANGLES, MeshConstructor::Kind::Reward); //34 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 34;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(xGlobalTranslate, 8);
	shapeTransformation(zGlobalTranslate, 20);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(32, -1, TRIANGLES, MeshConstructor::Kind::Reward); //35 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 35;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(yGlobalTranslate, -10);
	shapeTransformation(zGlobalTranslate, -22);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(32, -1, TRIANGLES, MeshConstructor::Kind::Reward); //36 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 36;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(xGlobalTranslate, -20);
	shapeTransformation(yGlobalTranslate, -4);
	shapeTransformation(zGlobalTranslate, -22);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(32, -1, TRIANGLES, MeshConstructor::Kind::Reward); //37 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 37;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(xGlobalTranslate, 20);
	shapeTransformation(zGlobalTranslate, 12);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(32, -1, TRIANGLES, MeshConstructor::Kind::Reward); //38 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 38;
	SetShapeTex(pickedShape, REWARD_TEX);
	shapeTransformation(xGlobalTranslate, 20);
	shapeTransformation(yGlobalTranslate, -4);
	shapeTransformation(zGlobalTranslate, -8);
	SetShapeShader(pickedShape, BASIC_SHADER);
	
}

void Game::AddBubbles()
{
	/*
	int X = 3000;
	int Z = 3000;

	//Center bubbles:
	addShapeFromFile("../res/objs/ball.obj", -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble, BUBBLE_SCALE);	//bubble ball
	SetNumOfShape();
	pickedShape = 39;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 100);
	shapeTransformation(yGlobalTranslate, -795);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 40;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120);
	shapeTransformation(yGlobalTranslate, -850);
	shapeTransformation(xGlobalTranslate, -50);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 41;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120);
	shapeTransformation(yGlobalTranslate, -792);
	shapeTransformation(xGlobalTranslate, -100);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 42;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120);
	shapeTransformation(yGlobalTranslate, -950);
	shapeTransformation(xGlobalTranslate, -60);
	SetShapeShader(pickedShape, BASIC_SHADER);
	
	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 43;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130);
	shapeTransformation(yGlobalTranslate, -980);
	shapeTransformation(xGlobalTranslate, -110);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 44;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130);
	shapeTransformation(yGlobalTranslate, -960);
	shapeTransformation(xGlobalTranslate, -30);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 45;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130);
	shapeTransformation(yGlobalTranslate, -1020);
	shapeTransformation(xGlobalTranslate, -50);
	SetShapeShader(pickedShape, BASIC_SHADER);
	*/
	//Left bubbles:
	/*
	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 46;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 100 + Z);
	shapeTransformation(yGlobalTranslate, -795);
	shapeTransformation(xGlobalTranslate, -X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 47;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120 + Z);
	shapeTransformation(yGlobalTranslate, -850);
	shapeTransformation(xGlobalTranslate, -50 - X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 48;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120 + Z);
	shapeTransformation(yGlobalTranslate, -792);
	shapeTransformation(xGlobalTranslate, -100 - X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 49;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120 + Z);
	shapeTransformation(yGlobalTranslate, -950);
	shapeTransformation(xGlobalTranslate, -60 - X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 50;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130 + Z);
	shapeTransformation(yGlobalTranslate, -980);
	shapeTransformation(xGlobalTranslate, -110 - X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 51;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130 + Z);
	shapeTransformation(yGlobalTranslate, -960);
	shapeTransformation(xGlobalTranslate, -30 - X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 52;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130 + Z);
	shapeTransformation(yGlobalTranslate, -1020);
	shapeTransformation(xGlobalTranslate, -50 - X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	//Down right bubbles:
	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 53;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 100 - Z);
	shapeTransformation(yGlobalTranslate, -795);
	shapeTransformation(xGlobalTranslate, X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 54;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120 - Z);
	shapeTransformation(yGlobalTranslate, -850);
	shapeTransformation(xGlobalTranslate, -50 + X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 55;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120 - Z);
	shapeTransformation(yGlobalTranslate, -792);
	shapeTransformation(xGlobalTranslate, -100 + X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 56;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 120 - Z);
	shapeTransformation(yGlobalTranslate, -950);
	shapeTransformation(xGlobalTranslate, -60 + X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 57;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130 - Z);
	shapeTransformation(yGlobalTranslate, -980);
	shapeTransformation(xGlobalTranslate, -110 + X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 58;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130 - Z);
	shapeTransformation(yGlobalTranslate, -960);
	shapeTransformation(xGlobalTranslate, -30 + X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();

	addShapeCopy(39, -1, TRIANGLE_STRIP, MeshConstructor::Kind::Bubble);
	SetNumOfShape();
	pickedShape = 59;
	SetShapeTex(pickedShape, BUBBLE_TEX);
	shapeTransformation(xScale, 0.0085);
	shapeTransformation(yScale, 0.0085);
	shapeTransformation(zScale, 0.0085);
	shapeTransformation(zGlobalTranslate, 130 - Z);
	shapeTransformation(yGlobalTranslate, -1020);
	shapeTransformation(xGlobalTranslate, -50 + X);
	SetShapeShader(pickedShape, BASIC_SHADER);
	shapes[pickedShape]->Hide();
	*/
}

void Game::Init()
{
	addBasicShapes();
	addBoundryBoxes();
	addObstacles();
	addRewards();
	AddBubbles();
	
	//Create the snake:
	snake = new Player((Scene*) this, GetSizeOfShapes() + 1, 3);

	/*
	for (int i = 0; i < shapes.size(); i++)
	{
		if (shapes[i]->GetMesh()->GetKind() != MeshConstructor::Kind::Bubble && 
			shapes[i]->GetMesh()->GetKind() != MeshConstructor::Kind::Default && 
			(shapes[i]->GetMode() == TRIANGLES || shapes[i]->GetMode() == Scene::QUADS))
		{
			CreateBoundingBoxes(shapes[i]->GetMesh()->GetBVH(), i, 0);
		}
	}*/

	//playTune("Sounds/Jump.wav");

	pickedShape = -1;
	//start_time = 
}

void Game::CreateBoundingBoxes(BVH* bvh, int parent, int level)
{
	addShapeCopy(2, -1, LINE_LOOP, MeshConstructor::Kind::Default);
	pickedShape = shapes.size() - 1;
	bvh->GetBox()->SetNumOfShape(pickedShape);
	bvh->SetLevel(level);
	
	shapeTransformation(xLocalTranslate, bvh->GetBox()->GetFixedCenter().x);
	shapeTransformation(yLocalTranslate, bvh->GetBox()->GetFixedCenter().y);
	shapeTransformation(zLocalTranslate, bvh->GetBox()->GetFixedCenter().z);

	shapeTransformation(xScale, bvh->GetBox()->GetSize().x);
	shapeTransformation(yScale, bvh->GetBox()->GetSize().y);
	shapeTransformation(zScale, bvh->GetBox()->GetSize().z);

	//Hides all the shapes unless the large boxes
	shapes[pickedShape]->Hide();
	chainParents[pickedShape] = parent;
	if (level == 0)
		shapes[pickedShape]->Unhide();

	//if (level == 8)
	//shapes[pickedShape]->Unhide();
	if (bvh->GetLeft() != nullptr)
		CreateBoundingBoxes(bvh->GetLeft(), parent, level + 1);
	if (bvh->GetRight() != nullptr)
		CreateBoundingBoxes(bvh->GetRight(), parent, level + 1);
}

void Game::Update(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 Camera, glm::mat4 &Normal, const int shaderIndx)
{
	int prev_shape = pickedShape;
	if (!once)
		MoveControlCubes();
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV, shaderIndx);
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("Camera", Camera, shaderIndx);
	s->SetUniformMat4f("Normal", Normal, shaderIndx);
	s->SetUniform4f("lightDirection", 0.0f, -1.0f, -1.0f, 0.0f);

	if (shaderIndx == 0) //picking shader
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else //other shader
		s->SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 1.0f);
	s->Unbind();
	if (snake->GetPlay() == true)
		CheckCollisionDetection(snake->GetHeadIndex());
}

void Game::UpdateLBS(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Camera, 
	const glm::mat4 &Normal, glm::mat4 jointTransforms[5], int linksNum, int index, const int shaderIndx)
{
	int prev_shape = pickedShape;
	if (!once)
		MoveControlCubes();
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV, shaderIndx);
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("Camera", Camera, shaderIndx);
	s->SetUniformMat4f("Normal", Normal, shaderIndx);
	s->SetUniformMat4fv("jointTransforms", jointTransforms, 5);
	s->SetUniform1i("linksNum", linksNum);
	s->SetUniform1i("index", index);
	s->SetUniform4f("lightDirection", 0.0f, -1.0f, -1.0f, 0.0f);

	if (shaderIndx == 0) //picking shader
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else //other shader
		s->SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 1.0f);
	s->Unbind();
	if (snake->GetPlay() == false)
		CheckCollisionDetection(snake->GetHeadIndex());
}

void Game::WhenRotate()
{
	if (pickedShape >= 0)
		GetShapeTransformation();
}

void Game::WhenTranslate()
{
	bool no_preservation = !(pickedShape <= 4 || pickedShape >= MAX_CTRL - 2);
	if (pickedShape >= 3 && pickedShape < MAX_CTRL) //Make sure that it only happens in the case of the cubes
	{
		glm::vec4 trans_vec = GetShapeTransformation()*glm::vec4(0, 0, 0, 1);
		//move the relevent control point
		head->MoveControlPoint((pickedShape - MIN_CTRL), (pickedShape - MIN_CTRL), no_preservation, trans_vec);
		//move the relevent cube incident to that specific control point
		shapes[1]->GetMesh()->InitLine(head->GetLine(30));
	}
}

void Game::Motion()
{
	if (isActive)
	{
		if (snake->GetMoveUp())
		{
			if (pickedShape != snake->GetTailIndex())
			{
				if (pickedShape == snake->GetHeadIndex())
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(zLocalRotate, -ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(-1, 0, 0)));
					glm::vec3 tip_pos_before = snake->GetHeadPos();
					pickedShape++;
					shapeTransformation(zLocalRotate, ANGLE);
					glm::vec3 tip_pos_after = snake->GetHeadPos();
					glm::vec3 tip_pos_diff = glm::vec3(tip_pos_before.x - tip_pos_after.x, tip_pos_before.y - tip_pos_after.y, tip_pos_before.z - tip_pos_after.z);
					//std::cout << "tip_pos_before: " << tip_pos_before.x << " " << tip_pos_before.y << " " << tip_pos_before.z << std::endl;
					//std::cout << "tip_pos_after: " << tip_pos_after.x << " " << tip_pos_after.y << " " << tip_pos_after.z << std::endl;
					//std::cout << "tip_pos_diff: " << tip_pos_diff.x << " " << tip_pos_diff.y << " " << tip_pos_diff.z << std::endl;
					//shapeTransformation(xLocalTranslate, tip_pos_diff.x);
					shapeTransformation(yLocalTranslate, tip_pos_diff.y);
					//shapeTransformation(zLocalTranslate, tip_pos_diff.z);
					cycle = 0;
					//playTune("Sounds/swim.wav");
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(zLocalRotate, -ANGLE);
					glm::vec3 tip_pos_before = GetTipPositionInSystem(pickedShape);
					pickedShape++;
					shapeTransformation(zLocalRotate, ANGLE);
					glm::vec3 tip_pos_after = GetTipPositionInSystem(pickedShape - 1);
					glm::vec3 tip_pos_diff = glm::vec3(tip_pos_before.x - tip_pos_after.x, tip_pos_before.y - tip_pos_after.y, tip_pos_before.z - tip_pos_after.z);
					shapeTransformation(zLocalTranslate, tip_pos_diff.y);
					//std::cout << "NOT HEAD: " << std::endl;
					//std::cout << "tip_pos_before: " << tip_pos_before.x << " " << tip_pos_before.y << " " << tip_pos_before.z << std::endl;
					//std::cout << "tip_pos_after: " << tip_pos_after.x << " " << tip_pos_after.y << " " << tip_pos_after.z << std::endl;
					//std::cout << "tip_pos_diff: " << tip_pos_diff.x << " " << tip_pos_diff.y << " " << tip_pos_diff.z << std::endl;
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

				shapeTransformation(zLocalRotate, -ANGLE);
				snake->SetMoveUp(false);
			}
		}
		else if (snake->GetMoveDown())
		{
			if (pickedShape != snake->GetTailIndex())
			{
				if (pickedShape == snake->GetHeadIndex())
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(zLocalRotate, ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(-1, 0, 0)));
					pickedShape++;
					shapeTransformation(zLocalRotate, -ANGLE);
					cycle = 0;

				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(zLocalRotate, ANGLE);
					pickedShape++;
					shapeTransformation(zLocalRotate, -ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

				shapeTransformation(zLocalRotate, ANGLE);
				snake->SetMoveDown(false);
			}
		}
		else if (snake->GetMoveRight())
		{
			if (pickedShape != snake->GetTailIndex())
			{
				if (pickedShape == snake->GetHeadIndex())
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(yLocalRotate, -ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(-1, 0, 0)));
					pickedShape++;
					shapeTransformation(yLocalRotate, ANGLE);
					cycle = 0;
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(yLocalRotate, -ANGLE);
					pickedShape++;
					shapeTransformation(yLocalRotate, ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

				shapeTransformation(yLocalRotate, -ANGLE);
				snake->SetMoveRight(false);
			}
		}
		else if (snake->GetMoveLeft())
		{
			if (pickedShape != snake->GetTailIndex())
			{
				if (pickedShape == snake->GetHeadIndex())
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(yLocalRotate, ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(-1, 0, 0)));
					pickedShape++;
					shapeTransformation(yLocalRotate, -ANGLE);
					cycle = 0;
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

					shapeTransformation(yLocalRotate, ANGLE);
					pickedShape++;
					shapeTransformation(yLocalRotate, -ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);

				shapeTransformation(yLocalRotate, ANGLE);
				snake->SetMoveLeft(false);
			}
		}
		shapeTransformation(xLocalTranslate, SPEED*snake->GetDirection().x);
		shapeTransformation(yLocalTranslate, SPEED*snake->GetDirection().y);
		shapeTransformation(zLocalTranslate, SPEED*snake->GetDirection().z);
		cycle++;
	}

	//Bubble truble:
	/*
	if (bubble_up == true) //bubbles goes up
	{
		if (bubble_sound == true)
		{
			playTune("Sounds/blop.wav");
			bubble_sound = false;
		}
		if (bubble_kind % 3 == 0)
		{
			for (int i = 39; i < 46; i++)
			{
				pickedShape = i;
				shapes[i]->Unhide();
				shapeTransformation(yGlobalTranslate, 0.2 * 90);
			}
		}
		else if (bubble_kind % 3 == 1)
		{
			for (int i = 46; i < 53; i++)
			{
				pickedShape = i;
				shapes[i]->Unhide();
				shapeTransformation(yGlobalTranslate, 0.2 * 90);
			}
		}
		else if (bubble_kind % 3 == 2)
		{
			for (int i = 52; i < 60; i++)
			{
				pickedShape = i;
				shapes[i]->Unhide();
				shapeTransformation(yGlobalTranslate, 0.2 * 90);
			}
		}
	}
	else //bubbles goes down
	{
		if (bubble_kind % 3 == 0)
		{
			for (int i = 39; i < 46; i++)
			{
				pickedShape = i;
				shapes[i]->Hide();
				shapeTransformation(yGlobalTranslate, 0.2 * -90);
			}
		}
		else if (bubble_kind % 3 == 1)
		{
			for (int i = 46; i < 53; i++)
			{
				pickedShape = i;
				shapes[i]->Hide();
				shapeTransformation(yGlobalTranslate, 0.2 * -90);
			}
		}
		else if (bubble_kind % 3 == 2)
		{
			for (int i = 52; i < 60; i++)
			{
				pickedShape = i;
				shapes[i]->Hide();
				shapeTransformation(yGlobalTranslate, 0.2 * -90);
			}
		}
	}
	bubble_cycle++;

	if (bubble_cycle == BUBBLE_CYCLE)
	{
		if (bubble_up == true)
			bubble_up = false;
		else
			bubble_up = true;
		bubble_sound = true; 
		bubble_cycle = 0;
		bubble_kind++;
	}
	if (bubble_kind == 3)
		bubble_kind = 0;
	*/
}

int Game::CreateCurveControlPoints(int counter, Bezier1D *curve)
{
	glm::vec3 control_point;
	int modulu = 0;
	for (int k = 0; k < curve->GetNumSegs(); k++)
	{
		modulu = 0;
		for (int i = 0; i < 4; i++)
		{
			addShapeCopy(2, -1, TRIANGLES, MeshConstructor::Kind::Default);
			pickedShape = counter++;
			control_point = *(curve->GetControlPoint(k, i)).GetPos();
			//scaling the cube
			shapeTransformation(xScale, CONTROL_POINT_SCALE);
			shapeTransformation(yScale, CONTROL_POINT_SCALE);
			shapeTransformation(zScale, CONTROL_POINT_SCALE);
			//move the cube to the control point
			if (modulu++ != 3 || (i == 3 && k == curve->GetNumSegs() - 1))
			{
				shapeTransformation(xGlobalTranslate, control_point.x / CONTROL_POINT_SCALE);
				shapeTransformation(yGlobalTranslate, control_point.y / CONTROL_POINT_SCALE);
				shapeTransformation(zGlobalTranslate, control_point.z / CONTROL_POINT_SCALE);
			}
			else
			{
				HideShape(pickedShape);
			}
		}
	}
	return counter;
}

void Game::MoveControlCubes()
{
	int old_picked_shape = pickedShape;
	glm::vec3 control_point;

	for (int i = MIN_CTRL; i < MAX_CTRL; i++)
	{
		if (!((i - 2) % 4) == 0 && i != MAX_CTRL - 1)
		{
			pickedShape = i;
			glm::vec4 curr_pos = GetShapeTransformation()*glm::vec4(0, 0, 0, 1);
			control_point = *(head->GetControlPoint((i - MIN_CTRL) / 4, (i - MIN_CTRL) % 4)).GetPos();
			control_point = (control_point)-glm::vec3(curr_pos);
			shapeTransformation(xGlobalTranslate, control_point.x);
			shapeTransformation(yGlobalTranslate, control_point.y);
			shapeTransformation(zGlobalTranslate, control_point.z);
		}
	}
	pickedShape = old_picked_shape;
}

void Game::SkinningUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, const glm::mat4 &Camera, glm::vec4 dqRot[5], glm::vec4 dqTrans[5], const int shaderIndx, int index)
{
	int prev_shape = pickedShape;
	if (!once)
		MoveControlCubes();
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV, shaderIndx);
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("Normal", Normal, shaderIndx);
	s->SetUniformMat4f("Camera", Camera, shaderIndx);
	s->SetUniform4vArr5("dqRot", dqRot, shaderIndx);
	s->SetUniform4vArr5("dqTrans", dqTrans, shaderIndx);
	s->SetUniform1i("index", index);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);

	if (shaderIndx == 0) //picking shader
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else //other shader
		s->SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 1.0f);
		
	s->Unbind();
}

void Game::LBSUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::mat4 jointTransforms[5], glm::ivec3 jointIndices, const int shaderIndx, int index)
{
	int prev_shape = pickedShape;
	if (!once) {
		MoveControlCubes();
	}
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV, shaderIndx);
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("Normal", Normal, shaderIndx);
	s->SetUniformMat4fv("jointTransforms", jointTransforms, 5);
	s->SetUniform3i("jointIndices", jointIndices.x, jointIndices.y, jointIndices.z);
	s->SetUniform1i("index", index);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else
		s->SetUniform4f("lightColor", 0.1f, 0.8f, 0.7f, 1.0f);
	s->Unbind();
}

int Game::GetMINCTRL()
{
	return MIN_CTRL;
}

int Game::GetMAXCTRL()
{
	return MAX_CTRL;
}

int Game::GetNumOfFrontBox()
{
	return num_of_front_cube;
}

int Game::GetNumOfBackBox()
{
	return num_of_back_cube;
}

int Game::GetNumOfUpBox()
{
	return num_of_up_cube;
}

int Game::GetNumOfDownBox()
{
	return num_of_down_cube;
}

int Game::GetNumOfRightBox()
{
	return num_of_right_cube;
}

int Game::GetNumOfLeftBox()
{
	return num_of_left_cube;
}