#version 330

const int MAX_JOINTS = 5; //max joints allowed in a skeleton
const int MAX_WEIGHTS = 3; //max number of joints that can affect a vertex

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec2 texCoords;
attribute vec3 weights;

out vec2 pass_texCoords;
out vec3 pass_normal;
out vec3 color0;

uniform mat4 MV;
uniform mat4 Projection;
uniform mat4 Normal;
uniform mat4 jointTransforms[MAX_JOINTS];
uniform int linksNum;

void main(void) {
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	mat4 matrix = mat4(1);
	for (int i = 0; i < MAX_WEIGHTS; i++) {
		if(i!=MAX_WEIGHTS-1 || jointIndices[i]<linksNum)
		{
		 
		 matrix =jointTransforms[int(jointIndices[i])];
		 //if(i!=1)
		//	matrix[3] = vec4(0,0,0,1);
		vec4 posePosition = matrix * vec4(position, 1.0);
		totalLocalPos += posePosition * weights[i];
		
		vec4 worldNormal = matrix * vec4(normal, 0.0);
		totalNormal += worldNormal * weights[i];
		}
	}

	//vec4 posePosition = matrix * vec4(position, 1.0);
	//vec4 worldNormal = matrix * vec4(normal, 0.0);

	gl_Position = MV * Projection * totalLocalPos;
	pass_normal = totalNormal.xyz;
	pass_texCoords = texCoords;
	color0 = color.xyz;
}
// Vertex shader for a point-light (local) source, with computation
// done in the fragment shader.
