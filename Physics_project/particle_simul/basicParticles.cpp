#include "basicParticles.hpp"
#include <assert.h>
#include <algorithm>

namespace particles
{
	void ParticleData::generate(size_t maxSize)
	{
		m_count = maxSize;
		m_countAlive = 0;

		m_pos.reset(new glm::vec4[maxSize]);
		m_col.reset(new glm::vec4[maxSize]);
		m_startCol.reset(new glm::vec4[maxSize]);
		m_endCol.reset(new glm::vec4[maxSize]);
		m_vel.reset(new glm::vec4[maxSize]);
		m_acc.reset(new glm::vec4[maxSize]);
		m_time.reset(new glm::vec4[maxSize]);
		m_alive.reset(new bool[maxSize]);
	}

	void ParticleData::kill(size_t id)
	{
		if (m_countAlive > 0) // maybe this if can be removed?
		{
			m_alive[id] = false;
			swapData(id, m_countAlive - 1);
			m_countAlive--;
		}
	}

	void ParticleData::wake(size_t id)
	{
		if (m_countAlive < m_count) // maybe this if can be removed?
		{
			m_alive[id] = true;
			swapData(id, m_countAlive);
			m_countAlive++;
		}
	}

	void ParticleData::swapData(size_t a, size_t b)
	{
		std::swap(m_pos[a], m_pos[b]);
		std::swap(m_col[a], m_col[b]);
		std::swap(m_startCol[a], m_startCol[b]);
		std::swap(m_endCol[a], m_endCol[b]);
		std::swap(m_vel[a], m_vel[b]);
		std::swap(m_acc[a], m_acc[b]);
		std::swap(m_time[a], m_time[b]);
		std::swap(m_alive[a], m_alive[b]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// ParticleEmitter class 

	void ParticleEmitter::emit(double dt, ParticleData *p)
	{
		const size_t maxNewParticles = static_cast<size_t>(dt*m_emitRate);
		const size_t startId = p->m_countAlive;
		const size_t endId = std::min(startId + maxNewParticles, p->m_count-1);

		for (auto &gen : m_generators)
			gen->generate(dt, p, startId, endId);

		for (size_t i = startId; i < endId; ++i)
		{
			p->wake(i);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// ParticleSystem class 

	////////////////////////////////////////////////////////////////////////////////
	ParticleSystem::ParticleSystem(size_t maxCount)
	{
		m_count = maxCount;
		m_particles.generate(maxCount);
		m_aliveParticles.generate(maxCount);

		for (size_t i = 0; i < maxCount; ++i)
			m_particles.m_alive[i] = false;
	}

	void ParticleSystem::update(double dt)
	{
		for (auto & em : m_emitters)
		{
			em->emit(dt, &m_particles);
		}

		for (size_t i = 0; i < m_count; ++i)
		{
			m_particles.m_acc[i] = glm::vec4(0.0f);
		}

		for (auto & up : m_updaters)
		{
			up->update(dt, &m_particles);
		}

		//ParticleData::copyOnlyAlive(&m_particles, &m_aliveParticles);
	}

	void ParticleSystem::reset()
	{
		m_particles.m_countAlive = 0;
	}
	/*
	size_t ParticleSystem::computeMemoryUsage(const ParticleSystem &p)
	{
		//return 2 * ParticleData::computeMemoryUsage(p.m_particles);
	}
	*/
}