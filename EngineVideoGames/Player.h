#pragma once
#include <vector>
#include "scene.h"

class Scene;

class Player
{
	private:
		Scene* scn;
		int head_index;
		int num_of_links;
		glm::vec3 direction;
		bool play;

		bool move_right;
		bool move_left;
		bool move_up;
		bool move_down;

	public:
		enum Direction { forwords, backwords, up, down, right, left };
		Player(Scene* Scn, int Head_index, int Num_of_links);
		~Player();
		void CreatePlayer();
		void AlignSegments(int not_moving_shape, int moving_shape); //Credits: Almog Dubin
		glm::mat4 GetSegRot(int indx); //Credits: Almog Dubin
		glm::mat4 GetRotMat(glm::vec3 not_moving_shape_vector, glm::vec3 moving_vector); //Credits: Almog Dubin

		void MoveRight();
		void MoveLeft();
		void MoveUp();
		void MoveDown();

		int GetHeadIndex();
		int GetTailIndex();
		int GetNumOfLinks();
		glm::vec3 GetHeadPos();

		bool GetMoveRight();
		bool GetMoveLeft();
		bool GetMoveUp();
		bool GetMoveDown();

		void SetMoveRight(bool value);
		void SetMoveLeft(bool value);
		void SetMoveUp(bool value);
		void SetMoveDown(bool value);

		glm::vec3 GetDirection();
		void SetDirection(glm::vec3 dir);

		bool GetPlay();
		void SetPlay(bool value);

};