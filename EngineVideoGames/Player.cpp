#include "Player.h"

#define BASIC_SHADER 1
#define LBS_SHADER 2

Player::Player(Scene* Scn, int Head_index, int Num_of_links)
{
	scn = Scn;
	head_index = Head_index;
	num_of_links = Num_of_links;
	is_alive = true;

	CreatePlayer();
}

Player::~Player() {}

void Player::CreatePlayer()
{
	/* Create the snake: */
	scn->addShapeCopy(7, -1, Scene::QUADS); //20 Add copy of head (7)
	scn->SetNumOfShape();
	head_index = scn->GetSizeOfShapes();
	scn->SetPickedShape(head_index);
	scn->shapeTransformation(Scene::xGlobalRotate, 170.0f);	//in order to put the eyes in place
	float mul = 2.0;
	for (int i = head_index; i < head_index + num_of_links; i++)
	{
		scn->addShapeCopy(19, -1, Scene::QUADS); //22 Add copy of cylinder (19) for body1
		scn->SetNumOfShape();
		scn->SetPickedShape(scn->GetSizeOfShapes());
		scn->shapeTransformation(Scene::xGlobalTranslate, mul);
		mul += 2.0;
		//shapeTransformation(Scene::xLocalTranslate, 1.8);
	}

	/*
	scn->addShapeCopy(19, -1, Scene::QUADS); //24 Add copy of cylinder (19) for body2
	scn->SetNumOfShape();
	scn->SetPickedShape(scn->GetSizeOfShapes());
	//shapeTransformation(Scene::xGlobalTranslate, 4.4 / 2);
	scn->shapeTransformation(Scene::xGlobalTranslate, 4.0);
	//shapeTransformation(Scene::xLocalTranslate, 1.8);

	scn->addShapeCopy(19, -1, Scene::QUADS); //26 Add copy of cylinder (19) for body3
	scn->SetNumOfShape();
	scn->SetPickedShape(scn->GetSizeOfShapes());
	//shapeTransformation(Scene::xGlobalTranslate, 6.6 / 3);
	scn->shapeTransformation(Scene::xGlobalTranslate, 6.0);
	//shapeTransformation(Scene::xLocalTranslate, 1.8);
	*/

	scn->addShapeCopy(13, -1, Scene::QUADS); //28 Add copy of tail (13)
	scn->SetNumOfShape();
	scn->SetPickedShape(scn->GetSizeOfShapes());
	//shapeTransformation(Scene::xGlobalTranslate, 8.8 / 4);
	scn->shapeTransformation(Scene::xGlobalTranslate, 8.0);
	//shapeTransformation(Scene::xLocalTranslate, 1.8);
	

	//Set the parents to connect the snake:
	/*
	setParent(num_of_head, -1);
	setParent(num_of_axis_body1, num_of_head);
	setParent(num_of_body1, num_of_head);
	setParent(num_of_axis_body2, num_of_body1);
	setParent(num_of_body2, num_of_body1);
	setParent(num_of_axis_body3, num_of_body2);
	setParent(num_of_body3, num_of_body2);
	setParent(num_of_axis_tail, num_of_body3);
	setParent(num_of_tail, num_of_body3);
	*/

	//Set snake shader - works with LBSUpdate:
	for (int i = head_index; i < head_index + num_of_links; i++)
		scn->SetShapeShader(i, BASIC_SHADER);

	//Set snake texture:
	scn->SetShapeTex(head_index, 1);
	int i = 0;
	for (i = head_index + 1; i < head_index + num_of_links - 1; i++)
		scn->SetShapeTex(i, 0);
	scn->SetShapeTex(i, 0);
}

void Player::MoveRight()
{

}

void Player::MoveLeft()
{

}

void Player::MoveUp()
{

}

void Player::MoveDown()
{

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