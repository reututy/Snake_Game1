#version 330

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec2 texCoords;
attribute vec3 weights;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;

uniform mat4 MV;
uniform mat4 Projection;
uniform mat4 Normal;

uniform mat4 jointTransforms[5];
uniform ivec3 jointIndices;

//uniform vec4 dqRot[5];
//uniform vec4 dqTrans[5];
uniform int index;

void main()
{
	vec4 totalLocalPos = vec4(0.0);
	mat4 matrix = mat4(1);
	
	for (int i = 0; i < 3; i++) 
	{
		matrix = jointTransforms[int(jointIndices[i])];		
		vec4 posePosition = matrix * vec4(position, 1.0);
		totalLocalPos += posePosition * weights[i];
	}
	//gl_Position = MV * Projection * totalLocalPos;
	texCoord0 = texCoords;
	color0 = color;
	
	if(index >= 20 && index <= 28)
	{	
		normal0 = (Normal * vec4(normal, 0.0)).xyz;	
		//gl_Position = Projection * M * vec4(position, 1.0); //you must have gl_Position
		gl_Position = MV * Projection * totalLocalPos;
	}
	else
	{
		normal0 = (Normal * vec4(normal, 0.0)).xyz;
		gl_Position = Projection * MV * vec4(position, 1.0); //you must have gl_Position

	}
}
