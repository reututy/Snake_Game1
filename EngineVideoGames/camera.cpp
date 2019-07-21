#include "camera.h"

Camera::Camera(const glm::vec3& pos, const glm::vec3& forward, float fov, float zNear, float zFar, const Viewport &view)
{
	this->pos = pos;
	this->forward = forward; //remove
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->projection = glm::perspective(fov, GetWHRelation(), zNear, zFar);
	this->projection = this->projection * glm::lookAt(pos, pos + forward, up); //remove
	this->fov = fov;
	this->near = zNear;
	this->far = zFar;
	this->vp = view;
}

void Camera::SetProjection(float zNear, float zFar, Viewport &view)
{
	this->vp = view;
	this->projection = glm::perspective(fov, view.GetWHRelation(), zNear, zFar)* glm::lookAt(pos, pos + forward, up);
	this->projection = glm::perspective(fov, view.GetWHRelation(), zNear, zFar);
	this->near = zNear;
	this->far = zFar;
}

void Camera::Pitch(float angle)
{
	glm::vec3 right = glm::normalize(glm::cross(up, forward));

	forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));
	up = glm::normalize(glm::cross(forward, right));
}

void Camera::RotateY(float angle)
{
	static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

	glm::mat4 rotation = glm::rotate(angle, UP);

	forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
	up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
}

int Camera::GetWidth()
{
	return vp.GetWidth();
}

int Camera::GetHeight()
{
	return vp.GetHeight();
}

int Camera::GetLeft()
{
	return vp.GetLeft();
}

int Camera::GetBottom()
{
	return vp.GetBottom();
}

glm::vec3 Camera::GetPos()
{
	return pos;
}

void Camera::SetPos(glm::vec3 position)
{
	pos = position;
}


/* Reut's adding: */
//void Camera::SetPlayer(Shape* player)
//{
//this->player = player;
//}

void Camera::Move()
{
	float horizontalDist = CalcHorizontalDist();
	float verticlDist = CalcVerticalDist();
	CalcCameraPosition(horizontalDist, verticlDist);
}

void Camera::MoveForward(float amt)
{
	pos += forward * amt;
}

void Camera::MoveRight(float amt)
{
	pos += glm::cross(up, forward) * amt;
}

float Camera::CalcHorizontalDist()
{
	return distance_from_player * cos(pitch);
}

float Camera::CalcVerticalDist()
{
	return distance_from_player * sin(pitch);
}

void Camera::CalcCameraPosition(float horizDist, float verticDist)
{
	float theta = angle_around_player;
	float offsetX = (float)horizDist * sin(theta);
	float offsetZ = (float)horizDist * cos(theta);
	//pos.x = player->GetMesh()->GetModel()->positions[0].x - offsetX;
	//pos.z = player->GetMesh()->GetModel()->positions[0].z - offsetZ;
	//pos.y = player->GetMesh()->GetModel()->positions[0].y + verticDist;

}

//void Camera::SetShape(Shape *num_of_shape)
//{
//player = num_of_shape;
//}