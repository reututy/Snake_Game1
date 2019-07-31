#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"
#include <iostream>
#include <chrono>
#include <thread>

#define NO_OF_MODES 8
int curr_mode = Scene::modes::QUADS;
char modes_names[8][20] = { "POINTS", "LINES", "LINE_LOOP", "LINE_STRIP", "TRIANGLES", "TRIANGLE_STRIP", "TRIANGLE_FAN", "QUADS" };
int twice = 1;
int play = 1;

void mouse_callback(GLFWwindow* window,int button, int action, int mods)
{	
	if(action == GLFW_PRESS )
	{
		Game *scn = (Game*)glfwGetWindowUserPointer(window);
		double x2,y2;
		glfwGetCursorPos(window,&x2,&y2);
		scn->picking((int)x2,(int)y2);
	}
}
	
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Game *scn = (Game*)glfwGetWindowUserPointer(window);
	scn->shapeTransformation(scn->zCameraTranslate,yoffset);
		
}
	
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game *scn = (Game*)glfwGetWindowUserPointer(window);

	if(action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			case GLFW_KEY_ESCAPE:			
				glfwSetWindowShouldClose(window,GLFW_TRUE);
			break;
			case GLFW_KEY_SPACE:
				if (scn->GetMenuView() == true)
				{

				}
				if (scn->IsActive() == false)
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->Activate();
				}
				else
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->Deactivate();
				}
			break;
			case GLFW_KEY_RIGHT:
				if (scn->GetPlayer()->GetPlay() == false) //camera moves
				{
					scn->GetCamera(0)->myRotate(-5, glm::vec3(0, 1, 0), -1);
					scn->GetCamera(2)->myRotate(-5, glm::vec3(0, 1, 0), -1);
				}
				else //player moves
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->GetPlayer()->SetMoveRight(true);
				}
				break;
			case GLFW_KEY_LEFT:
				if (scn->GetPlayer()->GetPlay() == false) //camera moves
				{
					scn->GetCamera(0)->myRotate(5, glm::vec3(0, 1, 0), -1);
					scn->GetCamera(2)->myRotate(5, glm::vec3(0, 1, 0), -1);
				}
				else //player moves
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->GetPlayer()->SetMoveLeft(true);
				}
				break;
			case GLFW_KEY_UP:
				if (scn->GetPlayer()->GetPlay() == false) //camera moves
				{
					scn->GetCamera(0)->myRotate(5, glm::vec3(1, 0, 0), -1);
					scn->GetCamera(2)->myRotate(5, glm::vec3(1, 0, 0), -1);
				}
					//scn->translateInSystem(*scn->GetShape(20), glm::vec3(0,0,1), 20, 0);
				else //player moves
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->GetPlayer()->SetMoveUp(true);
				}
				break;
			case GLFW_KEY_DOWN:
				if (scn->GetPlayer()->GetPlay() == false) //camera moves
				{
					scn->GetCamera(0)->myRotate(-5, glm::vec3(1, 0, 0), -1);
					scn->GetCamera(2)->myRotate(-5, glm::vec3(1, 0, 0), -1);
				}
				else //player moves
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->GetPlayer()->SetMoveDown(true);
				}
				break;
			case GLFW_KEY_M:
				curr_mode = (curr_mode + 1) % NO_OF_MODES;
				std::cout << "Current mode is: " << modes_names[curr_mode] << std::endl;
				//TODO for loop on the snake body
				//scn->ChangeShapeMode(scn->GetNumOfHead(), curr_mode);
				break;
			case GLFW_KEY_S: //Zoom in
				scn->SetPickedShape(-1);
				scn->shapeTransformation(scn->zCameraTranslate, 5.0f);
				break;
			case GLFW_KEY_W: //Zoom out
				scn->SetPickedShape(-1);
				scn->shapeTransformation(scn->zCameraTranslate, -5.0f);
				break;
			case GLFW_KEY_P: //Play
				if (scn->GetPlayer()->GetPlay() == false)
					scn->GetPlayer()->SetPlay(true);
				else if(scn->GetPlayer()->GetPlay() == true)
					scn->GetPlayer()->SetPlay(false);
				break;
			case GLFW_KEY_V: //Change Viewport
				if (scn->GetMainView() == false)
					scn->SetView();
				break;
			case GLFW_KEY_0: //Return to main menu Button
				if (scn->GetFinishView() == true)
				{
					twice = 0;
					play = 1;
					scn->SetFinishView();
					scn->SetPlayAgain();
					scn->SetShapeTex(40, 9);
					if (scn->GetMainView() == false)
						scn->SetMainView();
					if (scn->GetMenuView() == false)
						scn->SetMenuView();
					if (scn->GetAboutView() == true)
						scn->SetAboutView();
				}
				break;
			case GLFW_KEY_1: //Play Button
				if (scn->GetPlayAgain() == true)
				{
					for (int i = scn->GetPlayer()->GetHeadIndex(); i < scn->GetPlayer()->GetHeadIndex() + scn->GetPlayer()->GetNumOfLinks() + 2; i++)
					{
						scn->GetShape(scn->GetPlayer()->GetHeadIndex())->zeroTrans();
						scn->GetShape(scn->GetPlayer()->GetHeadIndex())->zeroRot(true);
					}
					for (int i = 0; i < scn->GetSizeOfShapes(); i++)
					{
						scn->GetShape(i)->Setfound(false);
						if (scn->GetShape(i)->GetMesh()->GetKind() == MeshConstructor::Kind::Reward)
							scn->GetShape(i)->Unhide();
					}
				}
				if (play == 1 && scn->GetMenuView() == true)
				{
					scn->SetShapeTex(40, 10);
					//std::this_thread::sleep_for(std::chrono::milliseconds(500));
					//std::cout << "Let's Play!";
					//scn->SetShapeTex(40, 9);
					if (scn->GetPlayAgain() == true)
						scn->SetPlayAgain();
				}
				else if (play == 2 && scn->GetMenuView() == true)
				{
					scn->SetShapeTex(40, 21);
				}
				else if (play == 3 && scn->GetMenuView() == true)
				{
					scn->SetShapeTex(40, 22);
				}
				else if (play == 4 && scn->GetMenuView() == true)
				{
					scn->SetMainView();
					scn->SetMenuView();
					play = 0;
				}
				play++;
				break;
			case GLFW_KEY_2: //About Button
				if (twice == 1 && scn->GetAboutView() == false)
				{
					scn->SetShapeTex(40, 11);
				}
				else if (twice == 2 && scn->GetAboutView() == false)
				{
					scn->SetShapeTex(40, 12);
					scn->SetAboutView();
					twice = 0;
				}
				twice++;
				break;
			case GLFW_KEY_3: //Quit Button
				if (twice == 1 && scn->GetQuitView() == false)
				{
					scn->SetShapeTex(40, 14);
				}
				else if (twice == 2 && scn->GetQuitView() == false)
				{
					scn->SetShapeTex(40, 9);
					scn->SetQuitView();
					twice = 0;
					glfwSetWindowShouldClose(window, GLFW_TRUE);
					//exit(0);
				}
				twice++;
				break;
			case GLFW_KEY_4: //Back Button
				if (twice == 1 && scn->GetAboutView() == true)
				{
					scn->SetShapeTex(40, 13);
				}
				else if (twice == 2 && scn->GetAboutView() == true)
				{
					scn->SetShapeTex(40, 9);
					scn->SetAboutView();
					twice = 0;
				}
				twice++;
				break;
		default:
			break;
		}
	}
}
	
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Game *scn = (Game*)glfwGetWindowUserPointer(window);

	scn->updatePosition((float)xpos,(float)ypos);
	/*
	if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		scn->mouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
	}
	else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		scn->mouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
	}
	*/
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	Game *scn = (Game*)glfwGetWindowUserPointer(window);
	scn->resize(width,height);
}

void init(Display &display)
{
	display.addKeyCallBack(key_callback);
	//display.addMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
	display.addResizeCallBack(window_size_callback);
}
