#define GLEW_STATIC
#include <GL\glew.h>
#include "scene.h"
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <SDL.h>
#include <glm/gtc/matrix_inverse.hpp>

using namespace std;
using namespace glm;

static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play

int star_count = 0;

void my_audio_callback(void *userdata, Uint8 *stream, int len);

static void printMat(const mat4 mat)
{
	printf(" matrix: \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[j][i]);
		printf("\n");
	}
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}

void Scene::playTune(char* str)
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		return;

	//local variables
	static Uint32 wav_length; // length of our sample
	static Uint8 *wav_buffer; // buffer containing our audio file
	static SDL_AudioSpec wav_spec; // the specs of our piece of music

	if (SDL_LoadWAV(str, &wav_spec, &wav_buffer, &wav_length) == NULL) {
		return;
	}
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length

	/* Open the audio device */
	if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

	/* Start playing */
	SDL_PauseAudio(0);

	// wait until we're don't playing
	while (audio_len > 0) {
		SDL_Delay(100);
	}

	// shut everything down
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);
}

Scene::Scene()
{
	//verticesSize = sizeof(vertices)/sizeof(vertices[0]);
	//
	//indicesSize = sizeof(indices)/sizeof(indices[0]) ; 
	glLineWidth(3);
	cameras.push_back(new Camera(vec3(0,0,1.0f),vec3(0,0,-1.0f),60.0f,0.1f,100.0f,Viewport()));		
	pickedShape = -1;
	depth = 0;
	cameraIndx = 0;
	xold = 0;
	yold = 0;
	isActive = false;
}

Scene::Scene(vec3 position,float angle,float near, float far,Viewport &vp)
{
	//verticesSize = sizeof(vertices)/sizeof(vertices[0]);
	//
	//indicesSize = sizeof(indices)/sizeof(indices[0]) ; 
	glLineWidth(6);
	cameras.push_back(new Camera(position, -position, angle, near, far, vp));
	pickedShape = -1;
	depth = 0;
	cameraIndx = 0;
	xold = 0;
	yold = 0;
	isActive = false;
}

void Scene::addShapeFromFile(const std::string& fileName,int parent,unsigned int mode, const int kind, float scale)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(fileName,mode, kind, scale));
}

void Scene::addShape(int type, int parent,unsigned int mode, const int kind)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(type,mode, kind));
}

void Scene::addShapeCopy(int indx,int parent,unsigned int mode, const int kind)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(*shapes[indx],mode, kind));
}

void Scene::addShader(const std::string& fileName)
{
	shaders.push_back(new Shader(fileName));
		
}

void Scene::AddTexture(const std::string& textureFileName,bool for2D)
{
	if(for2D)
	{
		texIndices.push_back(textures.size());
		plane2D->SetTexture(textures.size());
	}
	textures.push_back(new Texture(textureFileName));
}

void Scene::AddTexture(int width,int height,int mode)
{
	texIndices.push_back(textures.size());
	plane2D->SetTexture(textures.size());
	textures.push_back(new Texture(width,height,mode));
}

void Scene::AddCamera(const glm::vec3& pos , float fov, float zNear, float zFar,Viewport vp)
{
	cameras.push_back(new Camera(pos,-pos,fov,zNear,zFar,vp));
}

void Scene::AddBuffer(int texIndx,int cameraIndx,int mode)
{
	buffers.push_back(new DrawBuffer(cameras[cameraIndx]->GetWidth(),cameras[cameraIndx]->GetHeight()));
	buffers.back()->Bind();
	textures[texIndx]->bindTex2Buffer(0,mode);
	textures[texIndx+1]->bindTex2Buffer(0,DEPTH);
	buffers.back()->UnBind();
}

mat4 Scene::GetViewProjection(int indx) const
{
	return cameras[indx]->GetViewProjection();
}

mat4 Scene::GetShapeTransformation() const
{
	return shapes[pickedShape]->makeTrans();
}

void Scene::Draw(int shaderIndx,int cameraIndx,int buffer,bool toClear,bool debugMode)
{
	//glEnable(GL_CULL_FACE);
	//buffers.back()->SetDrawDistination(buffers.size()-1,buffer);
		
	glm::mat4 Normal = makeTrans();
	glm::mat4 Projection = cameras[cameraIndx]->GetViewProjection(); //Added
	glm::mat4 MV = Normal; //Added
	glm::mat4 MV1; //Added
	glm::mat4 Camera = cameras[cameraIndx]->makeTrans(); //Added

	glm::dualquat DQ; //Added
	glm::vec4 dqRot[5]; //Added
	glm::vec4 dqTrans[5]; //Added

	glm::mat4 jointTransforms[5]; //Added
	int k = 0; //Added

	if(buffer == BACK)
		glViewport(cameras[cameraIndx]->GetLeft(),cameras[cameraIndx]->GetBottom(),cameras[cameraIndx]->GetWidth(),cameras[cameraIndx]->GetHeight());
	else
		glViewport(0,0,cameras[cameraIndx]->GetWidth(),cameras[cameraIndx]->GetHeight());

	if (cameraIndx == 0)
		Camera = shapes[chainParents[snake->GetHeadIndex()+1]]->makeTrans() * Camera;

	glm::mat4 MVP = Projection * Normal;
	int p = pickedShape;
	if(toClear)
	{
		if(shaderIndx>0)
			Clear(1,1,1,1);
		else
			Clear(0,0,0,0);
	}

	for (int i = snake->GetHeadIndex(); i <= snake->GetHeadIndex() + snake->GetNumOfLinks() + 1; i++, k++)
	{
		mat4 Normal1 = mat4(1);
		pickedShape = i;
		for (int j = i; chainParents[j] > -1; j = chainParents[j])
		{
			Normal1 = shapes[chainParents[j]]->makeTrans() * Normal1;
		}

		Normal1 = Normal * Normal1;
		MV1 = Normal1;
		MV1 = MV1 * shapes[i]->makeTransScale(mat4(1));
		//MV1[3].x -= 1;
		//MV1[3].y -= 1;
		//MV1[3].z -= 1;
		jointTransforms[k] = MV1;

		DQ = getQuaternion(MV1);
		dqRot[k] = glm::vec4(DQ.real.w, DQ.real.x, DQ.real.y, DQ.real.z);
		dqTrans[k] = glm::vec4(DQ.dual.w, DQ.dual.x, DQ.dual.y, DQ.dual.z);
	}

	for (unsigned int i=0; i<shapes.size(); i++)
	{
		if (shapes[i]->Is2Render())
		{
			mat4 Normal1 = mat4(1);
			pickedShape = i;
			for (int j = i; chainParents[j] > - 1; j = chainParents[j])
			{
				Normal1 = shapes[chainParents[j]]->makeTrans() * Normal1;
			}

			Normal1 = Normal * Normal1;
			MV1 = Normal1;
			MV1 = MV1 * shapes[i]->makeTransScale(mat4(1));

			/*
			if (i == 32)
			{
				DQ = getQuaternion(MV1);
				dqRot[0] = glm::vec4(DQ.real.w, DQ.real.x, DQ.real.y, DQ.real.z);
				dqTrans[0] = glm::vec4(DQ.dual.w, DQ.dual.x, DQ.dual.y, DQ.dual.z);
			}
			else if (i == 33)
			{
				DQ = getQuaternion(MV1);
				dqRot[1] = glm::vec4(DQ.real.w, DQ.real.x, DQ.real.y, DQ.real.z);
				dqTrans[1] = glm::vec4(DQ.dual.w, DQ.dual.x, DQ.dual.y, DQ.dual.z);
			}
			else if (i == 34)
			{
				DQ = getQuaternion(MV1);
				dqRot[2] = glm::vec4(DQ.real.w, DQ.real.x, DQ.real.y, DQ.real.z);
				dqTrans[2] = glm::vec4(DQ.dual.w, DQ.dual.x, DQ.dual.y, DQ.dual.z);
			}
			else if (i == 35)
			{
				DQ = getQuaternion(MV1);
				dqRot[3] = glm::vec4(DQ.real.w, DQ.real.x, DQ.real.y, DQ.real.z);
				dqTrans[3] = glm::vec4(DQ.dual.w, DQ.dual.x, DQ.dual.y, DQ.dual.z);
			}
			else if (i == 36)
			{
				DQ = getQuaternion(MV1);
				dqRot[4] = glm::vec4(DQ.real.w, DQ.real.x, DQ.real.y, DQ.real.z);
				dqTrans[4] = glm::vec4(DQ.dual.w, DQ.dual.x, DQ.dual.y, DQ.dual.z);
			}*/
			
			if (shaderIndx > 0)
			{
				//Update(MV1, Projection, Camera, Normal1, shapes[i]->GetShader());
				UpdateLBS(MV1, Projection, Camera, Normal1, jointTransforms, 5, i-snake->GetHeadIndex(), shapes[i]->GetShader());
				//SkinningUpdate(MV1, Projection, Normal1, Camera, dqRot, dqTrans, shapes[i]->GetShader(), i);
				shapes[i]->Draw(shaders, textures, false);

			}
			else //picking
			{
				//Update(MV1, Projection, Camera, Normal1, shapes[i]->GetShader());
				UpdateLBS(MV1, Projection, Camera, Normal1, jointTransforms, 5, i-snake->GetHeadIndex(), shapes[i]->GetShader());
				//SkinningUpdate(MV1, Projection, Normal1, Camera, dqRot, dqTrans, shapes[i]->GetShader(), i);
				shapes[i]->Draw(shaders, textures, true);
			}
		}
	}
	pickedShape = p;
}

void Scene::Draw2D(int shaderIndx,int cameraIndx,int buffer,bool toClear,bool debugMode)
{
	glDisable(GL_CULL_FACE);
	buffers.back()->SetDrawDistination(buffers.size()-1,buffer);
	glm::mat4 Normal = glm::mat4(1);
		
	glViewport(cameras[cameraIndx]->GetLeft(),cameras[cameraIndx]->GetBottom(),cameras[cameraIndx]->GetWidth(),cameras[cameraIndx]->GetHeight());
		
	if(toClear)
	{
		if(shaderIndx>0)
			Clear(1,1,1,1);
		else
			Clear(0,0,0,0);
	}
	if(isActive)
		Update2D(Normal,1,shaderIndx);
	else
		Update2D(Normal,0,shaderIndx);
	plane2D->Draw(shaders,textures,false);
		
}

void Scene::Update2D(glm::mat4& mat, int time, const int shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	s->Bind();
	s->SetUniformMat4f("MVP",glm::mat4(1), shaderIndx);
	//s->SetUniformMat4f("Normal",mat);
	s->SetUniform1i("time", time);
		
	for(int i = 0; i<texIndices.size();i++)
	{
		char str[9];
		sprintf_s<9>(str,"sampler%d",i+1);
		s->SetUniform1i(str,textures[texIndices[i]]->GetSlot()); //to fix to slot
	}

}

void Scene::shapeRotation(vec3 v, float ang,int indx)
{
	if(v.x >0.9999)
		shapes[indx]->globalSystemRot(ang,v,xAxis1);
	else 
		if(v.z >0.9999)
			shapes[indx]->globalSystemRot(ang,v,zAxis12);
	else
		shapes[indx]->globalSystemRot(ang,v,-1);
}

void Scene::shapeTransformation(int type,float amt)
{
	vec3 newAxis;
	if(glm::abs(amt)>1e-5)
	{
		switch (type)
		{
		case xLocalTranslate:
			if(pickedShape ==-1)
				myTranslate(vec3(amt,0,0),1);
			else
			{
				int i = pickedShape;
				for (; chainParents[i] > -1; i = chainParents[i]);
				shapes[i]->myTranslate(vec3(amt,0,0),1);
			}
			break;
		case yLocalTranslate:
			if(pickedShape ==-1)
				myTranslate(vec3(0,amt,0),1);
			else
				{
					int i = pickedShape;
					for (; chainParents[i] > -1; i = chainParents[i]);
						shapes[i]->myTranslate(vec3(0, amt, 0), 1);
				}
			break;
		case zLocalTranslate:
			if(pickedShape ==-1)
				myTranslate(vec3(0,0,amt),1);
			else
			{
				int i = pickedShape;
				for (; chainParents[i] > -1; i = chainParents[i]);
				shapes[i]->myTranslate(vec3(0,0,amt),1);
			}
			break;
		case xGlobalTranslate:
			if(pickedShape ==-1)
				myTranslate(vec3(amt/5.0,0,0),0);
			else
			{
				int i = pickedShape;
				for (; chainParents[i] > -1; i = chainParents[i]);
				shapes[i]->myTranslate(vec3(amt,0,0),0);
			}
			break;
		case yGlobalTranslate:
			if(pickedShape ==-1)
				myTranslate(vec3(0,amt/5.0,0),0);
			else
			{
				int i = pickedShape;
				for (; chainParents[i] > -1; i = chainParents[i]);
				shapes[i]->myTranslate(vec3(0,amt,0),0);
			}
			break;
		case zGlobalTranslate:
			if(pickedShape ==-1)
				myTranslate(vec3(0,0,amt/5.0),0);
							else
			{
				int i = pickedShape;
				for (; chainParents[i] > -1; i = chainParents[i]);
				shapes[i]->myTranslate(vec3(0,0,amt),0);
			}
			break;
		case xLocalRotate:
			if(pickedShape ==-1)
				myRotate(amt,vec3(1,0,0),xAxis1);
			else
				shapes[pickedShape]->myRotate(amt,vec3(1,0,0),xAxis1);
			break;
		case yLocalRotate:
			if(pickedShape ==-1)
				myRotate(amt,vec3(0,1,0),-1);
			else
				shapes[pickedShape]->myRotate(amt, vec3(0, 1, 0), -1);
			break;
		case zLocalRotate:
			if(pickedShape ==-1)
				myRotate(amt,vec3(0,0,1),zAxis1);
			else

				shapes[pickedShape]->myRotate(amt,vec3(0,0,1),zAxis1);
		break;
		case xGlobalRotate:
			if(pickedShape ==-1)
				globalSystemRot(amt,vec3(1,0,0),xAxis1);
			else
			{
					
				if(direction == -1 && pickedShape+2<shapes.size())
				{
					OpositeDirectionRot(glm::vec3(1,0,0),amt);
				}
				else
					shapes[pickedShape]->globalSystemRot(amt,vec3(1,0,0),xAxis1);
			}
			break;
		case yGlobalRotate:
			if(pickedShape ==-1)
				globalSystemRot(amt,vec3(0,1,0),-1);
			else
			{
				shapes[pickedShape]->globalSystemRot(amt,vec3(0,1,0),-1);
				if(direction == -1)
				{
					OpositeDirectionRot(glm::vec3(0,1,0),amt);
				}
				else
					shapes[pickedShape]->globalSystemRot(amt,vec3(1,0,0),xAxis1);
			}
			break;
		case zGlobalRotate:
			if(pickedShape ==-1)
				globalSystemRot(amt,vec3(0,0,1),zAxis12);
			else
			{
					
					
				if(direction == -1 && pickedShape+2<shapes.size())
				{
					OpositeDirectionRot(glm::vec3(0,0,1),amt);
				}
				else
					shapes[pickedShape]->globalSystemRot(amt,vec3(0,0,1),zAxis12);
			}
		break;
		case xScale:
			if(pickedShape ==-1)
				myScale(vec3(amt,1,1));
			else
				shapes[pickedShape]->myScale(vec3(amt,1,1));
		break;
		case yScale:
			if(pickedShape ==-1)
				myScale(vec3(1,amt,1));
			else
				shapes[pickedShape]->myScale(vec3(1,amt,1));
		break;
		case zScale:
			if(pickedShape ==-1)
				myScale(vec3(1,1,amt));
			else
				shapes[pickedShape]->myScale(vec3(1,1,amt));
		break;
		case xCameraTranslate: //camera plane translate
			if (pickedShape == -1)
				myTranslate(vec3(amt / 5.0, 0, 0), 0);
			else
			{
				//newAxis = findAxis(vec3(1,0,0));					
				int i = pickedShape;
				for (; chainParents[i] > 0; i = chainParents[i]);

				shapes[i]->translateInSystem(*this, vec3(amt, 0, 0), 0, false);
			}
			break;
		case yCameraTranslate:
			if (pickedShape == -1)
				myTranslate(vec3(0, amt / 5.0, 0), 0);
			else
			{
				//newAxis = findAxis(vec3(0,1,0));
				int i = pickedShape;
				for (; chainParents[i] > 0; i = chainParents[i]);
					shapes[i]->translateInSystem(*this, vec3(0, amt, 0), 0, false);
			}
			break;
		case zCameraTranslate:
			if (pickedShape == -1)
				myTranslate(vec3(0, 0, amt / 5.0), 0);
			else
			{
				//	newAxis = findAxis(vec3(0,0,1));
				int i = pickedShape;
				for (; chainParents[i] > 0; i = chainParents[i]);
					shapes[i]->translateInSystem(*this, vec3(0, 0, amt), 0, false);
			}
			break;
		default:
			break;
		}
	}
}

void Scene::cameraTransformation(int type, float amt, int index)
{
	vec3 newAxis;
	if (glm::abs(amt) > 1e-5)
	{
		switch (type)
		{
		case xLocalTranslate:
			cameras[index]->myTranslate(vec3(amt, 0, 0), 1);
			break;
		case yLocalTranslate:
			cameras[index]->myTranslate(vec3(0, amt, 0), 1);
			break;
		case zLocalTranslate:
			cameras[index]->myTranslate(vec3(0, 0, amt), 1);
			break;
		case xGlobalTranslate:
			cameras[index]->myTranslate(vec3(amt, 0, 0), 0);
			break;
		case yGlobalTranslate:
			cameras[index]->myTranslate(vec3(0, amt, 0), 0);
			break;
		case zGlobalTranslate:
			cameras[index]->myTranslate(vec3(0, 0, amt), 0);
			break;
		case xLocalRotate:
			cameras[index]->myRotate(amt, vec3(1, 0, 0), xAxis1);
			break;
		case yLocalRotate:
			cameras[index]->myRotate(amt, vec3(0, 1, 0), -1);
			break;
		case zLocalRotate:
			cameras[index]->myRotate(amt, vec3(0, 0, 1), zAxis1);
			break;
		case xGlobalRotate:
			/*
			if (pickedShape == -1)
				globalSystemRot(amt, vec3(1, 0, 0), xAxis1);
			else
			{
				if (direction == -1 && pickedShape + 2 < shapes.size())
				{
					OpositeDirectionRot(glm::vec3(1, 0, 0), amt);
				}
				else
					shapes[pickedShape]->globalSystemRot(amt, vec3(1, 0, 0), xAxis1);
			}*/
			break;
		case yGlobalRotate:
			/*
			if (pickedShape == -1)
				globalSystemRot(amt, vec3(0, 1, 0), -1);
			else
			{
				shapes[pickedShape]->globalSystemRot(amt, vec3(0, 1, 0), -1);
				if (direction == -1)
				{
					OpositeDirectionRot(glm::vec3(0, 1, 0), amt);
				}
				else
					shapes[pickedShape]->globalSystemRot(amt, vec3(1, 0, 0), xAxis1);
			}*/
			break;
		case zGlobalRotate:
			/*
			if (pickedShape == -1)
				globalSystemRot(amt, vec3(0, 0, 1), zAxis12);
			else
			{


				if (direction == -1 && pickedShape + 2 < shapes.size())
				{
					OpositeDirectionRot(glm::vec3(0, 0, 1), amt);
				}
				else
					shapes[pickedShape]->globalSystemRot(amt, vec3(0, 0, 1), zAxis12);
			}*/
			break;
		default:
			break;
		}
	}
}

void Scene::OpositeDirectionRot(glm::vec3 vec,float angle)
{
	int i=pickedShape;
	glm::mat4 globalMat = glm::mat4(1)*shapes[pickedShape+2]->GetRot();
	glm::mat4 localMat = glm::mat4(1)*shapes[pickedShape+2]->GetRot();
	for (; chainParents[i] >= 0; i = chainParents[i])
		localMat =  shapes[i]->GetRot() * localMat;
	i=1;
	std::cout<<"vec1 ("<<vec.x <<", "<<vec.y<<", "<<vec.z<<")"<<std::endl;
	glm::mat4 rotMat = localMat*glm::rotate(glm::mat4(1),angle,vec)*glm::transpose(localMat); 
		
	glm::vec3 posStart = GetTipPositionInSystem(pickedShape);
	std::cout<<"start ("<<posStart.x <<", "<<posStart.y<<", "<<posStart.z<<")"<<std::endl;
		
	shapes[pickedShape]->zeroRot(true);

	shapes[i]->buildAngMatrices(rotMat*shapes[i]->GetRot());
	//rotMat = rotMat*shapes[i]->GetRot();
	//shapes[i]->SetRotMatrix(rotMat);
		
	rotMat = globalMat*glm::rotate(glm::mat4(1),-angle,vec)*glm::transpose(globalMat);
	rotMat = rotMat*shapes[pickedShape+2]->GetRot();
	shapes[pickedShape+2]->buildAngMatrices(rotMat);
	//if(vec.x>0.99)
	//	shapes[pickedShape+2]->myRotate(-angle,vec,xAxis1);
	//else
	//	shapes[pickedShape+2]->myRotate(-angle,vec,zAxis2);
		
	glm::vec3 posEnd = GetTipPositionInSystem(pickedShape);
	std::cout<<"end ("<<posEnd.x <<", "<<posEnd.y<<", "<<posEnd.z<<")"<<std::endl;
	shapes[i]->myTranslate((posStart-posEnd)/glm::vec3(1,1,(float)(scaleFactor)),0);
}
	
void Scene::resize(int width,int height)
{
	//glViewport(cameras[0]->GetLeft(),cameras[0]->GetBottom(),width,height);
		
	cameras[0]->SetProjection(cameras[cameraIndx]->GetNear(),cameras[cameraIndx]->GetFar(),Viewport(cameras[0]->GetLeft(),cameras[0]->GetBottom(),width,height));
	if(buffers.size()>0)
	{
		buffers[0]->resize(width,height);
		for(Texture* tex: textures)
		{
			if(tex->IsReadFromBuffer())
			{
				delete tex;
				tex = new Texture(width,height,COLOR);
				buffers[0]->Bind();
				tex->bindTex2Buffer(0,COLOR);
				buffers[0]->UnBind();
			}
		}
	}
}

float Scene::picking(int x,int y)
{
	Draw(0,0,BACK,true,false); 
		
	GLint viewport[4];  
	unsigned char data[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //reading viewport parameters
	glReadPixels(x,viewport[3] - y,1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glReadPixels(x,viewport[3] - y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
	cout<<"depth "<<depth<<endl;
	int pickedID = data[0] + data[1]*256 + data[2]*256*256 - 1;
	if(data[0] == 0 && data[1] == 0 && data[2] == 0)
	{
		pickedShape = -1;
		//std::cout<<"not picked"<<std::endl;
	}
	else
	{
		pickedShape = pickedID;
		std::cout<<"picked "<<pickedID<<std::endl;
		xold = x;
		yold = y;
	}
	return depth;
}

//return coordinates in global system for a tip of arm position is local system 
glm::vec3 Scene::GetTipPositionInSystem(int indx)
{
	mat4 Normal1 = mat4(1);
	if(indx>0)
	{
		int j = indx;
		glm::vec3 vec = glm::vec3(0);
		for (;  chainParents[j] > 0; j = chainParents[j])
		{
			vec = shapes[j]->getVectorInSystem(glm::mat4(1),vec + glm::vec3(0,0,2));
		}
		glm::vec3 origin = vec3(shapes[j]->getTraslate());
		return origin + shapes[j]->getVectorInSystem(glm::mat4(1),vec+glm::vec3(0,0,2))*(float)scaleFactor;
		//return shapes[indx]->getTipPos(Normal1);
	}
	else if(indx ==0)
		return vec3(shapes[0]->getTraslate());
	else
	{
		return vec3(0,0,0); 
	}
}

glm::vec3 Scene::GetVectorInSystem(int indx,glm::vec3 vec)
{
	mat4 Normal1 = mat4(1);
	if(indx>-1)
	{
		for (int j = indx;  chainParents[j] > -1; j = chainParents[j])
		{
			Normal1 = shapes[chainParents[j]]->makeTrans() * Normal1;
		}
		return vec3(Normal1*vec4(vec,0));//shapes[indx]->getVectorInSystem(Normal1,vec);
	}
	else
	{
		return vec; 
	}
}
	
void Scene::mouseProccessing(int button)
{
	if(pickedShape == -1 || shapes[pickedShape]->Is2D())
	{
		if(button == 1 )
		{				
			//GLint viewport[4];
			//float zTmp = 2.0*depth -1.0;
			//glGetIntegerv(GL_VIEWPORT, viewport);
			float z=cameras[cameraIndx]->GetFar()+depth*(cameras[cameraIndx]->GetNear()-cameras[cameraIndx]->GetFar());
			float transX = (xrel)/(float) (cameras[cameraIndx]->GetWidth())*cameras[cameraIndx]->GetNear()*2.0f*tan(cameras[cameraIndx]->GetAngle()*M_PI/360.0f)*(cameras[cameraIndx]->GetFar()/z);
			float transY =(yrel)/(float) (cameras[cameraIndx]->GetHeight())*cameras[cameraIndx]->GetNear()*2.0f*tan(cameras[cameraIndx]->GetAngle()*M_PI/360.0f)*(cameras[cameraIndx]->GetFar()/z);

			shapeTransformation(xCameraTranslate,-transX);
			shapeTransformation(yCameraTranslate,transY);
			WhenTranslate();
		}
		else
		{
			shapeTransformation(zGlobalRotate,xrel*.5f);
			shapeTransformation(xGlobalRotate,yrel*.5f);
			WhenRotate();
		}
	}
}

void Scene::ZeroShapesTrans()
{
	for (unsigned int i = 0; i < shapes.size(); i++)
	{
		shapes[i]->zeroTrans();
		shapes[i]->zeroRot(false);
		shapes[i]->zeroRot(true);
	}
}

void Scene::ReadPixel()
{
	glReadPixels(1,1,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
}

void Scene::updatePosition(float xpos, float ypos)
{
	xrel = xold - xpos;
	yrel = yold - ypos;
	xold = xpos;
	yold = ypos;
}

void Scene::HideShape(int shpIndx)
{
	if(shapes[shpIndx]->Is2Render())
		shapes[shpIndx]->Hide();
	else 
		shapes[shpIndx]->Unhide();
}

Scene::~Scene(void)
{
	for (Shape* shp : shapes)
		delete shp;
	for (Camera* cam : cameras)
		delete cam;
	for (Shader* sdr: shaders)
		delete sdr;
	for (Texture* tex: textures)
		delete tex;
	for (DrawBuffer* buf: buffers)
		delete buf;
	if (plane2D)
		delete plane2D;
}

void Scene::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
	 
void Scene::ScaleAllDirections(int factor)
{
	shapeTransformation(xScale,factor);
	shapeTransformation(yScale,factor);
	shapeTransformation(zScale,factor);
}

glm::dualquat Scene::getQuaternion(glm::mat4 mat)
{
	glm::mat3x4 mat_3x4 = glm::mat3x4(glm::transpose(mat));
	return glm::dualquat_cast(mat_3x4);
}

void Scene::ChangeShapeMode(int index, unsigned int new_mode)
{
	shapes[index]->ChangeMode(new_mode);
}

int Scene::GetPickedShape()
{
	return pickedShape;
}

Shape* Scene::GetShape(int index)
{
	return shapes[index];
}

int Scene::GetSizeOfShapes()
{
	return shapes.size() - 1;
}

void Scene::SetPickedShape(int value)
{
	pickedShape = value;
}

int Scene::GetNumOfShapes()
{
	return num_of_shapes;
}

bool Scene::GetView()
{
	return view;
}

bool Scene::GetMainView()
{
	return main_view;
}

bool Scene::GetMenuView()
{
	return menu_view;
}

bool Scene::GetAboutView()
{
	return about_view;
}

bool Scene::GetQuitView()
{
	return quit_view;
}

bool Scene::GetFinishView()
{
	return finish_view;
}

bool Scene::GetPlayAgain()
{
	return play_again;
}

Camera* Scene::GetCamera(int index)
{
	return cameras[index];
}

Player* Scene::GetPlayer()
{
	return snake;
}

void Scene::SetView()
{
	if (view == true)
		view = false;
	else
		view = true;
}

void Scene::SetMainView()
{
	if (main_view == true)
		main_view = false;
	else
		main_view = true;
}

void Scene::SetMenuView()
{
	if (menu_view == true)
		menu_view = false;
	else
		menu_view = true;
}

void Scene::SetAboutView()
{
	if (about_view == true)
		about_view = false;
	else
		about_view = true;
}

void Scene::SetQuitView()
{
	if (quit_view == true)
		quit_view = false;
	else
		quit_view = true;
}

void Scene::SetFinishView()
{
	if (finish_view == true)
		finish_view = false;
	else
		finish_view = true;
}

void Scene::SetPlayAgain()
{
	if (play_again == true)
		play_again = false;
	else
		play_again = true;
}

void Scene::SetNumOfShapes(int value)
{
	num_of_shapes = value;
}

void Scene::SetNumOfShape()
{
	pickedShape = shapes.size() - 1;
	shapes[pickedShape]->SetNumOfShape(pickedShape);
	pickedShape = -1;
}

//Checks collision for each pair of shapes in scene and if collide, draw the appropriate bounding boxes.
void Scene::CheckCollisionDetection(int num_of_shape)
{
	Shape* shape1 = shapes[num_of_shape]; //the snake
	glm::vec3 center_of_snake = glm::vec3(shape1->makeTransScale()[3]);
	glm::vec3 center_of_shape;

	float diff;
	for (int i = 0; i < shapes.size(); i++)
	{
		Shape* shape2 = shapes[i];
		if ((shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Reward ||
			shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Obstacle || 
			shape2->GetMesh()->GetKind() == MeshConstructor::Kind::WallWin ||
			shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Wall)
			&& shape1->GetNumOfShape() != shape2->GetNumOfShape())
		{
			center_of_shape = glm::vec3(shape2->makeTransScale()[3]);
			diff = glm::sqrt((center_of_shape.x - center_of_snake.x)*(center_of_shape.x - center_of_snake.x) + 
							(center_of_shape.y - center_of_snake.y)*(center_of_shape.y - center_of_snake.y) + 
							(center_of_shape.z - center_of_snake.z)*(center_of_shape.z - center_of_snake.z));
			if (shape2->GetMesh()->GetKind() == MeshConstructor::Kind::WallWin && diff < 40 && shape2->Getfound() == false)
			{
				shape2->Setfound(true);
				playTune("Sounds/win.wav");
				Deactivate();
				SetFinishView();
				if (star_count <= 3)
					SetShapeTex(40, 16);
				else if (star_count > 3 && star_count <= 6)
					SetShapeTex(40, 17);
				else if (star_count == 7)
					SetShapeTex(40, 18);
				SetMainView();
			}
			else if (shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Wall && diff < 40 && shape2->Getfound() == false)
			{
				shape2->Setfound(true);
				playTune("Sounds/loose.wav");
				Deactivate();
				SetShapeTex(40, 15);
				SetFinishView();
				SetMainView();
			}
			if (diff < 2)
			{
				if (shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Reward && shape2->Getfound() == false)
				{
					shape2->Hide();
					shape2->Setfound(true);
					star_count++;
					playTune("Sounds/eat.wav");
				}
			}
			else if (diff < 5)
			{
				if (shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Obstacle && shape2->Getfound() == false)
				{
					shape2->Setfound(true);
					playTune("Sounds/loose.wav");
					Deactivate();
					SetShapeTex(40, 15);
					SetFinishView();
					SetMainView();
				}
			}
		}
	}

	/*
	int box_to_draw_index;
	Shape* shape1 = shapes[num_of_shape]; //the snake
	for (int i = 0; i < shapes.size(); i++)
	{
		Shape* shape2 = shapes[i];
		if ((shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Reward ||
			shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Obstacle) && shape1->GetNumOfShape() != shape2->GetNumOfShape())
		{
			box_to_draw_index = shape1->CollisionDetection(shape2);
			if (box_to_draw_index > -1)
			{
				if (shape1->GetMesh()->GetKind() == MeshConstructor::Kind::Reward && shape1->Getfound() == false)
				{
					shape2->Hide();
					shape1->Setfound(true);
				}
				else if (shape2->GetMesh()->GetKind() == MeshConstructor::Kind::Reward && shape2->Getfound() == false)
				{
					shape2->Hide();
					shape2->Setfound(true);
					playTune("Sounds/eat.wav");
				}
				std::cout << "boom" << std::endl;
				shapes[box_to_draw_index]->Unhide();
			}
		}
	}
	*/
}