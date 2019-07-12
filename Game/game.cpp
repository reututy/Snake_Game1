#include "game.h"
#include <iostream>
#include <SDL.h>

#define CONTROL_POINT_SCALE 0.1
#define WATER_PLANE_SCALE 800
#define SPEED -0.03
#define BASIC_SHADER 1
#define LBS_SHADER 2
#define CYCLE 20
#define ANGLE 10.0f

bool once = false;
int cycle = 0;
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play

void my_audio_callback(void *userdata, Uint8 *stream, int len);

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

void Game::addShape(int type, int parent, unsigned int mode, Bezier1D* curve)
{
	chainParents.push_back(parent);
	if (type != BezierLine && type != BezierSurface)
		shapes.push_back(new Shape(type, mode));
	else
	{
		if (type == BezierLine)
			shapes.push_back(new Shape(curve, 30, 30, false, mode));
		else
			shapes.push_back(new Shape(curve, 30, 30, true, mode));
	}
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{

	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}

void Game::addBasicShapes()
{
	addShape(Axis, -1, LINES, nullptr); //0 Add Axis
	SetNumOfShape();
	//HideShape(0);
	addShape(BezierLine, -1, LINE_STRIP, head); //1 Add curve
	SetNumOfShape();
	//Translate all scene away from camera
	myTranslate(glm::vec3(0, 0, -20), 0);

	//Axis scale:
	pickedShape = 0;
	shapeTransformation(yScale, 20);
	shapeTransformation(xScale, 20);
	shapeTransformation(zScale, 20);

	addShape(Cube, -1, TRIANGLES, nullptr); //2 Add Cube for copying
	SetNumOfShape();
	pickedShape = 2;
	HideShape(pickedShape);

	num_of_shapes = MIN_CTRL;
	num_of_shapes = CreateCurveControlPoints(num_of_shapes, head);
	MAX_CTRL = num_of_shapes;

	//create 3d of head to copy:
	addShape(Scene::shapes::BezierSurface, -1, QUADS, head); //7 Add head to copy
	SetNumOfShape();
	for (int i = MIN_CTRL - 2; i < MAX_CTRL; i++)
	{
		if (i != 2 && i != 10)
			HideShape(i);
	}

	//create 3d of a tail to copy:
	addShape(BezierLine, -1, LINE_STRIP, tail); //8 Add curve
	SetNumOfShape();
	num_of_shapes = CreateCurveControlPoints(9, tail); //returns 13

	addShape(Scene::shapes::BezierSurface, -1, QUADS, tail); // 13 Add tail to copy
	SetNumOfShape();
	for (int i = 8; i < num_of_shapes + 1; i++)
	{
		if (i != 13)
			HideShape(i);
	}

	//create 3d of a cylinder to copy:
	addShape(BezierLine, -1, LINE_STRIP, body1); //14 Add curve 
	SetNumOfShape();
	num_of_shapes = CreateCurveControlPoints(15, body1); //returns 19

	addShape(Scene::shapes::BezierSurface, -1, QUADS, body1); // 19 Add cylinder to copy 
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
	addShapeCopy(2, -1, TRIANGLES); //20 Add copy cube = front
	SetNumOfShape();
	pickedShape = 20;
	shapeTransformation(xScale, 0.1);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(yGlobalRotate, 180);
	shapeTransformation(xGlobalTranslate, -WATER_PLANE_SCALE / 0.1);
	num_of_front_cube = 20;

	addShapeCopy(2, -1, TRIANGLES); //21 Add copy cube = back
	SetNumOfShape();
	pickedShape = 21;
	shapeTransformation(xScale, 0.1);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(xGlobalTranslate, WATER_PLANE_SCALE / 0.1);
	shapeTransformation(xGlobalRotate, 180);
	num_of_back_cube = 21;

	addShapeCopy(2, -1, TRIANGLES); //22 Add copy cube = up
	SetNumOfShape();
	pickedShape = 22;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, 0.1);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(yGlobalTranslate, WATER_PLANE_SCALE / 0.1);
	num_of_up_cube = 22;

	addShapeCopy(2, -1, TRIANGLES); //23 Add copy cube = down
	SetNumOfShape();
	pickedShape = 23;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, 0.1);
	shapeTransformation(zScale, WATER_PLANE_SCALE);
	shapeTransformation(yGlobalTranslate, -WATER_PLANE_SCALE / 0.1);
	num_of_down_cube = 23;

	addShapeCopy(2, -1, TRIANGLES); //24 Add copy cube = right
	SetNumOfShape();
	pickedShape = 24;
	shapeTransformation(xScale, WATER_PLANE_SCALE);
	shapeTransformation(yScale, WATER_PLANE_SCALE);
	shapeTransformation(zScale, 0.1);
	shapeTransformation(zGlobalTranslate, -WATER_PLANE_SCALE / 0.1);
	shapeTransformation(xGlobalRotate, 180);
	shapeTransformation(zGlobalRotate, 180);
	num_of_right_cube = 24;

	addShapeCopy(2, -1, TRIANGLES); //25 Add copy cube = right
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
	addShapeCopy(2, -1, TRIANGLES); //26 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 26;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 50);
	shapeTransformation(yScale, 50);
	shapeTransformation(zScale, 50);
	shapeTransformation(xGlobalTranslate, 13);
	//shapeTransformation(yGlobalTranslate, -10 / 2);
	shapeTransformation(zGlobalTranslate, 5);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES); //27 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 27;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 50);
	shapeTransformation(yScale, 50);
	shapeTransformation(zScale, 100);
	shapeTransformation(xGlobalTranslate, 5);
	shapeTransformation(yGlobalTranslate, -2);
	//shapeTransformation(zGlobalTranslate, 5);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES); //28 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 28;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 100);
	shapeTransformation(yScale, 50);
	shapeTransformation(zScale, 50);
	//shapeTransformation(xGlobalTranslate, -8);
	shapeTransformation(yGlobalTranslate, -5);
	shapeTransformation(zGlobalTranslate, -5);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES); //29 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 29;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 100);
	shapeTransformation(yScale, 50);
	shapeTransformation(zScale, 50);
	//shapeTransformation(xGlobalTranslate, -13);
	//shapeTransformation(yGlobalTranslate, -10 / 2);
	shapeTransformation(zGlobalTranslate, 5);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES); //30 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 30;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 50);
	shapeTransformation(yScale, 50);
	shapeTransformation(zScale, 100);
	shapeTransformation(xGlobalTranslate, -10);
	shapeTransformation(yGlobalTranslate, -2);
	shapeTransformation(zGlobalTranslate, -5);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShapeCopy(2, -1, TRIANGLES); //31 Add copy cube = Obstacle 1
	SetNumOfShape();
	pickedShape = 31;
	SetShapeTex(pickedShape, 6);
	shapeTransformation(xScale, 50);
	shapeTransformation(yScale, 50);
	shapeTransformation(zScale, 100);
	shapeTransformation(xGlobalTranslate, -10);
	shapeTransformation(yGlobalTranslate, -8);
	shapeTransformation(zGlobalTranslate, 3);
	SetShapeShader(pickedShape, BASIC_SHADER);
}

void Game::addRewards()
{
	//Rewards:
	addShape(Octahedron, -1, TRIANGLES, nullptr); //32 Add an Octahedron = Reward 1
	SetNumOfShape();
	pickedShape = 32;
	SetShapeTex(pickedShape, 7);
	shapeTransformation(xGlobalTranslate, 730);
	SetShapeShader(pickedShape, BASIC_SHADER);

	addShape(Octahedron, -1, TRIANGLES, nullptr); //33 Add an Octahedron = Reward 1
	SetNumOfShape();
	pickedShape = 33;
	SetShapeTex(pickedShape, 7);
	shapeTransformation(xGlobalTranslate, 730);
	shapeTransformation(zGlobalTranslate, 100);
	SetShapeShader(pickedShape, BASIC_SHADER);
}

void Game::Init()
{
	addBasicShapes();
	addBoundryBoxes();
	addObstacles();
	addRewards();

	//Create the snake:
	snake = new Player((Scene*) this, GetSizeOfShapes() + 1, 3);

	playTune("Sounds/Jump.wav");

	//Activate();

	pickedShape = -1;

	/* An example: */
	/*
	plane2D = new Shape(Plane,TRIANGLES);
	plane2D->SetShader(2);
	
	addShape(Axis,-1,LINES);
	shapes[0]->Hide();
	addShapeFromFile("../res/objs/monkey3.obj",-1,TRIANGLES);
	
	addShape(Cube,-1,TRIANGLES);
	
	
	//translate all scene away from camera
	myTranslate(glm::vec3(0,0,-5),0);

	pickedShape = 1;
	shapeTransformation(xGlobalTranslate,2);
	//ScaleAllDirections(2);
	SetShapeTex(1,0);
	SetShapeTex(2,1);
	
	ReadPixel();
	pickedShape = -1;
	//Activate();
	*/
}

void Game::Update(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, const int shaderIndx)
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
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, 0.0f, 0.0f);

	if (shaderIndx == 0) //picking shader
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else if (shaderIndx == 2) //skinning shader
		s->SetUniform4f("lightColor", 0.1f, 0.7f, 0.9f, 1.0f);
	else //other shader
		s->SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 1.0f);
	s->Unbind();
}

void Game::UpdateLBS(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::mat4 jointTransforms[5], int linksNum, const int shaderIndx)
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
	s->SetUniformMat4fv("jointTransforms", jointTransforms, 5);
	s->SetUniform1i("linksNum", linksNum);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, 0.0f, 0.0f);

	if (shaderIndx == 0) //picking shader
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else if (shaderIndx == 2) //skinning shader
		s->SetUniform4f("lightColor", 0.1f, 0.7f, 0.9f, 1.0f);
	else //other shader
		s->SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 1.0f);
	s->Unbind();
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
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(zLocalRotate, -ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(SPEED, 0, 0)));
					pickedShape++;
					shapeTransformation(zLocalRotate, ANGLE);
					cycle = 0;
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(zLocalRotate, -ANGLE);
					pickedShape++;
					shapeTransformation(zLocalRotate, ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, snake->GetDirection().z);

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
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(zLocalRotate, ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(SPEED, 0, 0)));
					pickedShape++;
					shapeTransformation(zLocalRotate, -ANGLE);
					cycle = 0;
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(zLocalRotate, ANGLE);
					pickedShape++;
					shapeTransformation(zLocalRotate, -ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, snake->GetDirection().z);

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
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(yLocalRotate, -ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(SPEED, 0, 0)));
					pickedShape++;
					shapeTransformation(yLocalRotate, ANGLE);
					cycle = 0;
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(yLocalRotate, -ANGLE);
					pickedShape++;
					shapeTransformation(yLocalRotate, ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, snake->GetDirection().z);

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
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(yLocalRotate, ANGLE);
					snake->SetDirection(GetVectorInSystem(pickedShape, glm::vec3(SPEED, 0, 0)));
					pickedShape++;
					shapeTransformation(yLocalRotate, -ANGLE);
					cycle = 0;
				}
				else if (cycle == CYCLE) //not tail and not head
				{
					shapeTransformation(xLocalTranslate, snake->GetDirection().x);
					shapeTransformation(yLocalTranslate, snake->GetDirection().y);
					shapeTransformation(zLocalTranslate, snake->GetDirection().z);

					shapeTransformation(yLocalRotate, ANGLE);
					pickedShape++;
					shapeTransformation(yLocalRotate, -ANGLE);
					cycle = 0;
				}
			}
			else if (cycle == CYCLE) //if tail
			{
				shapeTransformation(xLocalTranslate, snake->GetDirection().x);
				shapeTransformation(yLocalTranslate, snake->GetDirection().y);
				shapeTransformation(zLocalTranslate, snake->GetDirection().z);

				shapeTransformation(yLocalRotate, ANGLE);
				snake->SetMoveLeft(false);
			}
		}
		shapeTransformation(xLocalTranslate, snake->GetDirection().x);
		shapeTransformation(yLocalTranslate, snake->GetDirection().y);
		shapeTransformation(zLocalTranslate, snake->GetDirection().z);
		cycle++;
	}
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
			addShapeCopy(2, -1, TRIANGLES);
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

void Game::SkinningUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::vec4 dqRot[5], glm::vec4 dqTrans[5], const int shaderIndx, int index)
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
	s->SetUniform4vArr5("dqRot", dqRot, shaderIndx);
	s->SetUniform4vArr5("dqTrans", dqTrans, shaderIndx);
	s->SetUniform1i("index", index);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);
	/*
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else
		s->SetUniform4f("lightColor", 0.1f, 0.8f, 0.7f, 1.0f);
	*/
	if (shaderIndx == 0) //picking shader
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	//else if (shaderIndx == 2) //skinning shader
		//s->SetUniform4f("lightColor", 0.1f, 0.7f, 0.9f, 1.0f);
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

void Game::playTune(char* str) 
{

	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		return;

	//local variables
	static Uint32 wav_length; // length of our sample
	static Uint8 *wav_buffer; // buffer containing our audio file
	static SDL_AudioSpec wav_spec; // the specs of our piece of music



	if (SDL_LoadWAV(str, &wav_spec, &wav_buffer, &wav_length) == NULL) {
		return;
	}
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length

							/* Open the audio device */
	if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

	/* Start playing */
	SDL_PauseAudio(0);

	// wait until we're don't playing
	while (audio_len > 0) {
		SDL_Delay(100);
	}

	// shut everything down
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);

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

Player* Game::GetPlayer()
{
	return snake;
}