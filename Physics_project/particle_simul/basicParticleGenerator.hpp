#pragma once

#include <vector>
#include <math.h>
#include "basicParticles.hpp"

#define _USE_MATH_DEFINES

namespace particles
{
	namespace generators
	{
		//position generators
		class BoxPosGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_pos;
			glm::vec4 m_maxStartPosOffset;// { 0 };
		public:
			BoxPosGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class RoundPosGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_center;//{ 0.0 };
			float m_radX;//{ 0.0 };
			float m_radY;//{ 0.0 };
			float m_radZ;
		public:
			RoundPosGen() { }
			RoundPosGen(const glm::vec4 &center, double radX, double radY, double radZ): m_center(center), m_radX((float)radX), m_radY((float)radY), m_radZ((float)radZ){ }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class SpherePosGen:public ParticleGenerator
		{
		public:
			glm::vec4 m_sphere_center;
			float m_radius;
		public:
			SpherePosGen(){}

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};



		// color generators
		class BasicColorGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_minStartCol;//{ 0.0 };
			glm::vec4 m_maxStartCol;//{ 0.0 };
			glm::vec4 m_minEndCol;//{ 0.0 };
			glm::vec4 m_maxEndCol;//{ 0.0 };
		public:
			BasicColorGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};


		// velocity generators
		class BasicVelGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_minStartVel;//{ 0.0 };
			glm::vec4 m_maxStartVel;//{ 0.0 };
		public:
			BasicVelGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};
		
		class ShootForceGen : public ParticleGenerator
		{
		public:
			
			//glm::vec4 m_minVel;
			//glm::vec4 m_maxVel;
			float m_minVelY;
			float m_maxVelY;
			float m_minVelR;
			float m_maxVelR;

		public:
			ShootForceGen(){}
			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};
		
		class SphereVelGen : public ParticleGenerator
		{
		public:
			float m_minVel;//{ 0.0f };
			float m_maxVel;//{ 0.0f };
		protected:
			static const int m_radius = 1;
		public:
			SphereVelGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};


		// time generators
		class BasicTimeGen : public ParticleGenerator
		{
		public:
			float m_minTime;//{ 0.0 };
			float m_maxTime;//{ 0.0 };
		public:
			BasicTimeGen() { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};
	}
}