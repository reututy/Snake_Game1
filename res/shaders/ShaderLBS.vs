#version 330

const int MAX_JOINTS = 5; //max joints allowed in a skeleton

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
uniform mat4 Camera;
uniform mat4 Normal;
uniform mat4 jointTransforms[MAX_JOINTS];
uniform int links_num;
uniform int index;

void main()
{	
	if(index >= 0 && index <= 4)
	{
		vec4 res0 = vec4(0);
		vec4 res1 = vec4(0);
		vec4 res2 = vec4(0);
		vec4 result = vec4(0);

		res1 = weights[1] * jointTransforms[index] * vec4(position, 1.0);
		result = res1;
		if (index > 0)
		{
			res0 = weights[0] * jointTransforms[index - 1] * vec4(position, 1.0);
			result += res0;
		}	
		if(index < links_num - 1)
		{
			res2 =  weights[2] * jointTransforms[index + 1] * vec4(position, 1.0);
			result += res2;
		}
		if (index == 0 || index == links_num - 1)
			result += res1;
		gl_Position = Projection * inverse(Camera) * result; //you must have gl_Position
	}
	else gl_Position = Projection * inverse(Camera) * MV * vec4(position, 1.0); //you must have gl_Position
	
	texCoord0 = texCoords;
	color0 = color;
	normal0 = (Normal * vec4(normal, 0.0)).xyz;
	position0 = vec3(Normal * vec4(position, 1.0));
	
	//gl_Position = result * vec4(position, 1.0); //you must have gl_Position
	//gl_Position = Projection * inverse(Camera) * MV * result * vec4(position, 1.0); //you must have gl_Position
	//gl_Position = Projection * inverse(Camera) * MV * vec4(position, 1.0); //you must have gl_Position
}
