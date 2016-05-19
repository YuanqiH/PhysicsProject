#version 330 core

in vec4 outcolor;
//uniform sampler2D tex;
out vec4 finalColor;

void main(){

	//use the texture later
	// finalColor = texture(tex,gl_PointCoord) * outolor;
	finalColor = outcolor;
	
}
