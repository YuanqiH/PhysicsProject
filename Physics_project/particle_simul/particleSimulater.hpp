#include "basicParticles.hpp"
#include "glParticleRenderer.hpp"
#include "basicParticleGenerator.hpp"
#include "basicParticleUpdaters.hpp"

#include <GL\glew.h>
#include <glm/vec4.hpp>

class particleSimulater

{
public:
	 particleSimulater(rsize_t MaxNumber);
	//~particleSimulater();
	
	particles::ParticleSystem *m_system;
	particles::GLParticleRenderer *renderer;
	int gravity_on;

	//way to create shared ptr
	std::shared_ptr<particles::updaters::AttractorUpdater> m_attractor;
	std::shared_ptr<particles::updaters::EulerUpdater> m_eulerUpdater;

public:
	void updateSys(double dt);
	void renderBegin();
	void renderUpdate();
	virtual void render();
	void renderEnd();

private:

};

 particleSimulater::particleSimulater(rsize_t MaxNumber)
{
	//create a particle system 
	//auto m_system = std::make_shared<particles::ParticleSystem>(MaxNumber);

	m_system = new particles::ParticleSystem(MaxNumber);

	//add some emitters
	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	{
    particleEmitter->m_emitRate = (float)MaxNumber*0.8f;

    // pos:
	/*
    auto posGenerator = std::make_shared<particles::generators::RoundPosGen>();
    posGenerator->m_center = glm::vec4( 0.0, 8.0, 0.0, 0.0 );
    posGenerator->m_radX = 0.1f;// radius 
    posGenerator->m_radY = 0.0f;
	posGenerator->m_radZ = 0.1f;
    particleEmitter->addGenerator(posGenerator);
	*/
	
	auto posGenerator = std::make_shared<particles::generators::SpherePosGen>();
	posGenerator->m_sphere_center = glm::vec4(0.0f,4.0f,0.0f,0.0f);
	posGenerator->m_radius = 0.5f;
	particleEmitter->addGenerator(posGenerator);
	
	//color
    auto colGenerator = std::make_shared<particles::generators::BasicColorGen>();
    colGenerator->m_minStartCol = glm::vec4( 0.7, 0.0, 0.7, 1.0 );
    colGenerator->m_maxStartCol = glm::vec4( 1.0, 1.0, 1.0, 1.0 );
    colGenerator->m_minEndCol = glm::vec4( 187/255, 133/255, 17/255, 1.0 );
    colGenerator->m_maxEndCol = glm::vec4( 0.7, 0.5, 1.0, 1.0 );
    particleEmitter->addGenerator(colGenerator);
	
	//velocity

	/*
    auto velGenerator = std::make_shared<particles::generators::BasicVelGen>();
    velGenerator->m_minStartVel = glm::vec4( 0.0f, 0.5f, 0.1f, 0.0f );
    velGenerator->m_maxStartVel = glm::vec4( 0.0f, 1.5f, 0.7f, 0.0f );
    particleEmitter->addGenerator(velGenerator);
	
	
	auto velGenerator = std::make_shared<particles::generators::SphereVelGen>();
	velGenerator ->m_maxVel = 1.0f;
	velGenerator-> m_minVel = 5.0f;
	particleEmitter->addGenerator(velGenerator);
	*/
	
	auto velGenerator = std::make_shared<particles::generators::ShootForceGen>();
	velGenerator->m_minVelY = 1.0f;
	velGenerator->m_maxVelY = 2.0f;
	velGenerator->m_minVelR = 3.0f;
	velGenerator->m_maxVelR = 3.0f;
	particleEmitter->addGenerator(velGenerator);
	
	//time
    auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
    timeGenerator->m_minTime = 10.0;
    timeGenerator->m_maxTime = 20.5;
    particleEmitter->addGenerator(timeGenerator);
	}//end of add emitter
	m_system->addEmitter(particleEmitter);

	//add some updaters
	auto timeUpdater = std::make_shared<particles::updaters::BasicTimeUpdater>();
	m_system->addUpdater(timeUpdater);

	
	auto colorUpdater = std::make_shared<particles::updaters::BasicColorUpdater>();
	m_system->addUpdater(colorUpdater);
	
	/*
	auto colorUpdater = std::make_shared<particles::updaters::PosColorUpdater>();
	colorUpdater->m_minPos = glm::vec4(0.0f,4.0f,0.0f,0.0f);
	colorUpdater->m_maxPos = glm::vec4(13.0f,4.0f,13.0f,0.0f);
	m_system->addUpdater(colorUpdater);
	*/

	//auto m_attractor = std::make_shared<particles::updaters::AttractorUpdater>();
	this->m_attractor = std::make_shared<particles::updaters::AttractorUpdater>();
	this->m_attractor->add(glm::vec4(5.0f,2.0f,5.0f, 10.0f));
	m_system->addUpdater(this->m_attractor);
	/*
	auto m_collisionUpdater = std::make_shared<particles::updaters::CollisionUpdater>();
	m_collisionUpdater->m_floor_normal = glm::vec4(0.0f,1.0f,0.0f,0.0f);// up vector
	m_collisionUpdater->m_floor_point = glm::vec4(0.0f,0.0f,0.0f,0.0f);
	m_system->addUpdater(m_collisionUpdater);
	*/

	auto m_collisionUpdater = std::make_shared<particles::updaters::FloorUpdater>();
	m_collisionUpdater->m_floorY = 0.0f;// up vector
	m_collisionUpdater->m_bounceFactor = 1.0f;
	m_system->addUpdater(m_collisionUpdater);

	m_eulerUpdater = std::make_shared<particles::updaters::EulerUpdater>();
	m_eulerUpdater->m_globalAcceleration = glm::vec4( 0.0, -3.0, 0.0, 0.0 );
	m_eulerUpdater->m_k_drag = 0.3f;
	m_system->addUpdater(m_eulerUpdater);



}

void particleSimulater::updateSys(double dt){
	
	this->m_system->update(dt);


}

void particleSimulater::renderBegin(){
	//auto renderer = std::shared_ptr<particles::GLParticleRenderer>();
	renderer = new particles::GLParticleRenderer();
	renderer->generate(this->m_system,0);

}

void particleSimulater::renderUpdate(){
	this->renderer->update();
}

void particleSimulater::render(){
	this->renderer->render();
}

void particleSimulater::renderEnd(){
	this->renderer->destroy();
}