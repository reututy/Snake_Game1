#version 330

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec3 weights;
attribute vec2 texCoord;

uniform mat4 MVP;
uniform mat4 Normal;

out vec3 color0;

void main()
{
	color0 = weights;
	gl_Position = MVP * vec4(position, 1.0);
}
