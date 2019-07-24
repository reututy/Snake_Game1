#pragma once
#include "shader.h"
#include "shape.h"
#include "camera.h"
#include "Viewport.h"
#include "DrawBuffer.h"
#include "VertexArray.hpp"
#include <vector>
#include "glm/gtx/dual_quaternion.hpp"

enum buffers
	{
		COLOR,
		DEPTH,
		STENCIL,
		BACK,
		FRONT,
		ACCUM,
		NONE,
	};

class Scene : public MovableGLM
{

public:
	enum axis{xAxis,yAxis,zAxis};
	enum transformations{xLocalTranslate,yLocalTranslate,zLocalTranslate,xGlobalTranslate,yGlobalTranslate,zGlobalTranslate,
		xLocalRotate,yLocalRotate,zLocalRotate,xGlobalRotate,yGlobalRotate,zGlobalRotate,xScale,yScale,zScale,xCameraTranslate,yCameraTranslate,zCameraTranslate,
		vectorLocalTranslate, vectorLocalRotate};
	enum modes{POINTS,LINES,LINE_LOOP,LINE_STRIP,TRIANGLES,TRIANGLE_STRIP,TRIANGLE_FAN,QUADS};
	enum shapes
	{
		Axis,
		Plane,
		Cube,
		Octahedron,
		Tethrahedron,
		BezierLine,
		BezierSurface,
		LineCopy,
		MeshCopy,
	};
	
	Scene();
	Scene(glm::vec3 position,float angle,float near, float far,Viewport &vp);
	
	void addShapeFromFile(const std::string& fileName,int parent,unsigned int mode, const int kind, float scale);
	virtual void addShape(int type,int parent,unsigned int mode, const int kind);
	void addShapeCopy(int indx,int parent,unsigned int mode, const int kind);
	
	void addShader(const std::string& fileName);
	void AddTexture(const std::string& textureFileName, bool for2D);
	void AddTexture(int width,int height,int mode);
	void AddCamera(const glm::vec3& pos , float fov, float zNear, float zFar,Viewport vp);
	void AddBuffer(int texIndx,int cameraIndx,int mode);
	void ZeroShapesTrans();

	//virtual void Update( glm::mat4 MVP ,glm::mat4 *jointTransforms,const int length,const int  shaderIndx);//

	virtual void Update(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 Camera, glm::mat4 &Normal, const int shaderIndx) = 0;
	virtual void Update2D(glm::mat4& mat, int time, const int shaderIndx);
	/* Reut's addings:*/
	virtual void SkinningUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::vec4 dqRot[5], glm::vec4 dqTrans[5], const int shaderIndx, int index) = 0;
	virtual void LBSUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::mat4 jointTransforms[5], glm::ivec3 jointIndices, const int shaderIndx, int index) = 0;
	virtual void UpdateLBS(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 Camera, const glm::mat4 &Normal, glm::mat4 jointTransforms[5], int linksNum, int index, const int shaderIndx) = 0;

	virtual void WhenTranslate(){};
	virtual void WhenRotate(){};
	virtual void Motion(){};
	virtual void Draw(int shaderIndx,int cameraIndx,int buffer,bool toClear,bool debugMode);
	virtual void Draw2D(int shaderIndx,int cameraIndx,int buffer,bool toClear,bool debugMode);

	glm::mat4 GetViewProjection(int indx) const; 
	glm::mat4 GetShapeTransformation() const;
	
	void shapeTransformation(int type,float amt);
	void cameraTransformation(int type, float amt, int index); //Added
	void shapeRotation(glm::vec3 v, float ang,int indx);
	//void inline setPicked(int pickID){pickedShape = pickID;}
	float picking(int x,int y);
	void resize(int width,int hight);

	inline void setParent(int indx,int newValue) {chainParents[indx]=newValue;}
	inline float GetNear(int cameraIndx){return cameras[cameraIndx]->GetNear();}
	inline float GetFar(int cameraIndx){return cameras[cameraIndx]->GetFar();}
	inline float GetWHRelation(int cameraIndx){return cameras[cameraIndx]->GetWHRelation();}

	void ReadPixel();
	
	inline float GetAngle(int cameraIndx) {return cameras[cameraIndx]->GetAngle();}
	inline void Activate() {isActive = true;}
	inline void Deactivate() {isActive = false;}
	void HideShape(int shpIndx);
	//inline void SetMousePosition(double xpos, double ypos){xold =xpos; yold=ypos;}
	void updatePosition(float xpos, float ypos);
	void mouseProccessing(int button);
	bool inline IsActive() const { return isActive;} 
	virtual ~Scene(void);
	inline void SetDirection() {direction = -direction;}
	inline int GetDirection() {return direction;}
	glm::vec3 GetTipPositionInSystem(int indx);
	glm::vec3 GetVectorInSystem(int indx,glm::vec3 vec);
	void OpositeDirectionRot(glm::vec3 vec,float angle);
	inline void PrintShapeRotations(int indx){shapes[indx]->printRot(true);}
	inline void SetShapeTex(int shpIndx,int texIndx){shapes[shpIndx]->SetTexture(texIndx);} 
	inline void SetShapeShader(int shpIndx,int shdrIndx){shapes[shpIndx]->SetShader(shdrIndx);} 
	
	/* Reut's addings:*/
	glm::dualquat getQuaternion(glm::mat4 mat);
	void ChangeShapeMode(int index, unsigned int new_mode);

	int GetPickedShape();
	Shape* GetShape(int index);
	int GetSizeOfShapes();
	void SetPickedShape(int value);
	int GetNumOfShapes();
	bool GetView();
	Camera* GetCamera(int index);

	void SetView();
	void SetNumOfShape();
	void SetNumOfShapes(int value);
	void CheckCollisionDetection(int num_of_shape);
	void playTune(char* str);

private:	
	std::vector<DrawBuffer*> buffers;
	std::vector<int> texIndices;
	int verticesSize;
	int indicesSize;

	float depth;
	int xold, yold,xrel, yrel;
	int cameraIndx;
	void Clear(float r, float g, float b, float a);

protected:
	std::vector<Camera*> cameras; //light will have the properties of camera
	Shape *plane2D;
	std::vector<Shape*> shapes;
	std::vector<Shader*> shaders;
	std::vector<int> chainParents;
	std::vector<Texture*> textures;
	
	int pickedShape;
	int direction;
	static const int scaleFactor = 2;
	bool isActive;
	void ScaleAllDirections(int factor);
	int num_of_shapes; //Reut's addings
	bool view = true; //Reut's addings
};

