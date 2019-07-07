#include "Player.h"
#include <iostream>

#define BASIC_SHADER 1
#define LBS_SHADER 2

int R_cycle = 1;
int L_cycle = 1;

Player::Player(Scene* Scn, int Head_index, int Num_of_links)
{
	scn = Scn;
	head_index = Head_index;
	num_of_links = Num_of_links;
	is_alive = true;
	velocity = glm::vec3(-1, 0, 0);
	CreatePlayer();

	curr_num_of_shape = head_index;
}

Player::~Player() {}

void Player::CreatePlayer()
{
	//Create the snake: 
	float mone = 2.0;
	float mechane = 1.0;
	int i = 0;
	
	for (i = head_index; i < head_index + num_of_links + 2; i++)
	{
		if (i == head_index)
		{
			scn->addShapeCopy(7, -1, Scene::QUADS); //Add copy of head (7)
			scn->SetNumOfShape();
			scn->SetPickedShape(head_index);
			scn->shapeTransformation(Scene::xGlobalRotate, 170.0f);	//in order to put the eyes in place
			scn->setParent(i, -1);
			scn->SetShapeTex(head_index, 1);
		}
		else if (i == head_index + num_of_links + 1)
		{
			scn->addShapeCopy(13, -1, Scene::QUADS); //Add copy of tail
			scn->SetNumOfShape();
			scn->SetPickedShape(scn->GetSizeOfShapes());
			scn->shapeTransformation(Scene::xGlobalTranslate, mone);
			//scn->shapeTransformation(Scene::xGlobalTranslate, mone / mechane);
			//scn->setParent(i, i - 1);
			scn->SetShapeTex(i, 0);
		}
		else
		{
			scn->addShapeCopy(19, -1, Scene::QUADS); //Add copy of cylinder (19) for body
			scn->SetNumOfShape();
			scn->SetPickedShape(scn->GetSizeOfShapes());
			scn->shapeTransformation(Scene::xGlobalTranslate, mone);
			//scn->shapeTransformation(Scene::xGlobalTranslate, mone / mechane);
			//scn->setParent(i, i - 1);
			scn->SetShapeTex(i, 0);
			mone += 2.0;
			mechane += 2.0;
		}
		//Set snake shader - works with LBSUpdate:
		scn->SetShapeShader(i, BASIC_SHADER);
	}
}

void Player::MoveRight()
{
	/*
	if (curr_num_of_shape < head_index + num_of_links + 2)
	{
		scn->shapeRotation(glm::vec3(0, -1, 0), 5.0f, curr_num_of_shape);
		curr_num_of_shape++;
	}
	else if (curr_num_of_shape == head_index + num_of_links + 1)
		curr_num_of_shape = head_index;
		*/

	
	float t = 1.0;
	float slow = 0.2;
	int i = 0;
	for (i = head_index; i < head_index + num_of_links + 2; i++)
	{
		//if (R_cycle <= 10)
		//{
			scn->shapeRotation(glm::vec3(0, -1, 0), t*5.0f, i);
			t = abs(t - slow);
			slow = slow / 10.0;
			//if (i < head_index + num_of_links + 1)
			//{
				//scn->SetPickedShape(i + 1);
				//scn->shapeTransformation(Scene::xLocalTranslate, -0.0001);
			//}
		//}
		//if (i == head_index + 1 && R_cycle == 10)
			//R_cycle = 1;
	}
	//R_cycle++;
	//if (L_cycle > 0 && L_cycle <= 10)
		//L_cycle--;
		
}

void Player::MoveLeft()
{
	/*
	if (curr_num_of_shape < head_index + num_of_links + 2)
	{
		scn->shapeRotation(glm::vec3(0, -1, 0), -5.0f, curr_num_of_shape);
		curr_num_of_shape++;
	}
	else if (curr_num_of_shape == head_index + num_of_links + 1)
		curr_num_of_shape = head_index;
		*/
	/*
	float t = 1.0;
	float slow = 0.2;
	int i = 0;
	for (i = head_index; i < head_index + num_of_links + 2; i++)
	{
		if (L_cycle <= 10)
		{
			scn->shapeRotation(glm::vec3(0, -1, 0), t*(-5.0f), i);
			t = abs(t - slow);
			slow = slow / 10.0;
		}
		//if (i == head_index + 1 && R_cycle == 10)
			//L_cycle = 1;
	}
	L_cycle++;
	if (R_cycle > 0 && R_cycle <= 10)
		R_cycle--;
		*/
}

void Player::MoveUp()
{
	float t = 1.0;
	float slow = 0.8;
	for (int i = head_index; i < head_index + num_of_links + 2; i++)
	{
		scn->shapeRotation(glm::vec3(0, 0, -1), t*5.0f, i);
		t = abs(t - slow);
		slow = slow / 10.0;
	}
}

void Player::MoveDown()
{
	float t = 1.0;
	float slow = 0.8;
	for (int i = head_index; i < head_index + num_of_links + 2; i++)
	{
		scn->shapeRotation(glm::vec3(0, 0, -1), t*(-5.0f), i);
		t = abs(t - slow);
		slow = slow / 10.0;
	}
}

int Player::GetHeadIndex()
{
	return head_index;
}

int Player::GetTailIndex()
{
	return 0; //TODO
}

int Player::GetNumOfLinks()
{
	return num_of_links; 
}

glm::vec3 Player::GetHeadPos()
{
	return glm::vec3(0,0,0); //TODO
}