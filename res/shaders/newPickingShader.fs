#version 330

in vec3 color0;

void main()
{
	gl_FragColor = vec4(color0,1);
}