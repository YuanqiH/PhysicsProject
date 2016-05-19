#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>
#include <vector>
#include<glm\gtx\rotate_vector.hpp>
#include <iostream>
#include "Shape.hpp" 

#define GJK_NUM_ITERATIONS 50

typedef void (* Contactcallback)(glm::vec3, glm::vec3, glm::vec3, float);

struct SupportPoint
{
	glm::vec3 v;
	glm::vec3 sup_a;
	glm::vec3 sup_b;

	bool operator==(const SupportPoint &r) const{ return v ==r.v;}
};

struct Simplex4{ // this structure could be replaced by list ?
	//glm::vec3 a,b,c,d;
	SupportPoint a,b,c,d;
	int number;
};

struct contactInfo{
	glm::vec3 m_pointa,m_pointb,m_normal;
	float depth;
};

class GJK
{
public:
	GJK();
	~GJK();

	bool CollisionDetection(Shape *shape_1, Shape *shape_2);
	void drawContactPoint(GLint shaderID);
	contactInfo m_contactInfo;

private:
	Simplex4 m_simplex4;
	Shape *m_shape1, *m_shape2;
	SupportPoint support(glm::vec3);
	glm::vec3 doubleCross(glm::vec3, glm::vec3);

	bool ContainsOrigin(glm::vec3&);
	bool triangle(glm::vec3&);
	bool tetrahedron(glm::vec3&);
	bool checkTetrahedron(const glm::vec3&,const  glm::vec3&,const  glm::vec3&, const glm::vec3&, glm::vec3& );
	bool EPAcheck(glm::vec3 dir);
	void barycentric(const glm::vec3 &p,const glm::vec3 &a,const glm::vec3 &b,const glm::vec3 &c,float *u,float *v,float *w);
	
	void setContactInfo(glm::vec3, glm::vec3, glm::vec3, float);
	
};

GJK::GJK()
{
}

GJK::~GJK()
{
}

bool GJK::CollisionDetection(Shape *shape_1, Shape *shape_2){
	m_shape1 = shape_1;
	m_shape2 = shape_2;
	glm::vec3 dir = glm::vec3(1.0f,0.0f,0.0f); // initial a random dirction
	m_simplex4.c = support(dir);
	dir = - m_simplex4.c.v; // towards original
	m_simplex4.b = support(dir);
	if(glm::dot(dir,m_simplex4.b.v) < 0) // no need to check further 
		return false;
	dir = doubleCross( m_simplex4.c.v- m_simplex4.b.v , - m_simplex4.b.v); // get the direction towards original
	m_simplex4.number = 2; 

	// doing loop
	int steps = 0;
	while (steps++ < GJK_NUM_ITERATIONS)
	{
		m_simplex4.a = support(dir); // each loop add a new A to simplex
		
		if (glm::dot(m_simplex4.a.v,dir) < 0) //checking original 
		{
			return false;
		}
		else
		{
			if (ContainsOrigin(dir)) // if the tetrahedron is found by GJK
			{ 
				EPAcheck(dir);
				return true;
			}
		}
	}
	cout<<"doing loop : "<< steps<< "times"<<endl;
	return true; // could do contactPoint later on
}

bool GJK::ContainsOrigin(glm::vec3& dir){
	if (m_simplex4.number == 2) // 3 points inside simplex 
	{
		return triangle(dir);
	}
	else if (m_simplex4.number == 3) // 4 points inside simplex
	{
		return tetrahedron(dir);
		//return tetrahedron_op(dir);
	}

	return false;
}

bool GJK::triangle(glm::vec3& dir){
	 glm::vec3 ao = -m_simplex4.a.v;
	 glm::vec3 ab = m_simplex4.b.v - m_simplex4.a.v;
	 glm::vec3 ac = m_simplex4.c.v - m_simplex4.a.v;
	 glm::vec3 abc = glm::cross(ab, ac);

	 //point is can't be behind/in the direction of B,C or BC
	 glm::vec3  ab_abc = glm::cross(ab, abc);
	 	 
	 //if a0 is in that direction than
	 if (glm::dot(ab_abc,ao) > 0)
	 {
		//change points
		//m_simplex4.c = m_simplex4.b;
		//m_simplex4.b = m_simplex4.a;
		m_simplex4.c = m_simplex4.a;
		//dir is not ab_abc because it's not point towards the origin
		dir = doubleCross(ab,ao);

		//direction change; can't build tetrahedron
		return false;
	 }

	
	 glm::vec3 abc_ac = glm::cross(abc, ac); 

	 // is the origin away from ac edge? or it is in abc?
	 //if a0 is in that direction than
	 if (glm::dot(abc_ac,ao) > 0)
	 {
		//keep c the same
		m_simplex4.b = m_simplex4.a;

		//dir is not abc_ac because it's not point towards the origin
		dir = doubleCross(ac, ao);
				
		//direction change; can't build tetrahedron
		return false;
	 }


	 //now can build tetrahedron; check if it's above or below
	 if (glm::dot(abc,ao) > 0)
	 {
		 //base of tetrahedron
		 m_simplex4.d = m_simplex4.c;
		 m_simplex4.c = m_simplex4.b;
		 m_simplex4.b = m_simplex4.a;

		 //new direction
		 dir = abc;
	 }
	 else 
	 {
		 if (glm::dot(abc,ao) == 0)
			 //return true;
		 //upside down tetrahedron
		 m_simplex4.d = m_simplex4.b;
		 m_simplex4.b = m_simplex4.a;
		 dir = -abc;
	 }

	 m_simplex4.number = 3;
	
	 return false;
 }



 bool GJK::tetrahedron(glm::vec3& dir)
 {
	 glm::vec3 ao = -m_simplex4.a.v;//0-a
	 glm::vec3 ab = m_simplex4.b.v - m_simplex4.a.v;
	 glm::vec3 ac = m_simplex4.c.v - m_simplex4.a.v;
	 
	 //build abc triangle
	 glm::vec3 abc = glm::cross(ab, ac);

	 //CASE 1
	 if (glm::dot(abc,ao) > 0)
	 {
		 //in front of triangle ABC
		 //we don't have to change the ao,ab,ac,abc meanings
		return checkTetrahedron(ao,ab,ac,abc,dir);
	 }
	 

	 //CASE 2:
	 
	 glm::vec3 ad = m_simplex4.d.v - m_simplex4.a.v;

	 //build acd triangle
	  glm::vec3 acd = glm::cross(ac, ad);

	 //same direaction with ao
	 if (glm::dot(acd,ao) > 0)
	 {

		 //in front of triangle ACD
		 m_simplex4.b = m_simplex4.c;
		 m_simplex4.c = m_simplex4.d;
		 ab = ac;
		 ac = ad;
		 abc = acd;

		return checkTetrahedron(ao, ab, ac, abc, dir);
	 }

	 //build adb triangle
	 glm::vec3 adb = glm::cross(ad, ab);

	 //same direaction with ao
	 if (glm::dot(adb,ao) > 0)
	 {

		 //in front of triangle ADB

		 m_simplex4.c = m_simplex4.b;
		 m_simplex4.b = m_simplex4.d;

		 ac = ab;
		 ab = ad;

		 abc = adb;
		return checkTetrahedron(ao, ab, ac, abc, dir);
	 }


	 //origin in tetrahedron
	 return true;

 }

 bool GJK::checkTetrahedron(const glm::vec3& ao,
									  const glm::vec3& ab,
									  const glm::vec3& ac,
									  const glm::vec3& abc,
									  glm::vec3& dir)
 {
	 
	//almost the same like triangle checks
	glm::vec3 ab_abc = glm::cross(ab, abc);

	 if (glm::dot(ab_abc,ao) > 0)
	 {
		 //m_simplex4.c = m_simplex4.b;
		 //m_simplex4.b = m_simplex4.a;
		 m_simplex4.c = m_simplex4.a;

		 //dir is not ab_abc because it's not point towards the origin;
		 //ABxA0xAB direction we are looking for
		 dir = doubleCross(ab, ao);
		 
		 //build new triangle
		 // d will be lost
		 m_simplex4.number = 2;

		 return false;
	 }

	 glm::vec3 acp = glm::cross(abc, ac);

	 if (glm::dot(acp,ao) > 0)
	 {
		 m_simplex4.b = m_simplex4.a;

		 //dir is not abc_ac because it's not point towards the origin;
		 //ACxA0xAC direction we are looking for
		 dir = doubleCross(ac, ao);
		 
		 //build new triangle
		 // d will be lost
		 m_simplex4.number = 2;

		 return false;
	 }

	 //build new tetrahedron with new base
	 m_simplex4.d = m_simplex4.c;
	 m_simplex4.c = m_simplex4.b;
	 m_simplex4.b = m_simplex4.a;

	 dir = abc;

	 m_simplex4.number = 3;

	 return false;
 }


 bool GJK::EPAcheck(glm::vec3 dir){
	
	 struct Triangle
	 {
		 SupportPoint points[3];
		 glm::vec3 n;

		 Triangle( const SupportPoint &a, const SupportPoint &b, const SupportPoint &c){
			points[0] = a;
			points[1] = b;
			points[2] = c;
			n = glm::normalize( glm::cross(	(b.v-a.v), (c.v-a.v) ) );
		 }
	 };

	 struct  Edge
	 {
		SupportPoint points[2];
		 Edge(const SupportPoint &a, const SupportPoint &b){
			points[0] = a;
			points[1] = b;
		 }
	 };

	 const float EXIT_THRESHOLD = 0.001f;
	 const unsigned EXIT_ITERAT_LIMIT = 50;
	 unsigned	EXIT_ITERATE_CUR = 0;
	 unsigned ITERATION_NUM = 0;
	 std::list<Triangle> list_triangles;
	 std::list<Edge> list_edges;

	 auto addEdge = [&](const SupportPoint &a, const SupportPoint &b) -> void {
		for(auto it = list_edges.begin(); it != list_edges.end(); it++){
			if(it->points[0] == b && it->points[1] == a){
				// if opposite edge found, remove it 
				list_edges.erase(it);
				return;
			}
		}
		list_edges.emplace_back(a,b);
	 };

	 //put all the 4 faces into list of triangles
	 list_triangles.emplace_back(m_simplex4.a,m_simplex4.b,m_simplex4.c);
	 list_triangles.emplace_back(m_simplex4.a,m_simplex4.c,m_simplex4.d);
	 list_triangles.emplace_back(m_simplex4.a,m_simplex4.d,m_simplex4.b);
	 list_triangles.emplace_back(m_simplex4.b,m_simplex4.d,m_simplex4.c);

	 while (true)
	 {
		 if (ITERATION_NUM++ >= EXIT_ITERAT_LIMIT){ 
			 return false;
			cout<<"debug!! iteration exceed !!"<<endl; 
		 }
		 //find closest triangle to origin
		std::list<Triangle>::iterator entry_current_triangle_it = list_triangles.begin();
		float entry_current_dist = FLT_MAX;
		for (auto it = list_triangles.begin(); it != list_triangles.end(); it++){
			float dst = glm::abs(glm::dot(it->n,it->points[0].v));
			if (dst< entry_current_dist)
			{
				entry_current_dist = dst;
				entry_current_triangle_it = it;
			}
		}

		SupportPoint entry_current_support = support(entry_current_triangle_it->n);
		if (glm::dot(entry_current_support.v, entry_current_triangle_it->n) - entry_current_dist < EXIT_THRESHOLD)
		{
			//calculate the barycentric coordinates of the closest triangle 
			float bary_u,bary_v,bary_w;
			barycentric(entry_current_triangle_it->n * entry_current_dist,
						entry_current_triangle_it->points[0].v,
						entry_current_triangle_it->points[1].v,
						entry_current_triangle_it->points[2].v,
						&bary_u,
						&bary_v,
						&bary_w);

			//collision point on object A in world space
			glm::vec3 contact_Point_a((bary_u * entry_current_triangle_it->points[0].sup_a) +
								(bary_v * entry_current_triangle_it->points[1].sup_a) +
								(bary_w * entry_current_triangle_it->points[2].sup_a));
		
			glm::vec3 contact_Point_b((bary_u * entry_current_triangle_it->points[0].sup_b) +
								(bary_v * entry_current_triangle_it->points[1].sup_b) +
								(bary_w * entry_current_triangle_it->points[2].sup_b));


			//collision normal
			glm::vec3 wcolNormal =  - entry_current_triangle_it->n;

			float wpenDepth = entry_current_dist;

			//try to use callback, but fail 
			setContactInfo(contact_Point_a, contact_Point_b , wcolNormal, wpenDepth);

			//cout<<"the point a is : "<< "( " <<contact_Point_a.x <<contact_Point_a.y << contact_Point_a.z<< ")"<< endl;
			//cout<<"the point b is : "<< "( " <<contact_Point_b.x <<contact_Point_b.y << contact_Point_b.z<< ")"<< endl;
			cout<<"the depth is "<< wpenDepth << endl;

			break;
		}

		//check exist edges
		for (auto it = list_triangles.begin(); it != list_triangles.end() ;)
		{
			if ( glm::dot(it->n,(entry_current_support.v - it->points[0].v)) > 0 )
			{
			addEdge(it->points[0],it->points[1]);
			addEdge(it->points[1],it->points[2]);
			addEdge(it->points[2],it->points[0]);
			it = list_triangles.erase(it);
			continue;
			}
			it++;
		}

		// create new triangle list from the edges
		for(auto it = list_edges.begin(); it != list_edges.end(); it++){
			list_triangles.emplace_back(entry_current_support, it->points[0],it->points[1]);
		}

		list_edges.clear();

	 }// end of while

 }

 void GJK::barycentric(const glm::vec3 &p,const glm::vec3 &a,const glm::vec3 &b,const glm::vec3 &c,float *u,float *v,float *w) {
     // code from Crister Erickson's Real-Time Collision Detection
     glm::vec3 v0 = b - a,v1 = c - a,v2 = p - a;
     float d00 = glm::dot(v0,v0);
     float d01 = glm::dot(v0,v1);
     float d11 = glm::dot(v1,v1);
     float d20 = glm::dot(v2,v0);
     float d21 = glm::dot(v2,v1);
     float denom = d00 * d11 - d01 * d01;
     *v = (d11 * d20 - d01 * d21) / denom;
     *w = (d00 * d21 - d01 * d20) / denom;
     *u = 1.0f - *v - *w;
};

SupportPoint GJK::support(glm::vec3 dir){
	glm::vec3 d = glm::normalize(dir);
	SupportPoint ret;
	ret.sup_a = m_shape1->getSupporting(d);
	ret.sup_b = m_shape2->getSupporting(-d);
	ret.v = ret.sup_a- ret.sup_b;
	return ret;
}

glm::vec3 GJK::doubleCross(glm::vec3 a, glm::vec3 b){
	return glm::cross(glm::cross(a,b),a);
}

void GJK::drawContactPoint(GLint shaderID){
	
	GLfloat position[] = {
		m_contactInfo.m_pointa.x,m_contactInfo.m_pointa.y,m_contactInfo.m_pointa.z,
		m_contactInfo.m_pointa.x + m_contactInfo. m_normal.x,m_contactInfo.m_pointa.y+ m_contactInfo.m_normal.y,m_contactInfo.m_pointa.z+m_contactInfo.m_normal.z,
		//m_contactInfo.m_pointb.x,m_contactInfo.m_pointb.y,m_contactInfo.m_pointb.z
		
	};
	GLuint vao,vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

	GLint position_attribute = glGetAttribLocation(shaderID, "position");

	glBindVertexArray(vao);
	glEnableVertexAttribArray(position_attribute);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, 2); // maybe better visulisation 
	glBindVertexArray(0);
}

void GJK::setContactInfo(glm::vec3 pointa, glm::vec3 pointb, glm::vec3 normal, float dep){
	this->m_contactInfo.m_pointa = pointa;
	this->m_contactInfo.m_pointb = pointb;
	this->m_contactInfo.m_normal = normal;
	this->m_contactInfo.depth = dep;
}