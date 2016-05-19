#pragma once

#include <vector>
#include<glm\gtx\rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL\glew.h>
#include <glm\gtx\quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "AABB.hpp" 
#include "GJK.hpp" 

//#define NBODIES 2
#define STATE_SIZE 13	// 3+9+3+3 =18 / 13 for quarternion
bool isQuaternion = true;
//debug
bool isPause = false;
bool isDamping = true; 
bool isGravity = false;
bool isSpring = false;
bool isForce =false;
bool isConvergen = false;

const float Penetrating = 1.0f;
const float Colliding = 2.0f;
const float Nokdl = 0.002;
const float Nokda = 0.001;
const float Kdl = 0.4;
const float Kda = 0.4;
const float depthEpsilon = 0.05f;


struct wallContact{
	glm::vec3 normal;
	glm::vec3 cornerPoint;
	glm::vec3 Wallpoint;
	float d;
};
wallContact wallcontact[NBODIES];

struct rbPhysics
{
	glm::vec3 applyPoint; // apply point
	glm::vec3 force;
	glm::vec3 gravity;
	glm::vec3 J; // impulse magnitude
	glm::vec3 r; // impulse relative radius
};

rbPhysics Forces[NBODIES];

struct Rigidbody
{
	// constant quantites
	double mass;	//mass 
	double mass_inv; 
	glm::mat3 Ibody; //Ibody
	glm::mat3 Ibodyinv; // inverse of Ibody
	
	// state variables
	glm::vec3 x; // x(t)
//	glm::mat3 R; // R(t)
	glm::quat q; // Q(t) using quaternion
	glm::vec3 P; // P(t)
	glm::vec3 L; // L(t)

	// Derived quatities ( auxiliary variables)
	glm::mat3 Iinv; // I−1(t) inverse
	glm::mat3 R; // R from quaternion
	glm::vec3 V; // V(t)
	glm::vec3 omega; // ω(t) anguler velocity

	// Computed quantities
	glm::vec3 force; // F(t)
	glm::vec3 torque; // τ(t)

	bool isCollided;
	bool isNarrowCollided;
	float collisionState;
};

Rigidbody Bodies[NBODIES];

//copy the state information into an array
void StateToArray(Rigidbody *rb, double *y)
{

	*y++ = rb->x.x; // x component of position
	*y++ = rb->x.y;
	*y++ = rb->x.z;

	/*
	for(int i = 0; i<3; i++){ // copy rotation matrix
		for(int j =0; j<3 ; j++){
			*y++ = rb-> R[i][j]; 
		}
	}
	*/
	*y++ = rb->q.w;
	*y++ = rb->q.x;
	*y++ = rb->q.y;
	*y++ = rb->q.z;

	*y++ = rb->P.x; // copy the P(t)
	*y++ = rb->P.y;
	*y++ = rb->P.z;

	*y++ = rb->L.x;
	*y++ = rb->L.y;
	*y++ = rb->L.z;

}

// copy information from the array iinto the state varables
void ArrayToState(Rigidbody *rb, double *y){

	rb->x.x = *y++;
	rb->x.y = *y++;
	rb->x.z = *y++;

	/*
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			rb->R[i][j] = *y++;
	*/

	rb->q.w = *y++;
	rb->q.x = *y++;
	rb->q.y = *y++;
	rb->q.z = *y++;

	rb->P.x = *y++;
	rb->P.y = *y++;
	rb->P.z = *y++;

	rb->L.x = *y++;
	rb->L.y = *y++;
	rb->L.z = *y++;

	//Compute auxiliary varable 
	//Compute velocity
	rb->V = rb->P * (float)rb->mass_inv;

	//calculate Inertial tensor 
	rb->Iinv = rb->R * rb-> Ibodyinv * glm::transpose(rb-> R);

	//compute angular velocity
	rb->omega = rb->Iinv * rb->L;

	rb->R = glm::mat3(glm::toMat4(glm::normalize(rb->q))); //do we need to normalize the quaternion?
}

void ArrayToBodies(double x[]){
	
	for(int i = 0; i< NBODIES; i++){
		ArrayToState(&Bodies[i], &x[ i * STATE_SIZE]);
	}
}

void BodiesToArray(double x[]){
	for (int i =0; i < NBODIES; i++)
	{
		StateToArray(&Bodies[i], &x[i * STATE_SIZE]);
	}
}

void ComputeForceAndTorque(double t, Rigidbody *rb,int index){
	


	rb->force = Forces[index].force * 4.0f ;
	rb->torque = glm::cross((Forces[index].applyPoint),Forces[index].force);
	
	if(isForce){
		rb->force += 100.f* glm::normalize(rb->x); //glm::sphericalRand(100.0f);	
	}
	if (isConvergen)
	{
		rb->force -= 100.f* glm::normalize(rb->x);
	}

	if (isGravity)
	{
		rb->force += Forces[index].gravity * (float)rb->mass_inv;
	}

	if (isDamping)
	{
		rb->force += -Kdl * rb->V;
		rb->torque += -Kda * rb->omega;
	}
	else
	{
		rb->force += -Nokdl * rb->V;
		rb->torque += -Nokda * rb->omega;
	}

	if (isSpring)
	{
		float kbs = 0.8f;
		glm::vec3 springVector = Bodies[0].x - Bodies[1].x;
		Bodies[0].force += -kbs * springVector;
		Bodies[1].force -= -kbs * springVector;
	}


}

glm::mat3 Star(glm::vec3 a){
	
	glm::mat3 result = glm::mat3(0.0f);
	result[0][1] = - a.z;// a[2]
	result[0][2] =  a.y; // a[]
	result[1][0] =	a.z;
	result[1][2] = - a.x; // a[0]
	result[2][0] = - a.y; // -a[1]
	result[2][1] =	a.x;
	return result;
} // for calculating omega
glm::mat3 orthonormalize(glm::mat3 r) //use othonormal after update of R
{
	glm::mat3 result;
	//get the colume of the matrix
	glm::vec3 v1 = glm::vec3(r[0][0], r[0][1], r[0][2]);
	glm::vec3 v2 = glm::vec3(r[1][0], r[1][1], r[1][2]);
	glm::vec3 v3 = glm::vec3(r[2][0], r[2][1], r[2][2]);
 
 
	v1 = glm::normalize(v1);
	v2 = glm::cross(v3, v1);
	v2 = glm::normalize(v2);
	v3 = glm::cross(v1, v2);
	v3 = glm::normalize(v3);
 
	result = glm::mat3(v1,v2,v3);
	/*
	result[0][0] = v1.x;	result[0][1] = v2.x;	result[0][2] = v3.x;
	result[1][0] = v1.y;	result[1][1] = v2.y;	result[1][2] = v3.y;
	result[2][0] = v1.z;	result[2][1] = v2.z;	result[2][2] = v3.z;
	 */
	return result;
}

void DdtStateToArray ( Rigidbody *rb, double *xdot){
	/* copy ddt x(t) = v(t) into xdot */
	*xdot++ = rb->V.x;
	*xdot++ = rb->V.y;
	*xdot++ = rb->V.z;

	if(isQuaternion){
		
		glm::quat localOmega = glm::quat(0.0f, rb->omega.x, rb->omega.y,rb->omega.z);
		glm::quat qdot = localOmega * rb->q; // cant apply 0.5f as said in textbook

		*xdot++ = qdot.w;
		*xdot++ = qdot.x;
		*xdot++ = qdot.y;
		*xdot++ = qdot.z;

	}
	else{
	/* Compute R˙(t) = ω(t)∗R(t) */
	glm::mat3 Rdot = Star(rb->omega) * rb->R;

	/* copy R˙(t) into array */
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			*xdot++ = Rdot[i][j];
	}

	*xdot++ = rb->force.x; /* ddt P(t) = F(t) */
	*xdot++ = rb->force.y;
	*xdot++ = rb->force.z;

	*xdot++ = rb->torque.x; /* ddt L(t) = τ(t) */
	*xdot++ = rb->torque.y;
	*xdot++ = rb->torque.z;

}

//called by solver, this function compute derivative of x
//T has to be provied for the computation of the force
void Dxdt(double t, double x[], double xdot[]){
	
	// get the V and omega for further calculation can be ignored 
	ArrayToBodies(x); 

	for (int i = 0; i < NBODIES; i++)
	{
		ComputeForceAndTorque(t, &Bodies[i],  i);
		DdtStateToArray(&Bodies[i], &xdot[i * STATE_SIZE]);
	}

}

typedef void (*DerivFunc) ( double t, double x[], double xdot[]);

//compute the state vector at time t1 and return it in the array xEnd
void ode( double x0[], double xEnd[], int len, double t0, double t1, DerivFunc Dxdt ){

	double xdot[STATE_SIZE * NBODIES];
	double localDT = t1 - t0;
	// save xdot into array xdot
	Dxdt(t0, x0 , xdot);

	//compute the next state of the array 
	for (int i = 0; i < NBODIES; i ++)
	{
		for (int j = 0; j < STATE_SIZE; j++)
		{
			//using euler integration
			xEnd[STATE_SIZE*i+j] = x0[STATE_SIZE*i+j] + xdot[STATE_SIZE*i+j] * localDT;
		}
	}
}



#define SIMULATE_TIME 10.0


class RigidBodySim
{
public:

	RigidBodySim(){};
	void InitStates();
	
	void stateUpdate(double t, double dt);
	void render(GLint shaderID);
	void renderAABB(GLint shaderID);
	void renderGJK(GLint shaderID);

	
private:

	GLuint rigidbody_position_buffer[NBODIES], Rigidbody_color_buffer[NBODIES];
	GLuint ibo_cube_elements[NBODIES];
	GLuint m_vao[NBODIES];
	double x0[STATE_SIZE * NBODIES],
			xFinal[STATE_SIZE * NBODIES];
	GLfloat positionUpdated[3 * 8 * NBODIES];
	AABB aabb;
	Shape boxShape[NBODIES]; // for the GJK

	GJK gjkNarrow[NBODIES];
	void getWorldPosition(); // updating bodies and aabb 
	void reflectWall(); // boucing back from wall
	void flagAABB();
	void collisionResponse(int, int);
	void pushToRender();

	void checkGroundCollision(); // checking for ground
	void ResolveGroundCollisions();

	glm::vec3 lastPosition[NBODIES];
	glm::quat lastQuat[NBODIES];
	
	void renderInitial();
	void renderUpdate();

};



void RigidBodySim::renderInitial(){
	for (int i = 0; i < NBODIES; i++)
	{

	glGenVertexArrays(1, &m_vao[i]);
	glBindVertexArray(m_vao[i]);

	//set for the index
	glGenBuffers(1, &ibo_cube_elements[i]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  ibo_cube_elements[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(cube_elements),cube_elements,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// only initalise the position buffers
	// the VBO containing the position and size of the rigidbodies
	
	glGenBuffers(1, &rigidbody_position_buffer[i]);
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer[i]);
	//initialize with empty( null) buffer, it will be updated later
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set the color buffers
	glGenBuffers(1, &Rigidbody_color_buffer[i]);
	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	}

	aabb.initBox();
}

void RigidBodySim::renderUpdate(){
	//update the buffer that opengl used for rendering 
	for (int i = 0; i < NBODIES; i++)
	{
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_vertices),&positionUpdated[i*24]); // uisng the array only contain position of the vertices

	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void RigidBodySim::render(GLint shaderID){
	for (int i = 0; i < NBODIES; i++)
	{

	//send the isCollided bool to shader
	glUniform1f(glGetUniformLocation(shaderID, "isCollided"), float(Bodies[i].isCollided));
	glUniform1f(glGetUniformLocation(shaderID, "isNarrowCollided"), float(Bodies[i].isNarrowCollided));

	glBindVertexArray(m_vao[i]); // important to bind VA0 before rendering

	//prepare the buffer
	glEnableVertexAttribArray(0);// 1st attribute buffers
	glBindBuffer(GL_ARRAY_BUFFER, rigidbody_position_buffer[i]);
	glVertexAttribPointer(
		0, //attribute 0 
		3, // x+y+z = 3
		GL_FLOAT, //type
		GL_FALSE, //
		0, //stride
		(void*)0); //array buffer offset


	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Rigidbody_color_buffer[i]);
	glVertexAttribPointer(
		1,
		3,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		0,
		(void*)0);
	

	//set Index for drawing 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements[i]);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	}

}
void RigidBodySim::renderAABB(GLint shaderID){
	// have to outside of the loop
	aabb.renderBox(shaderID);
}

void RigidBodySim::InitStates(){
	for (int i = 0; i < NBODIES; i++)
	{
	
		int height = 2.0f,depth = 2.0f,width = 2.0f;

		Bodies[i].mass = 2.0;
		Bodies[i].mass_inv = 1 / Bodies[i].mass;
		Bodies[i].Ibody = glm::mat3(0.0f); // 1/6 = 1/12 * m * ( h^2 + d^2 )
		{
			Bodies[i].Ibody[0][0] = 1.0 / 12.0 * (Bodies[i].mass * ((height * height) + (depth * depth)));
			Bodies[i].Ibody[1][1] = 1.0 / 12.0 * (Bodies[i].mass * ((width * width) + (depth * depth)));
			Bodies[i].Ibody[2][2] = 1.0 / 12.0 * (Bodies[i].mass * ((width * width) + (height * height)));
		}
		Bodies[i].Ibodyinv = glm::inverse(Bodies[i].Ibody);
		//only initialise for state 
		//this will influence derivative directly 
		Bodies[i].x = /*glm::vec3(i * -5.0f,i * 0.3f,0.0f); /*/glm::sphericalRand(10.0f);// glm::vec3(0.0f,0.0f,0.0f);
		//Bodies[i].R = glm::mat3(1.0f); // each column show the x,y,z axis
		Bodies[i].q = glm::quat(1.0f,0.0f,0.0f,0.0f); // idnetity of the quaternion
		Bodies[i].P =/* glm::vec3(i,0.0f,0.0f);/*/glm::sphericalRand(0.0f); // the moving speed
		Bodies[i].L = /*Bodies[i].Ibody */ glm::sphericalRand(1.0f); // provide the anguler velocity, rotation direction and radius is speed

		Bodies[i].isCollided = false;

		//initial forces 
		Forces[i].applyPoint = glm::sphericalRand(0.3f);;
		Forces[i].force = glm::vec3(1.0f, 0.00f,0.0f);
		Forces[i].gravity = glm::vec3(0.00f, -20.00f, 0.0f);
		Forces[i].J = glm::vec3(0.0f, 0.0f,0.0f);
		
	}
		
	//send bodies data to array
	BodiesToArray(xFinal);
	renderInitial();
	
}

void copyState(double x0[],double x1[]){ // x0 <- x1
	for (int i = 0; i < STATE_SIZE * NBODIES; i++)
		{
			x0[i] = x1[i] ;
		}
}

void RigidBodySim::stateUpdate(double t,double dt){
	
	// copy xFinal back to x0
	copyState(x0,xFinal);

	float currentTime = 0;
	float TargetTime = dt;
	double tempState[STATE_SIZE * NBODIES];

	while(currentTime < dt){ // in this  while predo the collision detection until satisfied
	copyState(tempState, xFinal);
	ode(tempState,xFinal,STATE_SIZE * NBODIES,currentTime, TargetTime , Dxdt );// using solver

	for (int i = 0; i < NBODIES; i++) //save last body sate for roll back   //othonormal
	{
		//Bodies[i].R = orthonormalize(Bodies[i].R);
		if (Bodies[i].isNarrowCollided == false)
		{
		lastPosition[i] = Bodies[i].x;
		lastQuat[i] = Bodies[i].q;
		}

		//cout<<collisionState<<endl;
	}
	ArrayToBodies(xFinal);//copy state back into bodies at (t + dt) 
	getWorldPosition(); // update the position into world coord

	aabb.checkingCollision();
	flagAABB(); // do GJK here
	//checkGroundCollision();
	for (int i = 0; i < NBODIES; i++)
	{
		if(Bodies[i].collisionState == Penetrating )
		{
			// we simulate too far, roll back a little
			Bodies[i].x.y -= 2* wallcontact[i].d; 
			TargetTime =  (currentTime + TargetTime)/2; 
			break; // out for loop
		}
		else
		{
			if (Bodies[i].collisionState == Colliding )
			{
				//isPause =true;
				ResolveGroundCollisions();
				break;
			}
				currentTime = TargetTime;
				TargetTime = dt;
			
		}
	}


	}// end while


	pushToRender(); // make response more accurate before render

	renderUpdate();
}


void RigidBodySim::getWorldPosition(){
	
	for (int j = 0; j < NBODIES; j++) // for every rigidbody
	{
		boxShape[j].claer();
		float localDist = 0.0f;
			for (int i = 0; i < 8; i++) // for all the corners
		{
			glm::vec3 localPos = glm::vec3(cube_vertices[i *3 + 0], cube_vertices[i *3 + 1],cube_vertices[i *3 + 2]);
			reflectWall();
			glm::vec3 rotatePos = Bodies[j].R * localPos;
			localPos = rotatePos + Bodies[j].x; // p(t) = R(t)* p0 + x(t)
			/*
			positionUpdated[j*24 +3*i + 0] = localPos.x;
			positionUpdated[j*24 + 3*i + 1] = localPos.y;
			positionUpdated[j*24 + 3*i + 2] = localPos.z;
			*/
		// passing update bounding box data
			aabb.m_center[j] = Bodies[j].x;
			float localHalf_size = glm::abs(rotatePos.x) > glm::abs(rotatePos.y)?  glm::abs(rotatePos.x) :  glm::abs(rotatePos.y);
			localHalf_size = localHalf_size > glm::abs(rotatePos.z)? localHalf_size : glm::abs(rotatePos.z);
			localDist = localDist > localHalf_size ? localDist : localHalf_size;
		// passing the vertex for GJK
			boxShape[j].add(localPos);

			//cout << positionUpdated[j*24 +3*i + 0] <<positionUpdated[j*24 +3*i + 1]<< positionUpdated[j*24 +3*i + 2] << endl;
		}
			aabb.half_size[j] = localDist;
			//cout <<" current " << j <<" half size is" <<localDist << endl;
			Bodies[j].isCollided = false; //reset all the flag
			Bodies[j].isNarrowCollided = false;
	}
	
}

void RigidBodySim::pushToRender(){
	for (int j = 0; j < NBODIES; j++) // for every rigidbody
	{
		auto it = boxShape[j].begin();
		for (int i = 0; i < 8; i++, it++) // for all the corners
		{
		glm::vec3 localPos = *it;
		positionUpdated[j*24 +3*i + 0] = localPos.x;
		positionUpdated[j*24 + 3*i + 1] = localPos.y;
		positionUpdated[j*24 + 3*i + 2] = localPos.z;
		}
	}
}

#define WALL_BOX_WIDTH 10.0f

void RigidBodySim::reflectWall(){
	for (int i = 0; i < NBODIES; i++)
	{
		if(glm::abs(Bodies[i].x.x) > WALL_BOX_WIDTH	|| glm::abs(Bodies[i].x.y) > WALL_BOX_WIDTH || glm::abs(Bodies[i].x.z) > WALL_BOX_WIDTH){
			Forces[i].force = - 0.07f * Bodies[i].x ;}
		if (glm::abs(Bodies[i].x.x) < WALL_BOX_WIDTH && glm::abs(Bodies[i].x.y) < WALL_BOX_WIDTH && glm::abs(Bodies[i].x.z) < WALL_BOX_WIDTH){
			Forces[i].force = glm::vec3(0);}
	}
}

void RigidBodySim::flagAABB(){
	for (int i = 0; i < NBODIES; i++)
	{
		for (int j = i+1; j < NBODIES; j++)
		{
			// put all the pairs into upper triangle shape
			aabb.pairTable[i][j] += aabb.pairTable[j][i];
			aabb.pairTable[j][i] = 0;

			if (aabb.pairTable[i][j] /*+ aabb.pairTable[j][i]*/ == 3)
			{
				
				Bodies[i].isCollided = true;
				Bodies[j].isCollided = true;
				if(gjkNarrow[i].CollisionDetection(&boxShape[i],&boxShape[j])    ){
						
					Bodies[i].isNarrowCollided = true;
					Bodies[j].isNarrowCollided = true;	
					collisionResponse(i,j);
					//isPause = true;
				}
				//break;
			}
			
		}
	}

}


void RigidBodySim::renderGJK(GLint shaderID){
	for (int i = 0; i < NBODIES; i++) // quite ugly visulisation
	{
		if (Bodies[i].isNarrowCollided)
		{
			gjkNarrow[i].drawContactPoint(shaderID);
		}	
	}
}

void RigidBodySim::collisionResponse(int index_a, int index_b){ // THI DOESNT UPADTE THE RIGIDBODY!! WHAT'S WRONG?
	
	//prerpare for calculation
	glm::vec3 pa,pb,ra,rb,localNormal, p_dot_a, p_dot_b;
	pa = gjkNarrow[index_a].m_contactInfo.m_pointa;
	pb = gjkNarrow[index_b].m_contactInfo.m_pointa;
	ra = pa - aabb.m_center[index_a];
	rb = pb - aabb.m_center[index_b];

	localNormal = gjkNarrow[index_a].m_contactInfo.m_normal;
	
	p_dot_a = Bodies[index_a].V + glm::cross(Bodies[index_a].omega, ra);
	p_dot_b = Bodies[index_b].V + glm::cross(Bodies[index_b].omega, rb);

	double V_rel = glm::dot(localNormal, ( p_dot_a - p_dot_b));
	double epsilon = 1.0f;
	double part_a, part_b;
	part_a = glm::dot(localNormal, glm::cross( (Bodies[index_a].Iinv * glm::cross(ra, localNormal)) , ra) );
	part_b = glm::dot(localNormal, glm::cross( (Bodies[index_b].Iinv * glm::cross(rb, localNormal)) , rb) );
	double division = Bodies[index_a].mass_inv + Bodies[index_b].mass_inv + part_a + part_b;

	double j = -(1+epsilon) * V_rel / division;
	glm::vec3 force = (float)j * localNormal;
	//Forces[index_a].J = (float)j * localNormal;
	//Forces[index_a].r = ra;

	
	if (gjkNarrow[index_a].m_contactInfo.depth >= depthEpsilon)
	{
		//Bodies[index_a].x -= localNormal;
		//Bodies[index_b].x += localNormal;
		//Bodies[index_a].q = lastQuat[index_a];
		//Bodies[index_b].q = lastQuat[index_b];
	}
	


	Bodies[index_a].P += force * 2.0f;
	Bodies[index_b].P -= force * 2.0f;
	
	Bodies[index_a].L += glm::cross(ra, force);
	Bodies[index_b].L -=  glm::cross(rb, force);

	//compute the impulse to the bodies
	Bodies[index_a].V = Bodies[index_a].P * (float)Bodies[index_a].mass_inv;
	Bodies[index_b].V = Bodies[index_b].P * (float)Bodies[index_b].mass_inv;

	Bodies[index_a].omega = Bodies[index_a].Iinv * Bodies[index_a].L;
	Bodies[index_b].omega = Bodies[index_b].Iinv * Bodies[index_b].L;

	BodiesToArray(xFinal); // extremly important, update state again, ortherwise nothing will happen
}

void RigidBodySim::checkGroundCollision(){
	
	for (int index = 0; index < NBODIES; index++){ // checking all cube
		Bodies[index].collisionState = 0;
		for (auto it = boxShape[index].begin(); it != boxShape[index].end(); it++) // for 8 verteces of cube
		{
			glm::vec3 wallNormal = glm::vec3(0.0f,1.0f,0.0f);
			glm::vec3 position = *it;
			glm::vec3 u = position - aabb.m_center[index];
			glm::vec3 velocity = Bodies[index].V + glm::cross(Bodies[index].omega, u);

			if (Bodies[index].collisionState != Penetrating )
			{
				float d = glm::dot( (position - glm::vec3(5.0f, -10.0f,0.0f)), wallNormal );
				glm::vec3 pf = position - d* wallNormal;
				

				if (d < - depthEpsilon)
				{
					Bodies[index].collisionState = Penetrating;
					wallcontact[index].d = d;
					cout<< pf.x<< " , "<< pf.y<< " , "<< pf.z<<endl;
					cout<<d<<endl;
				}
				else if (d < depthEpsilon)
				{
					float relativeVelo = glm::dot(wallNormal, velocity);
					if (relativeVelo < 0)
					{
						Bodies[index].collisionState = Colliding;
						wallcontact[index].cornerPoint = position;
						wallcontact[index].Wallpoint = pf;
						wallcontact[index].normal = wallNormal;
					}
				}
			
			}

		}
	}

}

void RigidBodySim::ResolveGroundCollisions(){
	float e = 1.0f;
	float ea = 0.6f;
	float eu = 0.6f;
	for (int i = 0; i < NBODIES; i++)
	{
		if(Bodies[i].collisionState == Colliding){
			// apply linear impulse
			float d = glm::dot(Bodies[i].P, wallcontact[i].normal);
			float j = max(-(1+e) * d , 0.0f);
			Bodies[i].P += j* wallcontact[i].normal;

			// apply angular impluse
			glm::vec3 r = wallcontact[i].cornerPoint - aabb.m_center[i];
			glm::vec3 p_dot_a = Bodies[i].V + glm::cross(Bodies[i].omega, r);

			float ImpulseNumerator = -(1+ea) * glm::dot(p_dot_a, wallcontact[i].normal);
			float ImpulseDenominator = Bodies[i].mass_inv +  glm::dot(wallcontact[i].normal, glm::cross( (Bodies[i].Iinv * glm::cross(r, wallcontact[i].normal)) , r) );
			float ja = ImpulseNumerator/ImpulseDenominator;
			Bodies[i].L += glm::cross(r,ja * wallcontact[i].normal);

			//apply friction 
			glm::vec3 tangentDir = glm::normalize(glm::cross(Bodies[i].omega, r));
			ImpulseNumerator =  -(1+eu) * glm::dot(p_dot_a, tangentDir);
			ImpulseDenominator = Bodies[i].mass_inv +  glm::dot(tangentDir, glm::cross( (Bodies[i].Iinv * glm::cross(r, tangentDir)) , r) );
			float jt =  ImpulseNumerator/ImpulseDenominator;
			Bodies[i].L += glm::cross(r,jt * wallcontact[i].normal);
		}
	}

	BodiesToArray(xFinal); 
}