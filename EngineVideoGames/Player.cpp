#include "Player.h"
#include <iostream>

#define BASIC_SHADER 1
#define LBS_SHADER 2

Player::Player(Scene* Scn, int Head_index, int Num_of_links)
{
	scn = Scn;
	head_index = Head_index;
	num_of_links = Num_of_links;
	is_alive = true;
	velocity = glm::vec3(-1, 0, 0);
	CreatePlayer();
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
			//scn->setParent(i, -1);
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

void Player::Move(int direction)
{
	switch (direction)
	{
		case forwords:
			break;
		case right:
			velocity = glm::vec3(glm::rotate(5.0f, glm::vec3(0, -1, 0))*glm::vec4(velocity, 0));
			//std::cout << velocity.x << " " << velocity.y << " " << velocity.z << " " << std::endl;
			break;
		case left:
			velocity = glm::vec3(glm::rotate(5.0f, glm::vec3(0, 1, 0))*glm::vec4(velocity, 0));
			scn->SetPickedShape(head_index);
			break;
	}
	scn->SetPickedShape(head_index);
	scn->shapeTransformation(scn->xGlobalTranslate, 0.01*velocity.x);
	//scn->shapeTransformation(scn->yGlobalTranslate, 0.01*velocity.y);
	//scn->shapeTransformation(scn->zGlobalTranslate, 0.01*velocity.z);
	//scn->SetPickedShape(-1);
}

void Player::MoveRight()
{
	float t = 1.0;
	for (int i = head_index; i < head_index + num_of_links + 2; i++)
	{
		scn->shapeRotation(glm::vec3(0, -1, 0), t*5.0f, i);
		t -= 0.1;
	}
}

void Player::MoveLeft()
{
	float t = 1.0;
	
	for (int i = head_index; i < head_index + num_of_links + 2; i++)
	{
		scn->shapeRotation(glm::vec3(0, 1, 0), t*5.0f, i);
		t -= 0.1;
	}
}

void Player::MoveUp()
{
	float t = 1.0;
	
	for (int i = head_index; i < head_index + num_of_links + 2; i++)
	{
		scn->shapeRotation(glm::vec3(0, 0, -1), t*5.0f, i);
		t -= 0.1;
	}
}

void Player::MoveDown()
{
	float t = 1.0;
	
	for (int i = head_index; i < head_index + num_of_links + 2; i++)
	{
		scn->shapeRotation(glm::vec3(0, 0, -1), t*(-5.0f), i);
		t -= 0.1;
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