#pragma once

#include <memory>

namespace particles
{
	class ParticleSystem;	

	class GLParticleRenderer 
	{
	protected:
		ParticleSystem *m_system;//{ nullptr };

		unsigned int m_bufPos;// { 0 };
		unsigned int m_bufCol;//{ 0 };
		unsigned int m_vao; //{ 0 };
		bool ogl_ext_ARB_vertex_attrib_binding;

	public:
		GLParticleRenderer() { }
		~GLParticleRenderer() {  }

		void generate(ParticleSystem *sys, bool useQuads);
		void destroy() ;
		void update() ;
		void render() ;
	};
}