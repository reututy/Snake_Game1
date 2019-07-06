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
		//velocity?

	public:
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