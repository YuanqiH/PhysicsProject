#version 330 core

uniform mat4 view;
uniform mat4  model;
uniform mat4 projection;

layout(location = 0 ) in vec4 vertex;
layout(location = 1) in vec4 color;

out vec4 outcolor;

void main(){
	
	vec4 eyePos = view * model * vertex;
	gl_Position = projection * eyePos;

	outcolor = color;
	
	float dist = length(eyePos.xyz);
	float att = inversesqrt(0.1f * dist);
	gl_PointSize = 3.0f * att;


}