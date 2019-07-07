#pragma once
#include <vector>
#include "scene.h"

class Player
{
	private:
		Scene* scn;
		int head_index;
		int num_of_links;
		bool is_alive;
		glm::vec3 velocity;

		int curr_num_of_shape;

	public:
		enum Direction { forwords, backwords, up, down, right, left };
		Player(Scene* Scn, int Head_index, int Num_of_links);
		~Player();
		void CreatePlayer();

		void MoveRight();
		void MoveLeft();
		void MoveUp();
		void MoveDown();

		int GetHeadIndex();
		int GetTailIndex();
		int GetNumOfLinks();
		glm::vec3 GetHeadPos();


};