#version 330 core

in vec3 outcolor;
//uniform sampler2D tex;
out vec4 finalColor;

uniform float isCollided;
uniform float isNarrowCollided;
uniform float isBox;

void main(){

	//use the texture later
	// finalColor = texture(tex,gl_PointCoord) * outolor;
	//finalColor = vec4(outcolor, 1.0) ;
	
	if(isCollided == 1.0){
		finalColor = vec4( 0.415 , 0.407 , 0.141 , 1.0);
		if(isNarrowCollided == 1.0){
			finalColor = vec4(1.0 , 1.0 , 1.0 , 1.0);
			}
	}
	else{
		finalColor = vec4(0.53 , 0.671 , 0.153 , 1.0);
	}
	
	if(isBox == 1.0){
		finalColor = vec4(1.0 , 1.0 , 1.0 , 1.0);
	}
	
}
