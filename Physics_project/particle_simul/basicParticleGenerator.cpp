#include "basicParticleGenerator.hpp"
#include <assert.h>
#include <algorithm>
#include <math.h>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

namespace particles
{
	namespace generators
	{
		void BoxPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			glm::vec4 posMin = glm::vec4(	m_pos.x - m_maxStartPosOffset.x,
											m_pos.y - m_maxStartPosOffset.y, 
											m_pos.z - m_maxStartPosOffset.z, 
											1.0 );

			glm::vec4 posMax = glm::vec4(	m_pos.x + m_maxStartPosOffset.x,
											m_pos.y + m_maxStartPosOffset.y, 
											m_pos.z + m_maxStartPosOffset.z, 
											1.0 );

			for (size_t i = startId; i < endId; ++i)
			{
				p->m_pos[i] = glm::linearRand(posMin, posMax);
			}
		}

		void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				double ang = glm::linearRand(0.0, M_PI*2.0);

				p->m_pos[i] = m_center + glm::vec4(m_radX*sin(ang), m_radY*cos(ang), m_radZ * cos(ang), 1.0);
			}
		}

		void SpherePosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			float phi, theta, v, r;
			for (size_t i = startId; i < endId; ++i)
			{
				phi = glm::linearRand(0.0, M_PI);
				theta = glm::linearRand(0.0, 2* M_PI);

				float r_xz = m_radius*sinf(phi);
				p->m_pos[i].z = m_sphere_center.z + r_xz*cosf(theta);
				p->m_pos[i].x = m_sphere_center.x + r_xz*sinf(theta);
				p->m_pos[i].y = m_sphere_center.y + m_radius * cosf(phi);
				p->m_pos[i].w = 1.0f;
			}
		}


		void BasicColorGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_startCol[i] = glm::linearRand(m_minStartCol, m_maxStartCol);
				p->m_endCol[i] = glm::linearRand(m_minEndCol, m_maxEndCol);
			}
		}

		void BasicVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_vel[i] = glm::linearRand(m_minStartVel, m_maxStartVel);
			}
		}

		void ShootForceGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			
			for(size_t i = startId; i< endId;i++){
				float localvelY = glm::linearRand(m_minVelY,m_maxVelY);
				float localvelR = glm::linearRand(m_minVelR,m_maxVelR);
				p->m_vel[i] = glm::vec4(p->m_pos[i].x*localvelR, localvelY,p->m_pos[i].z*localvelR, 0.0f);
			}

		}

		void SphereVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			float phi, theta, v, r;
			for (size_t i = startId; i < endId; ++i)
			{
				phi = glm::linearRand(double(0.0f), M_PI);
				theta = glm::linearRand(-M_PI, M_PI);
				v = glm::linearRand(m_minVel, m_maxVel);

				float r_xz = m_radius*sinf(phi);
				p->m_vel[i].z = v*r_xz*cosf(theta);
				p->m_vel[i].x = v*r_xz*sinf(theta);
				p->m_vel[i].y = v*m_radius * cosf(phi);
			}
		}

		void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_minTime, m_maxTime);
				p->m_time[i].z = (float)0.0;
				p->m_time[i].w = (float)1.0 / p->m_time[i].x;
			}
		}
	}
}