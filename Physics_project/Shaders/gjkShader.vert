#version 330 core
layout (location = 0) in vec3 position;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{

	gl_PointSize = 20.0;
    gl_Position = projection * view * model * vec4(position, 1.0f);
	
}