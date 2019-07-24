#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"
#include <iostream>

#define NO_OF_MODES 8
int curr_mode = Scene::modes::QUADS;
char modes_names[8][20] = { "POINTS", "LINES", "LINE_LOOP", "LINE_STRIP", "TRIANGLES", "TRIANGLE_STRIP", "TRIANGLE_FAN", "QUADS" };

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
				scn->SetView();
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
