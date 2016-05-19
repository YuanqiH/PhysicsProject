#include <iostream>
#include <GL\glew.h>
//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include<glm\gtx\rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"

//GL include
#include "ShaderManager.hpp"
#include "loadTexture.h"
#include "Floor.h"
#include "basicParticles.hpp"
#include "glParticleRenderer.hpp"
#include "particleSimulater.hpp"
#include "rigibody.hpp"
#include "cube.hpp"

void RenderScene(Shader &shader);
glm::vec3 get_ray_from_mouse(float mouse_x,float mouse_y);
glm::vec3 get_mouse_move(float speed, glm::vec3 currentPosition);
bool ray_sphere (
	glm::vec3 ray_origin_wor,
	glm::vec3 ray_direction_wor,
	glm::vec3 sphere_centre_wor,
	float sphere_radius,
	float* intersection_distance
);

#define NUM_PARTICLE 4000

GLFWwindow* window;
#define window_width 1024
#define window_height 980
 
float a = 0.0f;
// position
glm::vec3 Camposition = glm::vec3( 0.0f, 8.0f, 8.0f );
glm::vec3 direction = glm::vec3( 0.0f, -0.5f, -1.0f );
glm::vec3 up = glm::vec3( 0.0f, 1.0f, 0.0f );
glm::vec3 rightv = glm::vec3( 0.0f, 0.0f, 0.0f );
glm::mat4 projectionMatrix;
glm::mat4 ViewMatrix;

// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
// Initial Field of View
float FoV = 45.0F;

float speed = 4.0f; // 4 units / second
float mouseSpeed = 0.005f;
bool keys[1024];
float lastTime;
float deltaTime;
//cursor parameter
bool mouse[20] = {false};
bool firstPush = true;
float preRotX,preRotY,RotX,RotY;
float rotateSpeed = 0.01;

//parameter for shaders
bool bump = false;
float swich =1.0f;
bool fixCamera;

//create lamp object
glm::vec3 lightPosition;
bool lampIsPicked;
glm::vec3 NewPosition;

//detection radius
int NumOfCube = 4;
const float sphere_radius = 0.9f;
int selected_cube = -1;


static void error_callback(int error, const char* description){
	fprintf(stderr, "Error %d: %s\n", error, description);
}


//initian windows
bool initWindow()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		std::cout << "ERROR: Could not initialise GLFW...";
		std::cin;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	 window = glfwCreateWindow(window_width, window_height, "physics", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::cout << "ERROR: Could not create winodw...";
		std::cin;
		return false;
	}

	glfwMakeContextCurrent(window);

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		std::cout << "ERROR: Problem initialising GLEW: " << glewGetErrorString(err);
		std::cin;
		return false;
	}


	//set up imgui binding
	ImGui_ImplGlfw_Init(window,true);
	



	//glViewport(0, 0, window_width, window_height);

	return true;
}
//Camera caculation
void computeMatricesFromInputs()
	{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	// Compute new orientation
	horizontalAngle += mouseSpeed * deltaTime * float(window_width/2 - xpos );
	verticalAngle   += mouseSpeed * deltaTime * float( window_height/2 - ypos );
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3 (
    cos(verticalAngle) * sin(horizontalAngle),
    sin(verticalAngle),
    cos(verticalAngle) * cos(horizontalAngle)
	);
	// Right vector

	rightv = glm::normalize(glm::vec3(glm::cross(direction,glm::vec3(0.0,1.0,0.0))));
	// Up vector : perpendicular to both direction and right
	up = glm::cross( rightv, direction );

	}
glm::mat4 getViewMatrix()
{
	// Camera matrix
	glm::mat4 ViewMatrix = glm::lookAt(
    Camposition,           // Camera is here
    Camposition+direction, // and looks here : at the same position, plus "direction"
    up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix()
{ 
	 glm::mat4 projectionMatrix = glm::perspective(FoV, float(window_width)/float(window_height), 1.0f, 200.0f);
	 return projectionMatrix;
}
//input calls
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	if(FoV>=1.0f && FoV<=45.0f)
	{
	FoV -= 0.1f* yoffset;
	}
	if (FoV<1.0f)
	{
		FoV=1.0f;
	}
	if (FoV>45.0f)
	{
		FoV = 45.0f;
	}
	std::cout<<"yoffset "<<yoffset<<std::endl;
	std::cout<<"Fov IS "<<FoV<<std::endl;
} 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;

	if(action == GLFW_PRESS && key == GLFW_KEY_F){
		fixCamera = ! fixCamera;
	}

	if(action == GLFW_PRESS && key == GLFW_KEY_P){
		isPause = ! isPause;
	}

	if(action == GLFW_PRESS && key == GLFW_KEY_B){
		isDamping = ! isDamping;
	}

	if(action == GLFW_PRESS && key == GLFW_KEY_G){
		isGravity = ! isGravity;
	}
	if(action == GLFW_PRESS && key == GLFW_KEY_L){
		isSpring = ! isSpring;
	}
	if(action == GLFW_PRESS && key == GLFW_KEY_V){
		isForce = ! isForce;
	}
	if(action == GLFW_PRESS && key == GLFW_KEY_C){
	 isConvergen =! isConvergen;
	}
}
void mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{

	if(action == GLFW_PRESS)
		mouse[button] = true;
	else if (action == GLFW_RELEASE)
		mouse[button] = false;

}
// Input action
void KeyAction()
{
		if( keys[GLFW_KEY_W])
	{
		 Camposition += direction * deltaTime * speed;
	}
	if( keys[GLFW_KEY_S])
	{
		 Camposition -= direction * deltaTime * speed;
	}
	if( keys[ GLFW_KEY_D])
	{
		Camposition += rightv * deltaTime * speed;
	}
	if(   keys[ GLFW_KEY_A] )
	{
		 Camposition -= rightv * deltaTime * speed;
	}

	if( keys[ GLFW_KEY_LEFT_SHIFT] ){
			speed = 12.0f;
	}
	else{
			speed = 4.0f;
	}

	//key behavior for bump-map
	if(keys[GLFW_KEY_N])
	{
		bump = 1.0 ;
	}
	
	if (keys[GLFW_KEY_M])
	{
		bump = 0.0;
	}
	
	//key behavior for reflection
	if (keys[GLFW_KEY_1])//reflection
	{
		swich = 1.0;
	}
	if (keys[GLFW_KEY_2])//refraction
	{
		swich = 2.0;
	}
	if (keys[GLFW_KEY_3])//combine
	{
		swich = 3.0;
	}




}
void MouseAction()
{
	if (mouse[GLFW_MOUSE_BUTTON_LEFT])
	{
		double xpos = 0 , ypos= 0 ;
		float Ori_rotateX = 0.0, Ori_rotateY = 0.0;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (firstPush)//the first push
		{
			firstPush = false;
			Ori_rotateX = xpos;
			Ori_rotateY = ypos;
			//std::cout<<"X: "<<xpos<<" , "<<"Y: "<<ypos<<std::endl;
		}
		else//listen course position when pushing
		{
			RotX = (xpos - Ori_rotateX) * rotateSpeed;
			RotY = (ypos - Ori_rotateY) * rotateSpeed;
			//std::cout<<"RotX: "<<RotX<<" , "<<"RotY: "<<RotY<<std::endl;
		}
	}
	else if (!mouse[GLFW_MOUSE_BUTTON_LEFT])
	{
		firstPush = true;
		preRotX += RotX;
		preRotY += RotY;
		RotX = 0;
		RotY = 0;
	}
	//use right button to pick cubes
	/*
	if(mouse[GLFW_MOUSE_BUTTON_RIGHT]){
		double xpos = 0 , ypos= 0 ;
		glfwGetCursorPos(window, &xpos, &ypos);
		//get the ray in world coords
		glm::vec3 ray_world = get_ray_from_mouse(xpos,ypos);
		// check ray against all spheres in scene
		int closest_sphere_clicked = -1;
		float closest_intersection = 0.0f;
		for (int i = 0; i < 4; i++) {
			float t_dist = 0.0f;
			if (ray_sphere (
				Camposition, ray_world, cubes_pos_wor[i], sphere_radius, &t_dist
			)) {
				// if more than one sphere is in path of ray, only use the closest one
				if (-1 == closest_sphere_clicked || t_dist < closest_intersection) {
					closest_sphere_clicked = i;
					closest_intersection = t_dist;
				}
			}
		} // endfor
		selected_cube = closest_sphere_clicked;
		//set the position of the sphere as mouse;
		//sphere_pos_wor[g_selected_sphere] = ray_wor;
		printf ("sphere %i was clicked\n", closest_sphere_clicked);
	}
	*/
}

//main loop
int main()
{


	/* Create GL Window */
	if (!initWindow())
		return -1;
	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//set some windows parameters

	bool show_test_window = true;
    bool show_another_window = false;
	ImVec4 clear_color = ImColor(114,144, 154);

	
	// Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
	glfwSetScrollCallback(window,scroll_callback);
	glfwSetKeyCallback(window,key_callback);
	glfwSetMouseButtonCallback(window,mousebutton_callback);
	

	//shader for the floor 
	Shader simpleShader;
	simpleShader.loadShadersFromFiles("../Shaders/simpleShader.vert","../Shaders/simpleShader.frag");
	
	//shader for the particle
	Shader particleShader;
	particleShader.loadShadersFromFiles("../Shaders/particleShader.vert","../Shaders/particleShader.frag");
	
	//shader for the rigidbod
	Shader rigidShader,aabbShader, GJKShader;
	rigidShader.loadShadersFromFiles("../Shaders/rigidbody.vert","../Shaders/rigidbody.frag");
	aabbShader.loadShadersFromFiles("../Shaders/rigidbody.vert","../Shaders/rigidbody.frag");
	GJKShader.loadShadersFromFiles("../Shaders/gjkShader.vert","../Shaders/gjkShader.frag");

	//initiate paritcle simulation
	//particleSimulater particle_system(NUM_PARTICLE);
	//particle_system.renderBegin();

	//initialise rigidbody simulation
	RigidBodySim m_simulation;
	m_simulation.InitStates();

	glm::mat4 model_particle;

	// Wireframe mode
	

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		ImGui_ImplGlfw_NewFrame();
		
		
		double currentTime = glfwGetTime();
		deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;

		//Compute the MV matrix from the keyboard and mouse
		KeyAction();
		MouseAction();
		if ( fixCamera == false)//when dragging, when picking, fix the camera
		{
			computeMatricesFromInputs();
		}
		projectionMatrix = getProjectionMatrix();
		ViewMatrix = getViewMatrix();

		glViewport(0, 0, window_width, window_height);        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		//enable point size
		glEnable(GL_PROGRAM_POINT_SIZE);
		
		
		/*
		//add a attractor 
		if(!keys[GLFW_KEY_B])
		{
			for(size_t i=0; i<particle_system.m_attractor->collectionSize(); i++)
			{
				glm::vec4 localAttrac = particle_system.m_attractor->get(i);
				localAttrac = glm::rotate(localAttrac,deltaTime * 0.2f,glm::vec3(0.0f,1.0f,0.0f));
				if (keys[GLFW_KEY_UP])
				{
					localAttrac.y +=  deltaTime * 4.0f;
				}
				if (keys[GLFW_KEY_DOWN])
				{
					localAttrac.y -=  deltaTime * 4.0f;
				}
				particle_system.m_attractor->get(i) = localAttrac;
				
			}
		}
		if(!isGravity){
			particle_system.m_eulerUpdater->m_globalAcceleration = glm::vec4(0);
		}
		else
		{
			particle_system.m_eulerUpdater->m_globalAcceleration = glm::vec4( 0.0, -3.0, 0.0, 0.0 );
		}
		*/
		/*does not work at all 
		{
		static float f = 0.0f;
		ImGui::Begin("My window");
		ImGui::Text("hello, world!");
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		}
		*/


		/*
		//prepare shader
		particleShader.enableShader();
		glm::mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(GJKShader.shaderProgramID,"model"),1,GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(GJKShader.shaderProgramID, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(GJKShader.shaderProgramID, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);

		// reuse the shader for rigid body
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//begin particle system
		if(!isPause){
		particle_system.updateSys(deltaTime);
		}
		
		particle_system.renderUpdate();
		particle_system.render();
		*/
		
		if(!isPause){
		m_simulation.stateUpdate(currentTime,0.005f);//deltaTime);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		GJKShader.enableShader();
		glm::mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(GJKShader.shaderProgramID,"model"),1,GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(GJKShader.shaderProgramID, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(GJKShader.shaderProgramID, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
		if (isPause)
		{
			m_simulation.renderGJK(GJKShader.shaderProgramID);
		}
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		aabbShader.enableShader();
		glUniformMatrix4fv(glGetUniformLocation(aabbShader.shaderProgramID,"model"),1,GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(aabbShader.shaderProgramID, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(aabbShader.shaderProgramID, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
		if(!isPause){
		//m_simulation.renderAABB(aabbShader.shaderProgramID);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		
		rigidShader.enableShader();
		glUniformMatrix4fv(glGetUniformLocation(rigidShader.shaderProgramID,"model"),1,GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(rigidShader.shaderProgramID, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(rigidShader.shaderProgramID, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
		m_simulation.render(rigidShader.shaderProgramID);
		

		

		

		//render floor
		simpleShader.enableShader();
		RenderScene(simpleShader);//render floor
		
		// std::cout<<particle_system.m_system->numAliveParticles()<<std::endl;

		//rendering window 
		/*
        glViewport(0, 0, window_width, window_height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		*/
		glfwSwapBuffers(window);
		
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}



void RenderScene(Shader &shader)
{
	
	 // Floor
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 model;
	model = glm::scale(model,glm::vec3(WALL_BOX_WIDTH,WALL_BOX_WIDTH,WALL_BOX_WIDTH));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, "model"), 1, GL_FALSE, &model[0][0]);
	
	Cube m_outside;
	m_outside.render(shader.shaderProgramID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	model = glm::mat4(1.0f);
	model = glm::translate(model,glm::vec3(0.0f,-10.0f,0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgramID, "model"), 1, GL_FALSE, &model[0][0]);
	Floor floor;
	floor.render();
   
	
}


