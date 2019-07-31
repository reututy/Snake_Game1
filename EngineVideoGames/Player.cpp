#include "Player.h"
#include <iostream>

#define BASIC_SHADER 1
#define LBS_SHADER 2
#define SKINING_SHADER 3
#define INTERPOLATION 40.0f

int R_cycle = 1;
int L_cycle = 1;

Player::Player(Scene* Scn, int Head_index, int Num_of_links)
{
	scn = Scn;
	head_index = Head_index;
	num_of_links = Num_of_links;
	CreatePlayer();

	move_right = false;
	move_left = false;
	move_up = false;
	move_down = false;
	play = false;
	direction = glm::vec3(-1, 0, 0);
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
			scn->addShapeCopy(7, -1, Scene::QUADS, MeshConstructor::Kind::Snake); //Add copy of head (7)
			scn->SetNumOfShape();
			scn->SetPickedShape(head_index);
			//scn->shapeTransformation(Scene::xGlobalRotate, 180.0f);	//in order to put the eyes in place
			scn->setParent(i, -1);
			scn->SetShapeTex(head_index, 0);
		}
		else if (i == head_index + num_of_links + 1)
		{
			scn->addShapeCopy(13, -1, Scene::QUADS, MeshConstructor::Kind::Default); //Add copy of tail
			scn->SetNumOfShape();
			scn->SetPickedShape(scn->GetSizeOfShapes());
			//scn->shapeTransformation(Scene::xGlobalRotate, 180.0f);	//in order to put the eyes in place
			scn->shapeTransformation(Scene::xGlobalTranslate, mone);
			scn->shapeTransformation(Scene::xLocalTranslate, mone);
			scn->setParent(i, i - 1);
			scn->SetShapeTex(i, 0);
		}
		else
		{
			scn->addShapeCopy(19, -1, Scene::QUADS, MeshConstructor::Kind::Default); //Add copy of cylinder (19) for body
			scn->SetNumOfShape();
			scn->SetPickedShape(scn->GetSizeOfShapes());
			//scn->shapeTransformation(Scene::xGlobalRotate, 180.0f);	//in order to put the eyes in place
			scn->shapeTransformation(Scene::xGlobalTranslate, mone);
			scn->shapeTransformation(Scene::xLocalTranslate, mone);
			scn->setParent(i, i - 1);
			scn->SetShapeTex(i, 0);
		}
		//Set snake shader - works with LBSUpdate:
		scn->SetShapeShader(i, LBS_SHADER);
	}
	//scn->SetPickedShape(head_index); //to add camera->maketrans of the camera
	//scn->shapeTransformation(Scene::xGlobalTranslate, 20);
}

//Credits: Almog Dubin
void Player::AlignSegments(int not_moving_shape, int moving_shape)
{
	glm::vec3 not_moving_shape_vector = glm::normalize(glm::vec3(GetSegRot(not_moving_shape)*glm::vec4(1, 0, 0, 0)));
	glm::vec3 moving_vector = glm::normalize(glm::vec3(GetSegRot(moving_shape)*glm::vec4(1, 0, 0, 0)));
	glm::mat4 R = GetRotMat(moving_vector, not_moving_shape_vector);
	scn->GetShape(moving_shape)->snakeRotate(R);
	scn->GetShape(moving_shape + 1)->snakeRotate(glm::transpose(R));
}

//Credits: Almog Dubin
glm::mat4 Player::GetSegRot(int indx)
{
	glm::mat4 rot = glm::mat4(1);
	for (int i = indx; i > head_index ; i--)
		rot *= scn->GetShape(i)->GetRot();
	return rot;
}

//Credits: Almog Dubin
glm::mat4 Player::GetRotMat(glm::vec3 not_moving_shape_vector, glm::vec3 moving_vector)
{
	not_moving_shape_vector = (INTERPOLATION*moving_vector + not_moving_shape_vector) / (INTERPOLATION + 1.0f);
	glm::vec3 v = glm::cross(moving_vector, not_moving_shape_vector);
	float c = glm::dot(moving_vector, not_moving_shape_vector);
	if (abs(1.0f + c) < 0.001)
	{
		return glm::mat4(1);
	}
	glm::mat4 vx = glm::mat4(
		glm::vec4(0, -v.z, v.y, 0),
		glm::vec4(v.z, 0, -v.x, 0),
		glm::vec4(-v.y, v.x, 0, 0),
		glm::vec4(0, 0, 0, 0));
	glm::mat4 unit = glm::mat4(1);
	glm::mat4 R = unit + vx + vx * vx*(1.0f / (1.0f + c));
	return R;
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
	return glm::vec3(scn->GetShape(head_index)->makeTransScale()[3]); 
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

bool Player::GetPlay()
{
	return play;
}

void Player::SetPlay(bool value)
{
	play = value;
}