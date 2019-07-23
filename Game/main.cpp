#include "InputManager.h"
#include "glm\glm.hpp"

int main(int argc, char *argv[])
{
	const int DISPLAY_WIDTH = 1340;
	const int DISPLAY_HEIGHT = 690;
	const float zFar = 10000.0f;
	const float zNear = 1.0f;
	const float CAM_ANGLE = 60.0f;
	const float relation = (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT;
	Viewport vp1(401, 0, DISPLAY_WIDTH - 401, DISPLAY_HEIGHT);
	Viewport vp2(0, 265, 400, DISPLAY_HEIGHT - 265);
	Viewport vp3(0, 0, 400, 264);
	
	Game *scn = new Game(glm::vec3(0.0f, 0.0f, 1.0f), 60.0f, zNear, zFar, vp1); //free view camera
	scn->AddCamera(glm::vec3(0.0f, 30.0f, 0.5f), 90.0f, zNear, zFar, vp2);	//up view camera
	scn->AddCamera(glm::vec3(0.0f, 0.0f, 1.0f), CAM_ANGLE, zNear, zFar, vp3);	//snake view camera

	scn->globalSystemRot(-90, glm::vec3(0, 1, 0), -1);
	scn->shapeTransformation(scn->zCameraTranslate, 80.0f);

	scn->GetCamera(0)->myTranslate(glm::vec3(0, 0, 2), -1);
	scn->GetCamera(0)->myTranslate(glm::vec3(0, -2, 0), -1);
	//scn->GetCamera(0)->myRotate(3, glm::vec3(0, 1, 0), -1);

	scn->GetCamera(1)->myTranslate(glm::vec3(0, 0, 3), -1);

	scn->GetCamera(2)->myTranslate(glm::vec3(15, 0, 0), -1);
	scn->GetCamera(2)->myTranslate(glm::vec3(0, 0, 5), -1);
	scn->GetCamera(2)->myRotate(90, glm::vec3(0, 1, 0), -1);
	

	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	init(display);

	scn->Init();

	scn->addShader("../res/shaders/pickingShader");  //0
	//scn->addShader("../res/shaders/newPickingShader");
	//scn->addShader("../res/shaders/pickingShaderOld");
	scn->addShader("../res/shaders/basicShader"); //1
	//scn->addShader("../res/shaders/basicShaderOld");
	scn->addShader("../res/shaders/skinningShader"); //2
	//scn->addShader("../res/shaders/LBSShader"); //2
	scn->addShader("../res/shaders/basicTrans1"); //3 - LBS
	//scn->addShader("../res/shaders/basicShadertex");
	scn->addShader("../res/shaders/basicShader2D"); //4
	scn->addShader("../res/shaders/blueShader"); //5

	//snake textures:
	scn->AddTexture("../res/textures/snake1.png", false); //0
	scn->AddTexture("../res/textures/snake_eyes.png", false); //1
	//water textures:
	scn->AddTexture("../res/textures/cave.png", false); //2
	scn->AddTexture("../res/textures/water_front.png", false); //3
	scn->AddTexture("../res/textures/water_up.png", false); //4
	scn->AddTexture("../res/textures/water_ground.png", false); //5
	//Rewords and Obstacles textures:
	scn->AddTexture("../res/textures/rock.png", false); //6
	scn->AddTexture("../res/textures/gold_shine.png", false); //7
	
	/*
	scn->AddTexture("../res/textures/plane.png", true); //8
	scn->AddTexture(vp2.GetWidth(), vp1.GetHeight(), COLOR);
	scn->AddTexture(vp2.GetWidth(), vp1.GetHeight(), DEPTH);
	scn->AddBuffer(9, 0, COLOR);*/
	
	//scn->AddCamera(glm::vec3(0.0f, 30.0f, 0.5f), 90.0f, zNear, zFar, vp2);	//up view camera
	//scn->AddCamera(glm::vec3(4.0f, 0.0f, 1.0f), CAM_ANGLE, zNear, zFar, vp3);	//snake view camera
	
	display.setScene(scn);

	while (!display.closeWindow())
	{
		for (int i = 0; i < 3; i++)
			scn->Draw(1, i, BACK, i<1, false);
		scn->Motion();
		display.SwapBuffers();
		display.PollEvents();
	}
	delete scn;
	return 0;
}



/*
#include "InputManager.h"
#include "glm\glm.hpp"

int main(int argc, char *argv[])
{

	const int DISPLAY_WIDTH = 1600;
	const int DISPLAY_HEIGHT = 800;
	Viewport vp1(400, 0, DISPLAY_WIDTH - 400, DISPLAY_HEIGHT - 0);
	Viewport vp2(0, 0, 400, 266);

	const float zFar = 100.0f;
	const float zNear = 1.0f;
	const float CAM_ANGLE = 60.0f;

	Game *scn = new Game(glm::vec3(0.0f, 0.0f, 1.0f), CAM_ANGLE, zNear, zFar, vp1);

	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	init(display);

	scn->Init();

	scn->addShader("../res/shaders/pickingShader");
	scn->addShader("../res/shaders/basicShader");
	scn->addShader("../res/shaders/basicShader2D");

	scn->AddTexture("../res/textures/box0.bmp", false);
	scn->AddTexture("../res/textures/plane.png", true);
	scn->AddTexture(vp1.GetWidth(), vp1.GetHeight(), COLOR);
	scn->AddTexture(vp1.GetWidth(), vp1.GetHeight(), DEPTH);
	scn->AddBuffer(2, 0, COLOR);


	scn->AddCamera(glm::vec3(0.0f, 0.0f, 1.0f), CAM_ANGLE, zNear, zFar, vp2);
	display.setScene(scn);

	while (!display.closeWindow())
	{
		scn->Draw(1, 0, BACK, true, false);
		scn->Draw(1, 0, COLOR, true, false);

		//scn->Draw2D(2, 1, BACK, false, false);
		scn->Motion();
		display.SwapBuffers();
		display.PollEvents();

	}
	delete scn;
	return 0;
}*/