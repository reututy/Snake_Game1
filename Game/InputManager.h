#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"
#include <iostream>

#define NO_OF_MODES 8
int curr_mode = Scene::modes::QUADS;
char modes_names[8][20] = { "POINTS", "LINES", "LINE_LOOP", "LINE_STRIP", "TRIANGLES", "TRIANGLE_STRIP", "TRIANGLE_FAN", "QUADS" };
int camera_mode = 0;
enum camera_mode {free_view, up_view, player_view};

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
				scn->Activate();
				scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
				scn->GetPlayer()->SetMoveRight(true);
			break;
			case GLFW_KEY_RIGHT:
				if (camera_mode == camera_mode::free_view)
					scn->globalSystemRot(-5, glm::vec3(0, 1, 0), -1);
				else if (camera_mode == camera_mode::up_view)
				{
					scn->SetPickedShape(scn->GetPlayer()->GetHeadIndex());
					scn->GetPlayer()->SetMoveRight(true);
					
					//scn->GetPlayer()->MoveRight();
					//scn->SetPickedShape(28);
					//scn->shapeRotation(glm::vec3(0, -1, 0), 5.0f, 28);
					//scn->GetPlayer()->Move(scn->GetPlayer()->right);
					//scn->GetPlayer()->MoveRight();
					//scn->shapeTransformation(Scene::yLocalRotate, -90);
					//scn->SetPickedShape(-1);
				}
				else if (camera_mode == camera_mode::player_view)
				{ }
				break;
			case GLFW_KEY_LEFT:
				if (camera_mode == camera_mode::free_view)
					scn->globalSystemRot(5, glm::vec3(0, 1, 0), -1);
				else if (camera_mode == camera_mode::up_view)
				{
					scn->GetPlayer()->MoveLeft();
					//scn->SetPickedShape(28);
					//scn->shapeRotation(glm::vec3(0, 1, 0), 5.0f, 28);
					//scn->GetPlayer()->Move(scn->GetPlayer()->left);
					//scn->GetPlayer()->MoveLeft();
					//scn->shapeTransformation(Scene::yLocalRotate, 90);
					//scn->SetPickedShape(-1);
				}
				else if (camera_mode == camera_mode::player_view)
				{}
				break;
			case GLFW_KEY_UP:
				if (camera_mode == camera_mode::free_view)
					scn->globalSystemRot(-5, glm::vec3(0, 0, 1), 1);
					//scn->translateInSystem(*scn->GetShape(20), glm::vec3(0,0,1), 20, 0);
				else if (camera_mode == camera_mode::up_view)
				{
					scn->GetPlayer()->MoveUp();
					//scn->SetPickedShape(28);
					//scn->shapeRotation(glm::vec3(0, 0, -1), 5.0f, 28);
					//scn->shapeTransformation(Scene::zLocalRotate, -90);
					//scn->SetPickedShape(-1);
				}
				else if (camera_mode == camera_mode::player_view)
				{}
				break;
			case GLFW_KEY_DOWN:
				if (camera_mode == camera_mode::free_view)
					scn->globalSystemRot(5, glm::vec3(0, 0, 1), 1);
				else if (camera_mode == camera_mode::up_view)
				{
					scn->GetPlayer()->MoveDown();
					//scn->SetPickedShape(28);
					//scn->shapeRotation(glm::vec3(0, 0, 1), 5.0f, 28);
					//scn->shapeTransformation(Scene::zLocalRotate, 90);
					//scn->SetPickedShape(-1);
				}
				else if (camera_mode == camera_mode::player_view)
				{
				}
				break;
			case GLFW_KEY_M:
				curr_mode = (curr_mode + 1) % NO_OF_MODES;
				std::cout << "Current mode is: " << modes_names[curr_mode] << std::endl;
				//TODO for loop on the snake body
				//scn->ChangeShapeMode(scn->GetNumOfHead(), curr_mode);
				break;
			case GLFW_KEY_C:
				if (camera_mode == camera_mode::free_view)
					camera_mode = camera_mode::up_view;
				else if (camera_mode == camera_mode::up_view)
					camera_mode = camera_mode::player_view;
				else if (camera_mode == camera_mode::player_view)
					camera_mode = camera_mode::free_view;
				/*
				if (!camera_mode)
					camera_mode = true;
				else
					camera_mode = false;
					*/
				break;
			case GLFW_KEY_D:
				scn->SetDirection();
				break;
			case GLFW_KEY_U: //moves the shape up
				scn->shapeTransformation(scn->yGlobalTranslate, 0.1f);
				break;
			case GLFW_KEY_J: //moves the shape down
				scn->shapeTransformation(scn->yGlobalTranslate, -0.1f);
				break;
			case GLFW_KEY_K: //moves the shape right
				scn->shapeTransformation(scn->xGlobalTranslate, 0.1f);
				break;
			case GLFW_KEY_H: //moves the shape left
				scn->shapeTransformation(scn->xGlobalTranslate, -0.1f);
				break;
			case GLFW_KEY_S: //Zoom in
				scn->shapeTransformation(scn->zCameraTranslate, 5.0f);
				break;
			case GLFW_KEY_W: //Zoom out
				scn->shapeTransformation(scn->zCameraTranslate, -5.0f);
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
	if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		scn->mouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
	}
	else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		scn->mouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
	}

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	Game *scn = (Game*)glfwGetWindowUserPointer(window);
	scn->resize(width,height);
}

void init(Display &display)
{
	display.addKeyCallBack(key_callback);
	display.addMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
	display.addResizeCallBack(window_size_callback);
}
