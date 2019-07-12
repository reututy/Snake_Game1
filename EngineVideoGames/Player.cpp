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

	move_right = false;
	move_left = false;
	move_up = false;
	move_down = false;
	direction = glm::vec3(-0.001, 0, 0);
	curr_num_of_shape = head_index;
}

Player::~Player() {}

void Player::CreatePlayer()
{
	//Create the snake: 
	float mone = 1.0;
	int i = 0;
	
	for (i = head_index; i < head_index + num_of_links + 2; i++)
	{
		if (i == head_index)
		{
			scn->addShapeCopy(7, -1, Scene::QUADS); //Add copy of head (7)
			scn->SetNumOfShape();
			scn->SetPickedShape(head_index);
			scn->shapeTransformation(Scene::xGlobalRotate, 180.0f);	//in order to put the eyes in place
			scn->setParent(i, -1);
			scn->SetShapeTex(head_index, 1);
		}
		else if (i == head_index + num_of_links + 1)
		{
			scn->addShapeCopy(13, -1, Scene::QUADS); //Add copy of tail
			scn->SetNumOfShape();
			scn->SetPickedShape(scn->GetSizeOfShapes());
			scn->shapeTransformation(Scene::xGlobalRotate, 180.0f);	//in order to put the eyes in place
			scn->shapeTransformation(Scene::xGlobalTranslate, mone);
			scn->shapeTransformation(Scene::xLocalTranslate, mone);
			scn->setParent(i, i - 1);
			scn->SetShapeTex(i, 0);
		}
		else
		{
			scn->addShapeCopy(19, -1, Scene::QUADS); //Add copy of cylinder (19) for body
			scn->SetNumOfShape();
			scn->SetPickedShape(scn->GetSizeOfShapes());
			scn->shapeTransformation(Scene::xGlobalRotate, 180.0f);	//in order to put the eyes in place
			scn->shapeTransformation(Scene::xGlobalTranslate, mone);
			scn->shapeTransformation(Scene::xLocalTranslate, mone);
			scn->setParent(i, i - 1);
			scn->SetShapeTex(i, 0);
		}
		//Set snake shader - works with LBSUpdate:
		scn->SetShapeShader(i, BASIC_SHADER);
	}
	//scn->SetPickedShape(head_index);
	//scn->shapeTransformation(Scene::xGlobalTranslate, 750);
}

void Player::MoveRight()
{
	move_right = true;
}

void Player::MoveLeft()
{
	move_left = true;
}

void Player::MoveUp()
{
	move_up = true;
}

void Player::MoveDown()
{
	move_down = true;
}

int Player::GetHeadIndex()
{
	return head_index;
}

int Player::GetTailIndex()
{
	return head_index + num_of_links + 1; 
}

int Player::GetNumOfLinks()
{
	return num_of_links; 
}

glm::vec3 Player::GetHeadPos()
{
	return glm::vec3(0,0,0); //TODO
}

bool Player::GetMoveRight()
{
	return move_right;
}

bool Player::GetMoveLeft()
{
	return move_left;
}

bool Player::GetMoveUp()
{
	return move_up;
}

bool Player::GetMoveDown()
{
	return move_down;
}

void Player::SetMoveRight(bool value)
{
	move_right = value;
}

void Player::SetMoveLeft(bool value)
{
	move_left = value;
}

void Player::SetMoveUp(bool value)
{
	move_up = value;
}

void Player::SetMoveDown(bool value)
{
	move_down = value;
}

glm::vec3 Player::GetDirection()
{
	return direction;
}

void Player::SetDirection(glm::vec3 dir)
{
	direction = dir;
}