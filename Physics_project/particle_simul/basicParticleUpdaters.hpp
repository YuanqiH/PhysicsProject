#pragma once

#include <vector>
#include <math.h>
#include "basicParticles.hpp"

/*
class ParticleUpdater 
{
public:
	ParticleUpdater() { }
	virtual ~ParticleUpdater() { }

	virtual void update(double dt, ParticleData *p) = 0;
};
*/

namespace particles
{

	namespace updaters
	{
	class EulerUpdater : public ParticleUpdater
	{
	public:
		// globalAccelaeration could be used for gravity
		glm::vec4 m_globalAcceleration;//{ 0.0f };
		float m_k_drag;

	public:
		virtual void update(double dt, ParticleData *p) override;
		void claculate_globalAcc() {}
	};

	// collision with the floor :) todo: implement a collision model
	class FloorUpdater : public particles::ParticleUpdater
	{
	public:
		float m_floorY;//{ 0.0f };
		float m_bounceFactor;//{ 0.5f };
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	// this collision particle will sink into the ground
	class CollisionUpdater : public particles::ParticleUpdater
	{
	public:
			glm::vec4 m_floor_point;
			glm::vec4 m_floor_normal;
	public:
		virtual void update(double dt, ParticleData *p)override;
	};


	class AttractorUpdater : public particles::ParticleUpdater
	{
	protected:
		std::vector<glm::vec4> m_attractors; // .w is force
	public:
		virtual void update(double dt, ParticleData *p) override;

		size_t collectionSize() const { return m_attractors.size(); }
		void add(const glm::vec4 &attr) { m_attractors.push_back(attr); }
		glm::vec4 &get(size_t id) { return m_attractors[id]; }
	};

	class BasicColorUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	class PosColorUpdater : public ParticleUpdater
	{
	public:
		glm::vec4 m_minPos;//{ 0.0 };
		glm::vec4 m_maxPos;//{ 1.0 };
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	class VelColorUpdater : public ParticleUpdater
	{
	public:
		glm::vec4 m_minVel;//{ 0.0 };
		glm::vec4 m_maxVel;//{ 1.0 };
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	class BasicTimeUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	}
}