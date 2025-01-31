#include "Particle.h"
#include "Boy/Environment.h"

Wog *Particle::spWogInstance = NULL;

Particle::Particle() {};

Particle::~Particle() {};

void Particle::init() { 
	spWogInstance = Wog::instance();
};